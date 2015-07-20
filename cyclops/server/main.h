/**
 # Cyclops machine
 # Alejandro Escario Méndez
 # June 2015
 */

#ifndef __object_h__
#define __object_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __APPLE__
#  ifdef _WIN32
#    include <windows.h>
#  endif
#  include <GL/glut.h>
#else
#  include <GLUT/glut.h>
#endif
#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/arMulti.h>

#include <list>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <thread>

#define MARKER_WIDTH 293
#define MARKER_HEIGHT 272

#ifndef AF_INET
#define AF_INET 2
#endif

#ifndef INADDR_ANY
#define INADDR_ANY 0
#endif

#define BUFFER_SIZE 1024

#define PORT 3333

#define MOUSE_DRAWING 1
#define MOUSE_NOT_DRAWING 0

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct {
        double x;
        double y;
        double z;
    } Point3D;
    
    typedef struct {
        double x;
        double y;
    }Point2D;
    
    typedef struct {
        double m;
        double n;
    }Line_eq;
    
    static void             init(int argc, char *argv[]);
    static void             cleanup(void);
    static void             mainLoop(void);
    static void             draw( ARdouble trans1[3][4], ARdouble trans2[3][4], int mode );
    
#ifdef __cplusplus
}
#endif

#endif // __object_h__
