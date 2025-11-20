#include "../Robot.h"
#include <stdlib.h>

// Helper function to generate random float in range [min, max]
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
}

// Random pose stance - generates random angles for dynamic look
void robot_randompose(Robot *robot, float crunchFactor, float rollAngle)
{
    if (!robot)
        return;

    robot_setStance(robot,
                    0.0f,  0.0f, 0.0f,        // lower torso (rotZ controlled by continuous spin)
                    randomFloat(-20, 20) - crunchFactor, randomFloat(-20, 20), randomFloat(-15, 15) - rollAngle * 0.6f,        // upper torso 
                    randomFloat(-60, 60) + crunchFactor * 0.5f, randomFloat(-80, 80), randomFloat(-40, 40) + rollAngle * 0.3f, // head
                    randomFloat(-90, 180) - crunchFactor * 0.4f, randomFloat(-60, 60) + crunchFactor, randomFloat(20, 180),       // right upper arm (Y forward)
                    randomFloat(0, 150), randomFloat(-40, 40), randomFloat(-60, 60),                        // right forearm 
                    randomFloat(-90, 180) - crunchFactor * 0.4f, randomFloat(-60, 60) - crunchFactor, randomFloat(-180, -20),     // left upper arm (Y forward)
                    randomFloat(0, 150), randomFloat(-40, 40), randomFloat(-60, 60),                        // left forearm 
                    randomFloat(-60, 90) + crunchFactor * 0.7f, randomFloat(-40, 40), randomFloat(-30, 30), // right thigh 
                    randomFloat(-110, -20) - crunchFactor * 0.5f, randomFloat(-20, 20), randomFloat(-20, 20), // right calf 
                    randomFloat(-60, 90) + crunchFactor * 0.7f, randomFloat(-40, 40), randomFloat(-30, 30), // left thigh 
                    randomFloat(-110, -20) - crunchFactor * 0.5f, randomFloat(-20, 20), randomFloat(-20, 20), // left calf
                    180.0f);                                                                                // speed
}
