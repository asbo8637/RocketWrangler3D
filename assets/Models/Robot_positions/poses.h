#ifndef ROBOT_POSES_H
#define ROBOT_POSES_H

#include "../Robot.h"

// T-Pose stance - arms extended horizontally
void robot_tpose(Robot *robot);

// Random pose stance - various angles for dynamic look
void robot_randompose(Robot *robot);

#endif // ROBOT_POSES_H
