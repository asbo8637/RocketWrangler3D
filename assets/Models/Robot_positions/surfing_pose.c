#include "../Robot.h"

void robot_surfpose(Robot *robot)
{
    if (!robot)
        return;

    robot_setStance(robot,
        -10.0f, -90.0f, -10.0f,  // lower torso
        -10.0f, 20.0f, -8.0f,   // upper torso
        0.0f, 75.0f, 0.0f,       // head
        10.0f, 0.0f, 60.0f,    // right upper arm: trailing arm extends back
        30.0f, 0.0f, -25.0f,     // right forearm: relaxed counterweight
        -10.0f, -10.0f, -60.0f,    // left upper arm: leading arm forward for balance
        10.0f, 0.0f, -5.0f,    // left forearm: gentle bend
        45.0f, 0.0f, 0.0f,       // right thigh: now the rear leg inline with board
        -55.0f, 0.0f, 0.0f,      // right calf: weight compressed over back binding
        45.0f, 0.0f, -30.0f,       // left thigh: front leg set ahead along board axis
        -55.0f, 0.0f, 0.0f,      // left calf: strong crouch over front binding
        300.0f);               // speed: fast snap but not jarring
}
