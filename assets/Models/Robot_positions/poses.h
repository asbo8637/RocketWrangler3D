#ifndef ROBOT_POSES_H
#define ROBOT_POSES_H

#include "../Robot.h"

// T-Pose stance - arms extended horizontally
void robot_tpose(Robot *robot);

// Random pose stance - various angles for dynamic look
// crunchFactor: positive values bend forward (tucked position)
// rollAngle: roll bias applied across joints
void robot_randompose(Robot *robot, float crunchFactor, float rollAngle);

// Riding stance
void robot_ridepose(Robot *robot);

// Surfing stance
void robot_surfpose(Robot *robot);

#endif // ROBOT_POSES_H
