#include "../Robot.h"

// hips flared outward, knees bent
void robot_ridepose(Robot *robot)
{
    if (!robot)
        return;

    robot_setStance(robot,
                    -25.0f, 0.0f, 0.0f,    // lower torso 
                    -25.0f, 0.0f, 0.0f,    // upper torso
                    20.0f, 0.0f, 0.0f,     // head looking
                    40.0f, 10.0f, 15.0f,   // right upper arm 
                    60.0f, 0.0f, -5.0f,    // right forearm
                    40.0f, -10.0f, -15.0f, // left upper arm
                    60.0f, 0.0f, 0.0f,     // left forearm
                    55.0f, 0.0f, 55.0f,    // right thigh
                    -35.0f, 0.0f, -5.0f,  // right calf
                    55.0f, 0.0f, -55.0f,   // left thigh
                    -35.0f, 0.0f, 0.0f,   // left calf 
                    800.0f);               // speed
}
