#include "../Robot.h"

// hips flared outward, knees bent
void robot_ridepose(Robot *robot)
{
    if (!robot)
        return;

    robot_setStance(robot,
                    -40.0f, 0.0f, 0.0f,    /* lower torso leans forward slightly */
                    -40.0f, 0.0f, 0.0f,      /* upper torso upright */
                    -5.0f, 0.0f, 0.0f,     /* head looking slightly forward/down */
                    40.0f, 10.0f, 15.0f,   /* right upper arm forward and slightly in */
                    60.0f, 0.0f, -5.0f,    /* right forearm bent to hold reins */
                    40.0f, -10.0f, -15.0f, /* left upper arm mirrored */
                    60.0f, 0.0f, 5.0f,     /* left forearm bent */
                    45.0f, 0.0f, 35.0f,    /* right thigh lifted and rotated outward */
                    -40.0f, 0.0f, -10.0f,  /* right calf dropped, heel slightly inward */
                    45.0f, 0.0f, -35.0f,   /* left thigh lifted and rotated outward */
                    -40.0f, 0.0f, 10.0f,   /* left calf dropped */
                    220.0f);               /* reasonably quick blend speed */
}
