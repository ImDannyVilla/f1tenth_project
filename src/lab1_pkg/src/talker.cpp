#include <chrono>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"
//Node 1: listens to two ROS topics and publishes to one
using namespace std::chrono_literals;

class Talker : public rclcpp::Node
{
public:
Talker() : Node("talker"){

    //1. Declare Parametrs (with default values)
//the lab asks for 'v' (speed) and 'd' (streering angle)
    //this-> is C++ pointer to the current object instance
    this->declare_parameter("v", 0.0);// means call he "declare_parameter method I inherited from rclcpp::Node on this current instance
    this->declare_parameter("d", 0.0);

    //2. Create Publisher
//Topic: "drive", Queue Size: 10
    publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>("drive", 10);

    //3. Create Timer
    //"publish as fast as possible" -> We set period to 0ms (runs continuously)
    timer_ = this->create_wall_timer(
        0ms, std::bind(&Talker::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "Talker Node has started.");
    }

private:
    void timer_callback()
    {
        //A. Get the current parameter values
        // We get them every loop so you can change them dynamically if you want
        double v = this->get_parameter("v").as_double();
        double d = this->get_parameter("d").as_double();

        //B. Create a message
        auto message = ackermann_msgs::msg::AckermannDriveStamped();
        message.drive.speed = v;
        message.drive.steering_angle = d;

        //c. Publish
        publisher_->publish(message);

        //Optional: Log it so we know it's working (dont do this too fast ro console floods)
        // RCLCPP_INFO(this->get_logger(), "Publishing: v=%.2f, d=%.2f", v, d);
    }

    //Member variables
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr publisher_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Talker>());
    rclcpp::shutdown();
return 0;
}