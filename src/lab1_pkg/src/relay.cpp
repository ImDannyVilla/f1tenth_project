#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"

using std::placeholders::_1;

class Relay : public rclcpp::Node //class relay inherits from rclcpp::Node
{
public:
    Relay()
    : Node("relay")
    {
        // 1. Create Subscriber
        // We listen to "drive". When a message arrives, 'topic_callback' is executed.
        subscription_ = this->create_subscription<ackermann_msgs::msg::AckermannDriveStamped>(
          "drive", 10, std::bind(&Relay::topic_callback, this, _1));

        // 2. Create Publisher
        // We will publish the modified data to "drive_relay"
        publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>("drive_relay", 10);

        RCLCPP_INFO(this->get_logger(), "Relay Node has started.");
    }

private:
    void topic_callback(const ackermann_msgs::msg::AckermannDriveStamped::SharedPtr msg) const
    {
        // A. Create a new message
        auto new_msg = ackermann_msgs::msg::AckermannDriveStamped();

        // B. Modify the data (The Lab Requirements: Multiply by 3)
        new_msg.drive.speed = msg->drive.speed * 3.0;
        new_msg.drive.steering_angle = msg->drive.steering_angle * 3.0;

        // C. Publish the new message
        publisher_->publish(new_msg);
    }

    // Member variables
    rclcpp::Subscription<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr subscription_;
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Relay>());
    rclcpp::shutdown();
    return 0;
}