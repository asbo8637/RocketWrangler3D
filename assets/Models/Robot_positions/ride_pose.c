#include "../Robot.h"

// hips flared outward, knees bent
void robot_ridepose(Robot *robot)
{
    if (!robot)
        return;

    robot_setStance(robot,
                    -40.0f, 0.0f, 0.0f,    // lower torso 
                    -40.0f, 0.0f, 0.0f,    // upper torso
                    -5.0f, 0.0f, 0.0f,     // head looking
                    70.0f, 10.0f, 15.0f,   // right upper arm 
                    60.0f, 0.0f, -5.0f,    // right forearm
                    70.0f, -10.0f, -15.0f, // left upper arm
                    60.0f, 0.0f, 5.0f,     // left forearm
                    45.0f, 0.0f, 45.0f,    // right thigh
                    -40.0f, 0.0f, -10.0f,  // right calf
                    45.0f, 0.0f, -45.0f,   // left thigh
                    -40.0f, 0.0f, 10.0f,   // left calf 
                    800.0f);               // speed
}
