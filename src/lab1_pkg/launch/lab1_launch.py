from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Start with Talker Node
        Node(
            package='lab1_pkg',
            executable='talker',
            name= 'talker',
            parameters=[
                {'v': 5.0}, #set speed to 5.0
                {'d': 2.0}  #set streering to 2.0
            ]
        ),

        Node(
            package='lab1_pkg',
            executable='relay',
            name='relay',
            output='screen'
        )
    ])