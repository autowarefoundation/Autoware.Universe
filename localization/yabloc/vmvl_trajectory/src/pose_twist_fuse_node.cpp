#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

class PoseTwistFuser : public rclcpp::Node
{
public:
  using PoseStamped = geometry_msgs::msg::PoseStamped;
  using TwistStamped = geometry_msgs::msg::TwistStamped;
  using Odometry = nav_msgs::msg::Odometry;

  PoseTwistFuser() : Node("pose_tiwst_fuser")
  {
    using std::placeholders::_1;

    // Subscriber
    auto on_twist = [this](const TwistStamped & msg) -> void { this->latest_twist_stamped_ = msg; };
    auto on_pose = std::bind(&PoseTwistFuser::onPose, this, _1);
    sub_twist_stamped_ = create_subscription<TwistStamped>("twist", 10, std::move(on_twist));
    sub_pose_stamped_ = create_subscription<PoseStamped>("pose", 10, std::move(on_pose));

    // Publisher
    pub_odometry_ = this->create_publisher<Odometry>("odometry", 10);
  }

private:
  rclcpp::Subscription<TwistStamped>::SharedPtr sub_twist_stamped_;
  rclcpp::Subscription<PoseStamped>::SharedPtr sub_pose_stamped_;
  rclcpp::Publisher<Odometry>::SharedPtr pub_odometry_;
  std::optional<TwistStamped> latest_twist_stamped_{std::nullopt};

  void onPose(const PoseStamped & pose)
  {
    if (!latest_twist_stamped_.has_value()) return;

    Odometry msg;
    msg.header = pose.header;
    msg.pose.pose = pose.pose;
    msg.twist.twist = latest_twist_stamped_->twist;
    pub_odometry_->publish(msg);
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PoseTwistFuser>());
  rclcpp::shutdown();
  return 0;
}
