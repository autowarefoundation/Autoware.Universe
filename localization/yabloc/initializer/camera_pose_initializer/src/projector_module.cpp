#include "camera_pose_initializer/projector_module.hpp"

#include <Eigen/Geometry>
#include <opencv2/imgproc.hpp>
#include <pcdless_common/cv_decompress.hpp>

namespace pcdless::initializer
{
ProjectorModule::ProjectorModule(rclcpp::Node * node)
: info_(node), tf_subscriber_(node->get_clock()), logger_(node->get_logger())
{
}

cv::Point2i to_cv_point(const Eigen::Vector3f & v)
{
  const float image_size_ = 800;
  const float max_range_ = 30;

  cv::Point pt;
  pt.x = -v.y() / max_range_ * image_size_ * 0.5f + image_size_ / 2.f;
  pt.y = -v.x() / max_range_ * image_size_ * 0.5f + image_size_ / 2.f;
  return pt;
}

cv::Mat ProjectorModule::project_image(const sensor_msgs::msg::Image & image_msg)
{
  cv::Mat mask_image = common::decompress_to_cv_mat(image_msg);

  // project semantics on plane
  std::vector<cv::Mat> masks;
  cv::split(mask_image, masks);
  std::vector<cv::Scalar> colors = {
    cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255)};

  cv::Mat projected_image = cv::Mat::zeros(cv::Size(800, 800), CV_8UC3);
  for (int i = 0; i < 3; i++) {
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(masks[i], contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    std::vector<std::vector<cv::Point> > projected_contours;
    for (auto contour : contours) {
      std::vector<cv::Point> projected;
      for (auto c : contour) {
        auto opt = project_func_(c);
        if (!opt.has_value()) continue;

        cv::Point2i pt = to_cv_point(opt.value());
        projected.push_back(pt);
      }
      if (projected.size() > 2) {
        projected_contours.push_back(projected);
      }
    }
    cv::drawContours(projected_image, projected_contours, -1, colors[i], -1);
  }
  return projected_image;
}

bool ProjectorModule::define_project_func()
{
  if (project_func_) return true;

  if (info_.is_camera_info_nullopt()) {
    RCLCPP_WARN_STREAM(logger_, "camera info is not ready");
    return false;
  }
  Eigen::Matrix3f Kinv = info_.intrinsic().inverse();

  std::optional<Eigen::Affine3f> camera_extrinsic =
    tf_subscriber_(info_.get_frame_id(), "base_link");
  if (!camera_extrinsic.has_value()) {
    RCLCPP_WARN_STREAM(logger_, "camera tf_static is not ready");
    return false;
  }

  const Eigen::Vector3f t = camera_extrinsic->translation();
  const Eigen::Quaternionf q(camera_extrinsic->rotation());

  // TODO: This will take into account ground tilt and camera vibration someday.
  project_func_ = [Kinv, q, t](const cv::Point & u) -> std::optional<Eigen::Vector3f> {
    Eigen::Vector3f u3(u.x, u.y, 1);
    Eigen::Vector3f u_bearing = (q * Kinv * u3).normalized();
    if (u_bearing.z() > -0.01) return std::nullopt;
    float u_distance = -t.z() / u_bearing.z();
    Eigen::Vector3f v;
    v.x() = t.x() + u_bearing.x() * u_distance;
    v.y() = t.y() + u_bearing.y() * u_distance;
    v.z() = 0;
    return v;
  };
  return true;
}
}  // namespace pcdless::initializer