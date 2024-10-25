#include "geometry_msgs/Twist.h"
#include "my_rb1_ros/Rotate.h"
#include "ros/init.h"
#include "ros/publisher.h"
#include "ros/rate.h"
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "ros/subscriber.h"
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
    double start_yaw;
    double converte_degrees;
    double target;
    double x, y, z, w;
    double roll, pitch, yaw;
    double pi = M_PI;
    float az;

    MyClass(){
        pub =nh.advertise<geometry_msgs::Twist>("cmd_vel",5);
        my_service = nh.advertiseService("/rotate_robot", &MyClass::my_callback,this);
        sub = nh.subscribe("odom", 5, &MyClass::odom_callback,this);
    }
    

    void odom_callback(const nav_msgs::Odometry::ConstPtr& msg){
    x = msg->pose.pose.orientation.x;
    y = msg->pose.pose.orientation.y;
    z = msg->pose.pose.orientation.z;
    w = msg->pose.pose.orientation.w;
    tf::Quaternion quat(x, y, z, w);
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    current_yaw = yaw;
    if (current_yaw < 0){
      current_yaw = current_yaw + 2*(pi);}
    }

    bool my_callback(my_rb1_ros::Rotate::Request &req,
                    my_rb1_ros::Rotate::Response &res
                    ) {
                
    ROS_INFO("Service Requested \n rotate_degrees= %d", req.degrees);
        ros::Rate rate(100);
        start_yaw = current_yaw;
        if (start_yaw < 0){
          start_yaw = start_yaw + 2*(pi);}
        result = false;


        while (result == false){
        converte_degrees = req.degrees*pi/180;
        target = start_yaw + converte_degrees;
        if(target > 2*(pi)){
          target = target - (2*pi);
        }

        if (target < 0){
          target = target + 2*(pi);}

            while (std::fabs(target - current_yaw) > 0.0005) {
            az = std::fabs(target - current_yaw)*2 + 0.01;
            if (az > 0.7){
              az=0.7;}
            // ROS_INFO(" speed = %f ", az);
              
            // ROS_INFO("start orientation = %f\n current orientation = %f target = %f" , start_yaw, current_yaw, target);
        if (req.degrees < 0){
        az = az*-1; 
        }
            twist.angular.z = az;
            pub.publish(twist);             
            ros::spinOnce();
            rate.sleep();   
            }
            
            twist.angular.z = 0;
            pub.publish(twist);
            result = true;
            ROS_INFO("Service Compleated ");
            res.result = "Service/rotate_robot: succeeded,  " + std::to_string(req.degrees) + " degrees" ;
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



