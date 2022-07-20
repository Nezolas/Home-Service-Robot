# Home-Service-Robot

Execute the following commands in the workspace terminal to set it up for the project:
```
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
catkin_init_workspace
cd ..
catkin_make
sudo apt-get update
cd ~/catkin_ws/src
git clone https://github.com/ros-perception/slam_gmapping
git clone https://github.com/turtlebot/turtlebot
git clone https://github.com/turtlebot/turtlebot_interactions
git clone https://github.com/turtlebot/turtlebot_simulator
cd ~/catkin_ws/
source devel/setup.bash
rosdep -i install gmapping
rosdep -i install turtlebot_teleop
rosdep -i install turtlebot_rviz_launchers
rosdep -i install turtlebot_gazebo
catkin_make
source devel/setup.bash
```


## Shell Scripts
A shell script is a file containing a series of commands and could be executed. It is commonly used to set up environment, run a program, etc.

You already know how to build a roslaunch file. It is very convenient to launch multiple ROS nodes and set parameters from a single roslaunch command. However, when developing robotic software with different packages, it might get harder to track errors and bugs generated from different nodes.

That's when shell scripts come in handy! After you create a shell script file to launch one or many nodes each in separate terminals, you will have the power to track the output of different nodes and keep the convenience of running a single command to launch all nodes.

### Your launch.sh Script
Let us start by creating this launch.sh script in the Udacity Workspace. Its goal is to launch Gazebo and Rviz in separate instances of terminals. Note that we are using xterm terminal in the script here.
```
#!/bin/sh
xterm  -e  " gazebo " &
sleep 5
xterm  -e  " source /opt/ros/kinetic/setup.bash; roscore" & 
sleep 5
xterm  -e  " rosrun rviz rviz" 
```
The launch.sh shell script launches three terminals and issues one or multiple commands in each terminal. Let’s break down this script to understand the meaning of each line.

### Code Breakdown
`#!/bin/sh`

This statement is called a shebang. It must be included in every shell script you write since it specifies the full path of the UNIX interpreter to execute it.

`xterm -e " gazebo " &`

With the xterm -e statement, we launch a new instance of an xterminal. Inside this terminal, we launch gazebo using the command "gazebo". Then we add an ampersand & to indicate that another instance of an xterm terminal will be created in a separate statement.

sleep 5

We pause this script for 5 seconds using sleep.

`xterm -e " source /opt/ros/kinetic/setup.bash; roscore" &`

We launch a second instance of the xterm terminal. Inside this terminal, we source the ROS workspace and launch the ROS master node.

sleep 5

We pause this script for another 5 seconds.

`xterm -e " rosrun rviz rviz"`

We are launching a third instance of the xterm terminal, and running rviz.

Save your script file and give it execute pemission by chmod +x launch.sh. Then launch the shell script with ./launch.sh.

## Simulation Setup
### Catkin Workspace
To program your home service robot, you will need to interface it with different ROS packages. Some of these packages are official ROS packages which offer great tools and others are packages that you’ll create. The goal of this section is to prepare and build your catkin workspace.

Here’s the list of the **official ROS packages** that you will need to grab, and other packages and directories that you’ll need to create at a later stage as you go through the project. Your catkin_ws/src directory should look as follows:

### Official ROS packages
Import these packages now and install them in the src directory of your catkin workspace. Be sure to clone the full GitHub directory and not just the package itself.

1.[gmapping](http://wiki.ros.org/gmapping): With the **gmapping_demo.launch** file, you can easily perform SLAM and build a map of the environment with a robot equipped with laser range finder sensors or RGB-D cameras.

2.[turtlebot_teleop](http://wiki.ros.org/turtlebot_teleop): With the **keyboard_teleop.launch** file, you can manually control a robot using keyboard commands.

3.[turtlebot_rviz_launchers](http://wiki.ros.org/turtlebot_rviz_launchers): With the **view_navigation.launch** file, you can load a preconfigured rviz workspace. You’ll save a lot of time by launching this file, because it will automatically load the robot model, trajectories, and map for you.

4.[turtlebot_gazebo](http://wiki.ros.org/turtlebot_gazebo): With the **turtlebot_world.launch** you can deploy a turtlebot in a gazebo environment by linking the world file to it.

Your Packages and Directories
You’ll install these packages and create the directories as you go through the project.

1.**map**: Inside this directory, you will store your gazebo world file and the map generated from SLAM.

2.**scripts**: Inside this directory, you’ll store your shell scripts.

3.**rvizConfig**: Inside this directory, you’ll store your customized rviz configuration files.

4.**pick_objects**: You will write a node that commands your robot to drive to the pickup and drop off zones.

5.**add_markers**: You will write a node that model the object with a marker in rviz.

Your package should look like this now:

    ├──                                # Official ROS packages
    |
    ├── slam_gmapping                  # gmapping_demo.launch file                   
    │   ├── gmapping
    │   ├── ...
    ├── turtlebot                      # keyboard_teleop.launch file
    │   ├── turtlebot_teleop
    │   ├── ...
    ├── turtlebot_interactions         # view_navigation.launch file      
    │   ├── turtlebot_rviz_launchers
    │   ├── ...
    ├── turtlebot_simulator            # turtlebot_world.launch file 
    │   ├── turtlebot_gazebo
    │   ├── ...
    ├──                                # Your packages and direcotries
    |
    ├── map                          # map files
    │   ├── ...
    ├── scripts                   # shell scripts files
    │   ├── ...
    ├──rvizConfig                      # rviz configuration files
    │   ├── ...
    ├──pick_objects                    # pick_objects C++ node
    │   ├── src/pick_objects.cpp
    │   ├── ...
    ├──add_markers                     # add_marker C++ node
    │   ├── src/add_markers.cpp
    │   ├── ...
    └──

### Reaching Multiple Goals

Earlier, you tested your robot capabilities in reaching multiple goals by manually commanding it to travel with the 2D NAV Goal arrow in rviz. Now, you will write a node that will communicate with the ROS navigation stack and autonomously send successive goals for your robot to reach. As mentioned earlier, the ROS navigation stack creates a path for your robot based on Dijkstra's algorithm, a variant of the Uniform Cost Search algorithm, while avoiding obstacles on its path.

There is an official ROS tutorial that teaches you how to send a single goal position and orientation to the navigation stack. You are already familiar with this code from the Localization project where you used it to send your robot to a pre-defined goal. Check out the [tutorial](http://wiki.ros.org/navigation/Tutorials/SendingSimpleGoals) and go through its documentation.

Here’s the C++ code of this node which sends a single goal for the robot to reach. I included some extra comments to help you understand it:

```
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "simple_navigation_goals");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  // set up the frame parameters
  goal.target_pose.header.frame_id = "base_link";
  goal.target_pose.header.stamp = ros::Time::now();

  // Define a position and orientation for the robot to reach
  goal.target_pose.pose.position.x = 1.0;
  goal.target_pose.pose.orientation.w = 1.0;

   // Send the goal position and orientation for the robot to reach
  ROS_INFO("Sending goal");
  ac.sendGoal(goal);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, the base moved 1 meter forward");
  else
    ROS_INFO("The base failed to move forward 1 meter for some reason");

  return 0;
}
```
## Modeling Virtual Objects

The final task of this project is to model a virtual object with markers in rviz. The virtual object is the one being picked and delivered by the robot, thus it should first appear in its pickup zone, and then in its drop off zone once the robot reaches it.

First, let’s see how markers can be drawn in rviz. Luckily, there’s an official ROS tutorial that teaches you how to do it. The tutorial is an excellent reference and includes a C++ node capable of drawing basic shapes like arrows, cubes, cylinders, and spheres in rviz. You will learn how to define a marker, scale it, define its position and orientation, and finally publish it to rviz. The node included in the tutorial will publish a different shape each second at the same position and orientation. Check out the tutorial and go through the documentation to get started.

You will need to first run this node and visualize the markers in rviz. Then you’ll need to modify the code and publish a single shape example: a cube. Your code should follow this algorithm:

-Publish the marker at the pickup zone

-Pause 5 seconds

-Hide the marker

-Pause 5 seconds

-Publish the marker at the drop off zone

Later you will be able to combine this node with the pick_objects node coded earlier to simulate the full home service robot.

## Your Home Service Robot
Now it’s time to simulate a full home service robot capable of navigating to pick up and deliver virtual objects. To do so, the **add_markers* and **pick_objects** node should be communicating. Or, more precisely, the **add_markers** node should subscribe to your **odometry** to keep track of your robot pose.

Modify the add_markers node as follows:

-Initially show the marker at the pickup zone

-Hide the marker once your robot reaches the pickup zone

-Wait 5 seconds to simulate a pickup

-Show the marker at the drop off zone once your robot reaches it

**Note**

There are many ways to solve this problem. To establish communications between the robot and the markers, one method already mentioned is to let your add_markers node subscribe to your robot odometry and keep track of your robot pose.

Other solutions to this problem might be to use [ROS parameters](http://wiki.ros.org/ROS/Tutorials/UnderstandingServicesParams), subscribe to the AMCL pose, or even to publish a new variable that indicates whether or not your robot is at the pickup or drop off zone. Feel free to solve this problem in any way you wish.

![23](https://user-images.githubusercontent.com/25018831/179875137-03ad771a-c67e-4e77-80be-99f89623edff.gif)
