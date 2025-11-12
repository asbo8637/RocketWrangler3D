#include "../Robot.h"

// T-Pose stance - arms extended horizontally
void robot_tpose(Robot *robot)
{
    if (!robot) return;
    
    robot_setStance(robot,
        0, 0, 0,          // lower torso (no rotation)
        0, 0, 0,          // upper torso (no rotation)
        0, 0, 0,          // head (looking forward)
        0, 0, 90,         // right upper arm (extended right)
        0, 0, 0,          // right forearm (straight)
        0, 0, -90,        // left upper arm (extended left)
        0, 0, 0,          // left forearm (straight)
        0, 0, 0,          // right thigh (straight)
        0, 0, 0,          // right calf (straight)
        0, 0, 0,          // left thigh (straight)
        0, 0, 0,          // left calf (straight)
        180.0f);          // speed
}
