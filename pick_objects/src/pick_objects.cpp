#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <std_msgs/UInt8.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "simple_navigation_goals");
  ros::NodeHandle n;

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //set up publisher to broadcast if robot is at pick up location
  ros::Publisher location_pub = n.advertise<std_msgs::UInt8>("/destination_reached", 1);

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal pick;
  move_base_msgs::MoveBaseGoal drop;
  
  // set up the frame parameters
  pick.target_pose.header.frame_id = "map";
  pick.target_pose.header.stamp = ros::Time::now();

  drop.target_pose.header.frame_id = "map";
  drop.target_pose.header.stamp = ros::Time::now();

  // Define a position and orientation for the robot to reach at each position
  pick.target_pose.pose.position.x = -4.0;
  pick.target_pose.pose.position.y = 4.12;
  pick.target_pose.pose.orientation.w = 1.0;

  drop.target_pose.pose.position.x = -2;
  drop.target_pose.pose.position.y = -4.5;
  drop.target_pose.pose.orientation.w = 1.0;

  // Send the first position and orientation for the robot to reach
  ROS_INFO("Robot is traveling to pickup location.");
  ac.sendGoal(pick);

  // Waiting until take a results
  ac.waitForResult();

  // Check if the robot reached virtual object
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    ROS_INFO("Robot picked up virtual object.");
    std_msgs::UInt8 msg;
    msg.data = 1;
    location_pub.publish(msg);

    sleep(5);
  }
  else
  {
    ROS_INFO("The robot failed to reach the pick up area");
    return 0;
  }
  
  ROS_INFO("Robot is traveling to drop off location.");
  ac.sendGoal(drop);
  
  // Waiting until take a results
  ac.waitForResult();
  
  // Check if the robot reached the virtual object
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    ROS_INFO("Robot droped off virtual object.");
    std_msgs::UInt8 msg2;
    msg2.data = 3;
    location_pub.publish(msg2);
  }
  else
  {
    ROS_INFO("The robot failed to reach the drop off area");
  }

  sleep(5);
  return 0;
}
