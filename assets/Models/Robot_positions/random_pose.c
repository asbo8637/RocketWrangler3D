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
                    randomFloat(-10, 0) - crunchFactor, randomFloat(-5, 5), randomFloat(-5, 5) - rollAngle * 0.1f,        // upper torso 
                    randomFloat(-20, 20) + crunchFactor * 0.5f, randomFloat(-20, 20), randomFloat(-20, 20), // head
                    randomFloat(-90, 180) - crunchFactor * 0.4f, randomFloat(-60, 60) + crunchFactor, randomFloat(20, 180),       // right upper arm (Y forward)
                    randomFloat(30, 90) , randomFloat(-20, 20), randomFloat(-20, 20),                        // right forearm 
                    randomFloat(-90, 180) - crunchFactor * 0.4f, randomFloat(-60, 60) - crunchFactor, randomFloat(-180, -20),     // left upper arm (Y forward)
                    randomFloat(30, 90) , randomFloat(-20, 20), randomFloat(-20, 20),                        // left forearm 
                    randomFloat(-60, 90) + crunchFactor * 0.4f, randomFloat(-10, 10), randomFloat(35, 20)-crunchFactor*0.2f, // right thigh 
                    randomFloat(-110, -20) - crunchFactor * 0.5f, randomFloat(-20, 20), randomFloat(-20, 20), // right calf 
                    randomFloat(-60, 90) + crunchFactor * 0.4f, randomFloat(-10, 10), randomFloat(-35, -20)+crunchFactor*0.2f, // left thigh 
                    randomFloat(-110, -20) - crunchFactor * 0.5f, randomFloat(-20, 20), randomFloat(-20, 20), // left calf
                    280.0f);                                                                                // speed
}
