#!/bin/sh

export TURTLEBOT_GAZEBO_WORLD_FILE=$(rospack find map)/world1.world

export TURTLEBOT_GAZEBO_MAP_FILE=$(rospack find map)/map.yaml

xterm -e "roslaunch turtlebot_gazebo turtlebot_world.launch" &
sleep 3
xterm -e "roslaunch turtlebot_gazebo amcl_demo.launch" &
sleep 3
xterm -e "roslaunch turtlebot_rviz_launchers view_navigation.launch"&
sleep 15
xterm -e "rosrun add_markers add_markers"&
sleep 2
xterm -e "rosrun pick_objects pick_objects"



