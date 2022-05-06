#include <geometry_msgs/msg/pose_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>

#include "sign_detector/fix2mgrs.hpp"
#include "sign_detector/ll2_util.hpp"
#include <eagleye_msgs/msg/heading.hpp>
#include <pcl-1.10/pcl/kdtree/kdtree_flann.h>
#include <pcl-1.10/pcl/point_cloud.h>
#include <tf2_ros/transform_broadcaster.h>

class Fix2Pose : public rclcpp::Node
{
public:
  Fix2Pose() : Node("fix_to_pose"), kdtree_(nullptr)
  {
    std::string map_topic = "/map/vector_map";
    std::string fix_topic = "/eagleye/fix";
    std::string pose_topic = "/eagleye/pose";
    std::string heading_topic = "/eagleye/heading_interpolate_3rd";

    this->declare_parameter<std::string>("map_topic", map_topic);
    this->declare_parameter<std::string>("fix_topic", fix_topic);
    this->declare_parameter<std::string>("pose_topic", pose_topic);
    this->declare_parameter<std::string>("heading_topic", heading_topic);
    this->get_parameter("map_topic", map_topic);
    this->get_parameter("fix_topic", fix_topic);
    this->get_parameter("pose_topic", pose_topic);
    this->get_parameter("heading_topic", heading_topic);

    sub_heading_ = this->create_subscription<eagleye_msgs::msg::Heading>(heading_topic, 10, std::bind(&Fix2Pose::headingCallback, this, std::placeholders::_1));
    sub_map_ = this->create_subscription<autoware_auto_mapping_msgs::msg::HADMapBin>(map_topic, rclcpp::QoS(10).transient_local().reliable(), std::bind(&Fix2Pose::mapCallback, this, std::placeholders::_1));
    sub_fix_ = this->create_subscription<sensor_msgs::msg::NavSatFix>(fix_topic, 10, std::bind(&Fix2Pose::fixCallback, this, std::placeholders::_1));
    pub_pose_stamped_ = this->create_publisher<geometry_msgs::msg::PoseStamped>(pose_topic, 10);

    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

    pose_topic_ = pose_topic;
  }

private:
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  std::string pose_topic_;

  void publishTf(const geometry_msgs::msg::PoseStamped& pose, const rclcpp::Time&)
  {
    geometry_msgs::msg::TransformStamped t;

    t.header.stamp = this->get_clock()->now();
    t.header.frame_id = "map";
    t.child_frame_id = pose_topic_;
    t.transform.translation.x = pose.pose.position.x;
    t.transform.translation.y = pose.pose.position.y;
    t.transform.translation.z = pose.pose.position.z;
    t.transform.rotation.w = pose.pose.orientation.w;
    t.transform.rotation.x = pose.pose.orientation.x;
    t.transform.rotation.y = pose.pose.orientation.y;
    t.transform.rotation.z = pose.pose.orientation.z;

    tf_broadcaster_->sendTransform(t);
  }

  void headingCallback(const eagleye_msgs::msg::Heading& msg)
  {
    heading_ = msg.heading_angle;
  }

  void fixCallback(const sensor_msgs::msg::NavSatFix& msg)
  {
    if (!kdtree_) return;

    Eigen::Vector3d mgrs = fix2Mgrs(msg);
    pcl::PointXYZ p;
    p.x = mgrs.x();
    p.y = mgrs.y();
    p.z = mgrs.z();

    constexpr int K = 10;
    std::vector<int> indices;
    std::vector<float> distances;
    kdtree_->nearestKSearch(p, K, indices, distances);

    float height = std::numeric_limits<float>::max();
    for (int index : indices) {
      Eigen::Vector3f v = cloud_->at(index).getVector3fMap();
      height = std::min(height, v.z());
    }

    RCLCPP_INFO_STREAM(this->get_logger(), mgrs.x() << " " << mgrs.y() << " (" << msg.latitude << ", " << msg.longitude << ") " << height);

    geometry_msgs::msg::PoseStamped pose;
    pose.header.frame_id = "map";
    pose.header.stamp = msg.header.stamp;
    pose.pose.position.x = p.x;
    pose.pose.position.y = p.y;
    pose.pose.position.z = height;

    pose.pose.orientation.w = std::cos(0.5f * (M_PI_2 - heading_));
    pose.pose.orientation.z = std::sin(0.5f * (M_PI_2 - heading_));

    pub_pose_stamped_->publish(pose);

    publishTf(pose, msg.header.stamp);
  }

  void mapCallback(const autoware_auto_mapping_msgs::msg::HADMapBin& msg)
  {
    lanelet::LaneletMapPtr viz_lanelet_map = fromBinMsg(msg);

    RCLCPP_INFO_STREAM(this->get_logger(), "lanelet: " << viz_lanelet_map->laneletLayer.size());
    RCLCPP_INFO_STREAM(this->get_logger(), "line: " << viz_lanelet_map->lineStringLayer.size());
    RCLCPP_INFO_STREAM(this->get_logger(), "point: " << viz_lanelet_map->pointLayer.size());

    kdtree_ = boost::make_shared<pcl::KdTreeFLANN<pcl::PointXYZ>>();

    auto toPointXYZ = [](const lanelet::ConstPoint3d& p) -> pcl::PointXYZ {
      pcl::PointXYZ q;
      q.x = p.x();
      q.y = p.y();
      q.z = p.z();
      return q;
    };

    cloud_ = boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>();
    for (lanelet::LineString3d& line : viz_lanelet_map->lineStringLayer) {
      if (!line.hasAttribute(lanelet::AttributeName::Type)) continue;
      lanelet::Attribute attr = line.attribute(lanelet::AttributeName::Type);
      if (attr.value() != "line_thin") continue;

      for (const lanelet::ConstPoint3d& p : line)
        cloud_->push_back(toPointXYZ(p));
    }
    kdtree_->setInputCloud(cloud_);
  }

  rclcpp::Subscription<autoware_auto_mapping_msgs::msg::HADMapBin>::SharedPtr sub_map_;
  rclcpp::Subscription<eagleye_msgs::msg::Heading>::SharedPtr sub_heading_;
  rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr sub_fix_;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pub_pose_stamped_;
  float heading_;
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_;
  pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr kdtree_;
};

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<Fix2Pose>());
  rclcpp::shutdown();
  return 0;
}
