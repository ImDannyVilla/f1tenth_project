#include "rclcpp/rclcpp.hpp"
/// CHECK: include needed ROS msg type headers and libraries
#include "sensor_msgs/msg/laser_scan.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"
#include <cmath>
#include <limits>


class Safety : public rclcpp::Node {
// The class that handles emergency braking

public:
    Safety() : Node("safety_node")
    {
        /*
        You should also subscribe to the /scan topic to get the
        sensor_msgs/LaserScan messages and the /ego_racecar/odom topic to get
        the nav_msgs/Odometry messages

        The subscribers should use the provided odom_callback and 
        scan_callback as callback methods

        NOTE that the x component of the linear velocity in odom is the speed
        */
        /* odom_callback and scan_callback can run at the same time on different threads
         * — one writing speed_, the other reading it.
         */
        scan_cb_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        odo_cb_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

        /// TODO: create ROS subscribers and publishers
        /// creating a pointer to the saftey Node, and a subscritpion the base class provides for making a new subscription.
        /// that belongs to the safety node class
        rclcpp::SubscriptionOptions odo_opts;
        odo_opts.callback_group = odo_cb_group_;
        odo_subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/ego_racecar/odom",
            rclcpp::QoS(10),
            std::bind(&Safety::odom_callback, this, std::placeholders::_1),
            odo_opts);

        rclcpp::SubscriptionOptions scan_opts;
        scan_opts.callback_group = scan_cb_group_;
        scan_subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", // which topic? /scan
            //use SensorDataQoS for sensor topics
            // How many incoming messages get held if our callback is too slow to process them in time.
            rclcpp::SensorDataQoS(), //important to use for best_effort UDP type, volatile druability, and keep_last depth 5
            std::bind(&Safety::scan_callback, this, std::placeholders::_1), // callable - what to do when a message arrives?
            scan_opts);

        publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>("/drive", 10);
        //10, how many outgoing messages get buffered if subscribers can't keep up.


    }

private:
    std::atomic<double> speed_ = 0.0;
    /// TODO: create ROS subscribers and publishers

    // odom_callback writes speed_; scan_callback reads it.
    void odom_callback(const nav_msgs::msg::Odometry::ConstSharedPtr msg)
    {
        /// TODO: update current speed
        speed_ = msg->twist.twist.linear.x;
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
            "Speed: %.2f", speed_.load());
    }

    void scan_callback(const sensor_msgs::msg::LaserScan::ConstSharedPtr msg)
    {
        double v = speed_.load();
        RCLCPP_INFO_THROTTLE(
            this->get_logger(), *this->get_clock(), 100,
            "scan in: %zu beams | angle [%.2f, %.2f] rad | speed: %.2f m/s",
            msg->ranges.size(), msg->angle_min, msg->angle_max, v);

        double angle_min = msg->angle_min;
        double angle_increment = msg->angle_increment;

        bool should_brake = false;
        double braking_threshold = .5;
        double min_ttc = std::numeric_limits<double>::infinity();

        //walking the beams
        const double fov_half = .35;
        for (size_t i {}; i < msg->ranges.size(); ++i) {
            double theta = angle_min + i * angle_increment; //this beams angle theta_i
            if (std::abs(theta) > fov_half) continue; //ignoring side beams
            double r = msg->ranges[i];
            if (!std::isfinite(r)) continue;
            double closing = v * std::cos(theta); //closing speed for this beam
            /// TODO: calculate TTC (distance / closing speed)
            if (closing > 0.0) {
                double ttc = r / closing;
                if (ttc < min_ttc) min_ttc = ttc;

                if (ttc < braking_threshold) {
                    should_brake = true;
                   RCLCPP_ERROR(this->get_logger(),
                       "BRAKE: beam %zu, r=%.2f m, theta=%.2f rad, closing=%.2f m/s, ttc=%.3f s",
                       i, r, theta, closing, ttc);
                    break;
                }
            }
        }
        RCLCPP_DEBUG(this->get_logger(), "min_ttc this scan: %.3f s", min_ttc);
        /// TODO: publish drive/brake message
        if (should_brake) {
            auto brake = ackermann_msgs::msg::AckermannDriveStamped();
            brake.drive.speed = 0.0;
            publisher_->publish(brake);
        }
    }

    //created separate groups to prevent scan fom waiting on odom to finish (parallism)
    rclcpp::CallbackGroup::SharedPtr scan_cb_group_;
    rclcpp::CallbackGroup::SharedPtr odo_cb_group_;

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odo_subscription_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_subscription_;
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr publisher_;



};
int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    rclcpp::executors::MultiThreadedExecutor executor;
    // The executor only holds a weak_ptr to the node, so we must keep a
    // shared_ptr alive ourselves for the lifetime of spin(). Passing a
    // temporary make_shared<>() directly would destroy the node immediately.
    auto node = std::make_shared<Safety>();
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}