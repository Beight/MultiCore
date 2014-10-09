#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NROFTRIANGLES 12
#define CUBESIZE 32
#define NROFLIGHTS 10

//Variables we change on for the tests
static const float WIDTH = 1024.0f;
static const float HEIGHT= 768.0f;

static int g_NrofBounces = 5;
static int g_NrofLights = 5;

#define NROFBOUNCES 5



#define THREADGROUPSX 32
#define THREADGROUPSY 24
#define	THREADGROUPSZ 1

#define NUMTHREADSX 32
#define NUMTHREADSY 32
#define NUMTHREADSZ 1

//Variables we can change
#define REFLECTIVITY 0.5f

#endif 