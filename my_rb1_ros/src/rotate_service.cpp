#include "geometry_msgs/Twist.h"
#include "my_rb1_ros/Rotate.h"
#include "ros/init.h"
#include "ros/publisher.h"
#include "ros/rate.h"
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "tf/LinearMath/Matrix3x3.h"
#include "tf/LinearMath/Quaternion.h"
#include "tf/transform_datatypes.h"
#include <cmath>
#include <tf/tf.h>
#include <string>
class MyClass
{
public:

    ros::NodeHandle nh;
    ros::ServiceServer my_service;
    ros::Subscriber sub;
    ros::Publisher pub;
    geometry_msgs::Twist twist;

    bool result = false;
    double current_yaw;
    // double degree;
    double start_yaw;
    double converte_degrees;
    double target;
    double x, y, z, w;
    double roll, pitch, yaw;

    // double* ptr;

    MyClass(){
        pub =nh.advertise<geometry_msgs::Twist>("cmd_vel",1000);
        my_service = nh.advertiseService("/rotate_robot", &MyClass::my_callback,this);
        sub = nh.subscribe("odom", 10000, &MyClass::odom_callback,this);
    }

    void odom_callback(const nav_msgs::Odometry::ConstPtr& msg){
    x = msg->pose.pose.orientation.x;
    y = msg->pose.pose.orientation.y;
    z = msg->pose.pose.orientation.z;
    w = msg->pose.pose.orientation.w;
    tf::Quaternion quat(x, y, z, w);
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    current_yaw = yaw;
    // ROS_INFO("%f", current_yaw);
    }

    bool my_callback(my_rb1_ros::Rotate::Request &req,
                    my_rb1_ros::Rotate::Response &res
                    ) {
                
    ROS_INFO("Service Requested \n Request Data==> rotate_degrees= %d", req.degrees);
        ros::Rate rate(10);
        start_yaw = current_yaw;
        result = false;

    // degree = req.degrees;
    // rotate_degre(degree);
        while (result == false){
        // ROS_INFO("start while");
        converte_degrees = req.degrees*3.14159/180;
        
        if (start_yaw < 0 && (start_yaw + converte_degrees) < 0){
            if(req.degrees > 0){
              target = start_yaw + std::fabs(converte_degrees);}
            else {
              target = converte_degrees + start_yaw;
            }  
        }
        else{
         target = converte_degrees + start_yaw; 
        }
        if (std::fabs(target) > 3.1415) {
            if (target > 0) {
            target = target - (3.1415*2);
            }
            else{
            target = target + (3.1415*2);
            }
            
        }
            
            while (std::fabs(current_yaw - target) > 0.006) {
            if (req.degrees < 0){
            // ROS_INFO("start orientation = %f\n current orientation = %f target = %f" , start_yaw, current_yaw, target);
            twist.angular.z = -0.1;
            pub.publish(twist);            
            }
            else{
            // ROS_INFO("start orientation = %f\n current orientation = %f target = %f", start_yaw, current_yaw, target);
            twist.angular.z = 0.1;
            pub.publish(twist);         
            }
            ros::spinOnce();
            rate.sleep();   
            }
            
            twist.angular.z = 0;
            pub.publish(twist);
            result = true;
            ROS_INFO("Service Compleated ");
            res.result = "completed";
            }
        return true;
        }};





    int main(int argc, char **argv) {
    ros::init(argc, argv, "service_server_rotate_robot");
    MyClass myclass;
    ROS_INFO("Service Ready");
    ros::spin();
    return 0;
    }



