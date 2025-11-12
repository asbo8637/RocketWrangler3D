#include "../Robot.h"
#include <stdlib.h>

// Helper function to generate random float in range [min, max]
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}

// Random pose stance - generates random angles for dynamic look
void robot_randompose(Robot *robot)
{
    if (!robot)
        return;

    robot_setStance(robot,
                    randomFloat(-40, 40), randomFloat(-40, 40), randomFloat(-30, 30),    // torso 
                    randomFloat(-60, 60), randomFloat(-80, 80), randomFloat(-40, 40),    // head 
                    randomFloat(-90, 180), randomFloat(-60, 60), randomFloat(20, 180),   // right upper arm 
                    randomFloat(0, 150), randomFloat(-40, 40), randomFloat(-60, 60),     // right forearm
                    randomFloat(-90, 180), randomFloat(-60, 60), randomFloat(-180, -20), // left upper arm 
                    randomFloat(0, 150), randomFloat(-40, 40), randomFloat(-60, 60),     // left forearm 
                    randomFloat(-60, 90), randomFloat(-40, 40), randomFloat(-30, 30),    // right thigh 
                    randomFloat(-140, 0), randomFloat(-20, 20), randomFloat(-20, 20),    // right calf 
                    randomFloat(-60, 90), randomFloat(-40, 40), randomFloat(-30, 30),    // left thigh
                    randomFloat(-140, 0), randomFloat(-20, 20), randomFloat(-20, 20),    // left calf
                    250.0f);                                                             // speed
}
