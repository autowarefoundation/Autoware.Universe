#include "common/util.hpp"
#include "imgproc/segment_filter.hpp"

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <pcl/filters/extract_indices.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

namespace imgproc
{
SegmentFilter::SegmentFilter()
: Node("segment_filter"),
  subscriber_(rclcpp::Node::SharedPtr{this}, "lsd_cloud", "graph_segmented"),
  tf_subscriber_(this->get_clock()),
  image_size_(declare_parameter<int>("image_size", 800)),
  max_range_(declare_parameter<float>("max_range", 20.f))
{
  using std::placeholders::_1;
  using std::placeholders::_2;
  auto cb = std::bind(&SegmentFilter::execute, this, _1, _2);
  subscriber_.setCallback(cb);
  auto cb_info = [this](const CameraInfo & msg) -> void { info_ = msg; };
  sub_info_ =
    create_subscription<CameraInfo>("/sensing/camera/traffic_light/camera_info", 10, cb_info);

  pub_cloud_ = create_publisher<PointCloud2>("/lsd_cloud3", 10);
  pub_image_ = create_publisher<Image>("/projected_image2", 10);
}

cv::Point2i SegmentFilter::toCvPoint(const Eigen::Vector3f & v) const
{
  cv::Point pt;
  pt.x = -v.y() / max_range_ * image_size_ * 0.5f + image_size_ / 2;
  pt.y = -v.x() / max_range_ * image_size_ * 0.5f + image_size_;
  return pt;
}

void SegmentFilter::execute(const PointCloud2 & lsd_msg, const PointCloud2 & segment_msg)
{
  if (!info_.has_value()) return;

  const rclcpp::Time stamp = lsd_msg.header.stamp;
  pcl::PointCloud<pcl::PointXYZ>::Ptr mask{new pcl::PointCloud<pcl::PointXYZ>()};
  pcl::PointCloud<pcl::PointNormal>::Ptr lsd{new pcl::PointCloud<pcl::PointNormal>()};
  pcl::fromROSMsg(segment_msg, *mask);
  pcl::fromROSMsg(lsd_msg, *lsd);

  cv::Size size(info_->width, info_->height);

  Eigen::Matrix3f K =
    Eigen::Map<Eigen::Matrix<double, 3, 3>>(info_->k.data()).cast<float>().transpose();
  Eigen::Matrix3f Kinv = K.inverse();

  auto camera_extrinsic = tf_subscriber_(info_->header.frame_id, "base_link");
  if (!camera_extrinsic.has_value()) return;
  if (mask->empty()) {
    RCLCPP_WARN_STREAM(this->get_logger(), "there are no contours");
    return;
  }
  const Eigen::Vector3f t = camera_extrinsic->translation();
  const Eigen::Quaternionf q(camera_extrinsic->rotation());

  ProjectFunc project_func = [Kinv, q,
                              t](const Eigen::Vector3f & u) -> std::optional<Eigen::Vector3f> {
    Eigen::Vector3f u3(u.x(), u.y(), 1);
    Eigen::Vector3f u_bearing = (q * Kinv * u3).normalized();
    if (u_bearing.z() > -0.01) return std::nullopt;
    float u_distance = -t.z() / u_bearing.z();
    Eigen::Vector3f v;
    v.x() = t.x() + u_bearing.x() * u_distance;
    v.y() = t.y() + u_bearing.y() * u_distance;
    v.z() = 0;
    return v;
  };

  pcl::PointCloud<pcl::PointXYZ>::Ptr projected_mask{new pcl::PointCloud<pcl::PointXYZ>()};
  pcl::PointCloud<pcl::PointNormal>::Ptr projected_lines{new pcl::PointCloud<pcl::PointNormal>()};
  *projected_mask = projectMask(*mask, project_func);
  *projected_lines = projectLines(*lsd, project_func);

  pcl::PointIndices indices = filtByMask(*projected_mask, *projected_lines);

  // Publish point cloud
  pcl::ExtractIndices<pcl::PointNormal> extract;
  pcl::PointIndicesPtr indices_ptr = boost::make_shared<pcl::PointIndices>(indices);
  extract.setIndices(indices_ptr);
  extract.setInputCloud(projected_lines);
  pcl::PointCloud<pcl::PointNormal> reliable_edges;
  extract.filter(reliable_edges);
  util::publishCloud(*pub_cloud_, reliable_edges, stamp);

  // Make image
  cv::Mat reliable_line_image = cv::Mat::zeros(cv::Size{image_size_, image_size_}, CV_8UC3);
  for (size_t i = 0; i < projected_lines->size(); i++) {
    auto & pn = projected_lines->at(i);
    cv::Point2i p1 = toCvPoint(pn.getVector3fMap());
    cv::Point2i p2 = toCvPoint(pn.getNormalVector3fMap());
    cv::Scalar color = cv::Scalar(255, 255, 255);
    cv::line(reliable_line_image, p1, p2, color, 2, cv::LineTypes::LINE_8);
  }
  util::publishImage(*pub_image_, reliable_line_image, stamp);
}

std::set<ushort> getUniquePixelValue(cv::Mat & image)
{
  // NOTE: What is this?
  auto begin = image.begin<ushort>();
  auto last = std::unique(begin, image.end<ushort>());
  std::sort(begin, last);
  last = std::unique(begin, last);
  return std::set<ushort>(begin, last);
}

pcl::PointCloud<pcl::PointXYZ> SegmentFilter::projectMask(
  const pcl::PointCloud<pcl::PointXYZ> & points, ProjectFunc project) const
{
  pcl::PointCloud<pcl::PointXYZ> projected_points;
  for (const auto & p : points) {
    std::optional<Eigen::Vector3f> opt = project(p.getVector3fMap());
    if (!opt.has_value()) continue;
    pcl::PointXYZ xyz(opt->x(), opt->y(), opt->z());
    projected_points.push_back(xyz);
  }
  return projected_points;
}

pcl::PointCloud<pcl::PointNormal> SegmentFilter::projectLines(
  const pcl::PointCloud<pcl::PointNormal> & points, ProjectFunc project) const
{
  pcl::PointCloud<pcl::PointNormal> projected_points;
  for (const auto & pn : points) {
    std::optional<Eigen::Vector3f> opt1 = project(pn.getVector3fMap());
    std::optional<Eigen::Vector3f> opt2 = project(pn.getNormalVector3fMap());
    if (!opt1.has_value()) continue;
    if (!opt2.has_value()) continue;
    pcl::PointNormal xyz;
    xyz.x = opt1->x();
    xyz.y = opt1->y();
    xyz.z = opt1->z();
    xyz.normal_x = opt2->x();
    xyz.normal_y = opt2->y();
    xyz.normal_z = opt2->z();
    projected_points.push_back(xyz);
  }
  return projected_points;
}

pcl::PointIndices SegmentFilter::filtByMask(
  const pcl::PointCloud<pcl::PointXYZ> & mask, const pcl::PointCloud<pcl::PointNormal> & edges)
{
  // Create line image and assign different color to each segment.
  cv::Mat line_image = cv::Mat::zeros(cv::Size{image_size_, image_size_}, CV_16UC1);
  for (size_t i = 0; i < edges.size(); i++) {
    auto & pn = edges.at(i);
    cv::Point2i p1 = toCvPoint(pn.getVector3fMap());
    cv::Point2i p2 = toCvPoint(pn.getNormalVector3fMap());
    cv::Scalar color = cv::Scalar::all(i + 1);
    cv::line(line_image, p1, p2, color, 1, cv::LineTypes::LINE_4);
  }

  // Create mask image
  cv::Mat mask_image = cv::Mat::zeros(cv::Size{image_size_, image_size_}, CV_16UC1);
  std::vector<std::vector<cv::Point2i>> projected_hull(1);
  for (const auto p : mask)
    projected_hull.front().push_back(toCvPoint(Eigen::Vector3f(p.x, p.y, p.z)));
  auto max_color = cv::Scalar::all(std::numeric_limits<u_short>::max());
  cv::drawContours(mask_image, projected_hull, 0, max_color, -1);

  // And operator
  cv::Mat masked_line;
  cv::bitwise_and(mask_image, line_image, masked_line);
  std::set<ushort> pixel_values = getUniquePixelValue(masked_line);

  // Extract edges within masks
  pcl::PointIndices reliable_indices;
  for (size_t i = 0; i < edges.size(); i++) {
    auto & pn = edges.at(i);
    cv::Point2i p1 = toCvPoint(pn.getVector3fMap());
    cv::Point2i p2 = toCvPoint(pn.getNormalVector3fMap());
    int line_thick = 2;
    if (pixel_values.count(i + 1) != 0) {
      reliable_indices.indices.push_back(i);
    }
  }

  return reliable_indices;
}

}  // namespace imgproc

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<imgproc::SegmentFilter>());
  rclcpp::shutdown();
  return 0;
}
