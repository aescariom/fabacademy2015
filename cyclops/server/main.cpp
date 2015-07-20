/**
 # Cyclops machine
 # Alejandro Escario Méndez
 # June 2015
 */

#include "main.h"

using namespace std;

#define                 CPARA_NAME       "Data/camera_para.dat"
#define                 CONFIG_NAME      "Data/multi/marker.dat"
#define                 MARKER_NAME      "Data/multi/markerf.dat"

#define MARKER_SIZE 40
#define MARKER_SIZE_HALF MARKER_SIZE/2

ARHandle               *arHandle;
AR3DHandle             *ar3DHandle;
ARGViewportHandle      *vp;
ARMultiMarkerInfoT     *config, *fMarker;
int                     robustFlag = 0;
int                     countI;
ARParamLT              *gCparamLT = NULL;
Point2D                 point2D, fixedPoints[4], pF, targetPoint;

Point2D movement;
Point2D zero;
double z = 0;
int moveTo = 0;
int zeroCalculated = 0;

int mouseState = MOUSE_NOT_DRAWING;

std::list<Point2D> pointList;

Point3D lastSent;


static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b || key == 'q' ) {
        ARLOG("*** %f (frame/sec)\n", (double)countI/arUtilTimer());
        cleanup();
        exit(0);
    }
    
    if( key == ' ' ) {
        if(z == 0){
            z = 15;
        }else{
            z = 0;
        }
        printf("z=%f\n", z);
    }
}

int socketInit(){
    lastSent.x = 0;
    lastSent.y = 0;
    lastSent.z = 0;
    
    
    int echo_socket = 0;
    int echo_socket_child = 0; // for TCP
    struct sockaddr_in server;
    struct sockaddr_in client;
    struct hostent *hostp; // client host info
    char *hostaddrp; // dotted decimal host addr string
    char buffer[BUFFER_SIZE];
    unsigned int clientlen = 0;
    unsigned int serverlen = 0;
    int optval = 1;
    ssize_t msg_byte_size = 0;
    
    
    if ((echo_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("EchoServer: Failed opening socket");
        return -1;
    }
    
    // setsockopt: Handy debugging trick that lets  us rerun the server immediately after we kill it; otherwise we have to wait about 20 secs.
    // Eliminates "ERROR on binding: Address already in use" error.
    setsockopt(echo_socket, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int));
    
    // Construct the server sockaddr_in structure
    memset(&server, 0, sizeof(server));             /* Clear struct */
    server.sin_family = AF_INET;                    /* Internet/IP */
    server.sin_addr.s_addr = htonl(INADDR_ANY);     /* Any IP address */
    server.sin_port = htons(3333);         /* server port */
    
    // Bind the socket
    serverlen = sizeof(server);
    if (::bind(echo_socket, (struct sockaddr *) &server, serverlen) < 0) {
        printf("EchoServer: Failed binding socket");
        return -1;
    }
    
    // Wait for a connection until cancelled
    if (listen(echo_socket, PORT) == -1)
    {
        perror("listen");
        exit(1);
    }
    
    while (1)
    {
        clientlen = sizeof(client);
        
        echo_socket_child = accept(echo_socket, (struct sockaddr *) &client, &clientlen);
        
        if (echo_socket_child < 0)
        {
            perror("accept");
            break;
        }
        
        // gethostbyaddr: determine who sent the message
        hostp = gethostbyaddr((const char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), AF_INET);
        
        if (hostp == NULL)
        {   herror("byaddr");
            break;
        }
        
        hostaddrp = inet_ntoa(client.sin_addr);
        
        if (hostaddrp == NULL)
        {
            printf("ERROR on inet_ntoa\n");
            break;
        }
        
        printf("server established connection with %s (%s)\n", hostp->h_name, hostaddrp);
        
        bzero(buffer, BUFFER_SIZE);
        
        while(1){
            msg_byte_size = read(echo_socket_child, buffer, BUFFER_SIZE);
            
            if (buffer[0] != '1')
            {
                printf("ERROR reading from socket");
                break;
            }
            
            Point3D current;
            if(lastSent.z != z){
                current.x = lastSent.x;
                current.y = lastSent.y;
                lastSent.z = current.z = z;
                sprintf(buffer, "%f;%f;%f", current.x, current.y, current.z);
                msg_byte_size = write(echo_socket_child, buffer, strlen(buffer));
                if (msg_byte_size < 0)
                {
                    printf("ERROR writing to socket");
                    break;
                }
            }else if(moveTo == 1){
                movement.x -= 18.5;
                movement.y += 1;
                current.x = movement.x;
                current.y = movement.y;
                current.z = z;
                
                    sprintf(buffer, "%f;%f;%f", current.x, current.y, current.z);
                    msg_byte_size = write(echo_socket_child, buffer, strlen(buffer));
                    lastSent.x = movement.x;
                    lastSent.y = movement.y;
                    lastSent.z = current.z;
                    
                    if (msg_byte_size < 0)
                    {
                        printf("ERROR writing to socket");
                        break;
                    }
                
                moveTo = 0;
            }else{
                sprintf(buffer, "-1");
                msg_byte_size = write(echo_socket_child, buffer, strlen(buffer));
            }
        }
        close(echo_socket_child);
        
        
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    init(argc, argv);
    
    std::thread first (socketInit);
    argSetDispFunc( mainLoop, 1 );
    argSetKeyFunc( keyEvent );
    countI = 0;
    arVideoCapStart();
    arUtilTimerReset();
    argMainLoop();
    
    return (0);
}

Point3D _2Dto3D(int x, int y)
{
    Point3D point;
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
    
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
    
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
    
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    point.x = posX;
    point.y = posY;
    point.z = posZ;
    return point;
}

Point2D _3Dto2D(Point3D origin)
{
    Point2D point;
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble posX, posY, posZ;
    
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
    
    gluProject(origin.x, origin.y, origin.z,
               modelview, projection, viewport,
               &posX, &posY, &posZ);
    point.x = posX;
    point.y = glutGet(GLUT_WINDOW_HEIGHT) - posY;
    return point;
}

void drawDots(){
    std::list<Point2D>::const_iterator iterator;
    for (iterator = pointList.begin(); iterator != pointList.end(); ++iterator) {
        Point2D point = *iterator;
        Point3D _3D = _2Dto3D(point.x, point.y);
        glBegin(GL_POINTS); // render with points
        glVertex3f(_3D.x, _3D.y, _3D.z); //display a point
        glEnd();
    }
}

static int fInsideMachine(){
    if(pF.x < fixedPoints[0].x || pF.y < fixedPoints[0].y){
        return 0;
    }
    if(pF.x > fixedPoints[1].x || pF.y < fixedPoints[1].y){
        return 0;
    }
    if(pF.x < fixedPoints[2].x || pF.y > fixedPoints[2].y){
        return 0;
    }
    if(pF.x > fixedPoints[3].x || pF.y > fixedPoints[3].y){
        return 0;
    }
    return 1;
}

Point2D getDistance(Line_eq ab, Line_eq ad, Line_eq bc, Line_eq dc, Point2D current){
    
    double ys = -(ab.m * current.x + ab.n);
    double yi = -(dc.m * current.x + dc.n);
    double height = yi - ys;
    // medimos el eje x a esa altura
    double xl = (- current.y - ad.n) / ad.m;
    double xr = (- current.y - bc.n) / bc.m;
    double width = xr - xl;
    //printf("width: %f\n", width);
    
    
    Point3D draw1 = _2Dto3D(current.x, yi);
    Point3D draw2 = _2Dto3D(current.x, ys);
    glColor3f(0, 1, 0.0);
    glLineWidth(1);
    glBegin(GL_LINES); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glVertex3f(draw2.x, draw2.y, draw2.z); //display a point
    glEnd();
    
    draw1 = _2Dto3D(xl, current.y);
    draw2 = _2Dto3D(xr, current.y);
    glBegin(GL_LINES); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glVertex3f(draw2.x, draw2.y, draw2.z); //display a point
    glEnd();
    
    
    double inbox_x = current.x - xl;
    double inbox_y = current.y - ys;
    
    Point2D relativePositionToA;
    relativePositionToA.x = MARKER_WIDTH*inbox_x/width;
    relativePositionToA.y = MARKER_HEIGHT*inbox_y/height;
    
    //printf("movement: %f, %f\n", relativePositionToA.x, relativePositionToA.y);
    return relativePositionToA;
}

void getRelativeMovements(){
    
    
    Line_eq ab, ad, bc, dc;
    
    ab.m = (fixedPoints[1].y - fixedPoints[0].y) / (fixedPoints[0].x - fixedPoints[1].x);
    ab.n = -fixedPoints[0].y - ab.m*fixedPoints[0].x;
    
    dc.m = (fixedPoints[3].y - fixedPoints[2].y) / (fixedPoints[2].x - fixedPoints[3].x);
    dc.n = -fixedPoints[2].y - dc.m*fixedPoints[2].x;
    
    ad.m = (fixedPoints[2].y - fixedPoints[0].y) / (fixedPoints[0].x - fixedPoints[2].x);
    ad.n = -fixedPoints[0].y - ad.m*fixedPoints[0].x;
    
    bc.m = (fixedPoints[3].y - fixedPoints[1].y) / (fixedPoints[1].x - fixedPoints[3].x);
    bc.n = -fixedPoints[1].y - bc.m*fixedPoints[1].x;
    
    glColor3b(0, 1, 0);
    Point3D draw1 = _2Dto3D(fixedPoints[0].x, fixedPoints[0].y);
    Point3D draw2 = _2Dto3D(fixedPoints[1].x, fixedPoints[1].y);
    glBegin(GL_LINES); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glVertex3f(draw2.x, draw2.y, draw2.z); //display a point
    glEnd();
    
    draw1 = _2Dto3D(fixedPoints[1].x, fixedPoints[1].y);
    draw2 = _2Dto3D(fixedPoints[3].x, fixedPoints[3].y);
    glBegin(GL_LINES); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glVertex3f(draw2.x, draw2.y, draw2.z); //display a point
    glEnd();
    
    draw1 = _2Dto3D(fixedPoints[2].x, fixedPoints[2].y);
    draw2 = _2Dto3D(fixedPoints[3].x, fixedPoints[3].y);
    glBegin(GL_LINES); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glVertex3f(draw2.x, draw2.y, draw2.z); //display a point
    glEnd();
    
    draw1 = _2Dto3D(fixedPoints[2].x, fixedPoints[2].y);
    draw2 = _2Dto3D(fixedPoints[0].x, fixedPoints[0].y);
    glBegin(GL_LINES); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glVertex3f(draw2.x, draw2.y, draw2.z); //display a point
    glEnd();
    
    
    draw1 = _2Dto3D(targetPoint.x, targetPoint.y);
    glColor3b(1, 0, 0);
    glPointSize(2.5);
    glBegin(GL_POINT); // render with points
    glVertex3f(draw1.x, draw1.y, draw1.z); //display a point
    glEnd();
    
    /*printf("AB_h => y = %.2f x + %.2f\n", ab.m, ab.n);
    printf("DC_h => y = %.2f x + %.2f\n", dc.m, dc.n);
    printf("AD_v => y = %.2f x + %.2f\n", ad.m, ad.n);
    printf("BC_v => y = %.2f x + %.2f\n", bc.m, bc.n);*/
    
    if(zeroCalculated == 0){
        zero = getDistance(ab, ad, bc, dc, pF);
        zeroCalculated = 1;
    }
    
    if(pointList.size() > 0 && moveTo == 0) {
        targetPoint = *pointList.begin();
        
        Point2D destiny = getDistance(ab, ad, bc, dc, targetPoint);
        
        movement.y = -(destiny.x - zero.x);
        movement.x = (destiny.y - zero.y);
        printf("Movement: %f, %f\n", movement.x, movement.y);
        moveTo = 1;
        pointList.pop_front();
    }else{
        getDistance(ab, ad, bc, dc, targetPoint);
    }
}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             imageProcMode;
    int             debugMode;
    double          err;
    int             i;
    
    /* grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    
    if( countI == 100 ) {
        ARLOG("*** %f (frame/sec)\n", (double)countI/arUtilTimer());
        arUtilTimerReset();
        countI = 0;
    }
    countI++;
    
    /* detect the markers in the video frame */
    if( arDetectMarker(arHandle, dataPtr) < 0 ) {
        cleanup();
        exit(0);
    }
    marker_num = arGetMarkerNum( arHandle );
    marker_info =  arGetMarker( arHandle );
    
    argDrawMode2D(vp);
    arGetDebugMode( arHandle, &debugMode );
    if( debugMode == 0 ) {
        argDrawImage( dataPtr );
    }
    else {
        arGetImageProcMode(arHandle, &imageProcMode);
        if( imageProcMode == AR_IMAGE_PROC_FRAME_IMAGE ) {
            argDrawImage( arHandle->labelInfo.bwImage );
        }
        else {
            argDrawImageHalf( arHandle->labelInfo.bwImage );
        }
        glColor3f( 1.0f, 0.0f, 0.0f );
        glLineWidth( 2.0f);
        for( i = 0; i < marker_num; i++ ) {
            argDrawSquareByIdealPos( marker_info[i].vertex );
        }
        glLineWidth( 1.0f );
    }
    
    argDrawMode3D(vp);
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    
    if( robustFlag ) {
        err = arGetTransMatMultiSquareRobust( ar3DHandle, marker_info, marker_num, config);
    }
    else {
        err = arGetTransMatMultiSquare( ar3DHandle, marker_info, marker_num, config);
    }
    
    for( i = 0; i < config->marker_num; i++ ) {
        if( config->marker[i].visible >= 0 ) draw( config->trans, config->marker[i].trans, 0 );
        else                                 draw( config->trans, config->marker[i].trans, 1 );
        Point3D point;
        point.x = 0;
        point.y = 0;
        point.z = 0;
        fixedPoints[i] = _3Dto2D(point);
    }
    
    if( robustFlag ) {
        err = arGetTransMatMultiSquareRobust( ar3DHandle, marker_info, marker_num, fMarker);
    }
    else {
        err = arGetTransMatMultiSquare( ar3DHandle, marker_info, marker_num, fMarker);
    }
    
    for( i = 0; i < fMarker->marker_num; i++ ) {
        if( fMarker->marker[i].visible >= 0 ){
            
            draw(fMarker->trans, fMarker->marker[i].trans, 0);
            
            Point3D point;
            point.x = 0;
            point.y = 0;
            point.z = 0;
            pF = _3Dto2D(point);
            if(fInsideMachine()){
                draw(fMarker->trans, fMarker->marker[i].trans, 2);
            }else{
                draw(fMarker->trans, fMarker->marker[i].trans, 3);
            }
        }
    }
    getRelativeMovements();
    drawDots();
    argSwapBuffers();
}

void onMouseClick(int button, int state, int x, int y) {
    
    if (button==GLUT_LEFT_BUTTON && state==GLUT_UP) {
        // adding the position
        Point2D point;
        point.x = x;
        point.y = y;
        if(pointList.size() > 0){
            pointList.pop_front();
        }
        pointList.push_back(point);
    }
    
    if (button==GLUT_RIGHT_BUTTON && state==GLUT_UP && mouseState == MOUSE_NOT_DRAWING) {
        mouseState = MOUSE_DRAWING;
        // adding the position
        Point2D point;
        point.x = x;
        point.y = y;
        if(pointList.size() > 0){
            pointList.pop_front();
        }
        pointList.push_back(point);
        
    }else if(button==GLUT_RIGHT_BUTTON && state==GLUT_UP && mouseState == MOUSE_DRAWING){
        mouseState = MOUSE_NOT_DRAWING;
    }
    glutPostRedisplay();
}

void onMouseDrag(int x, int y)
{
    if(mouseState == MOUSE_DRAWING){
        Point2D point;
        point.x = x;
        point.y = y;
        pointList.push_back(point);
    }
    glutPostRedisplay();
}

static void   init(int argc, char *argv[]){
    ARParam         cparam;
    ARGViewport     viewport;
    ARPattHandle   *arPattHandle;
    char            vconf[512];
    char            configName[512];
    int             xsize, ysize;
    AR_PIXEL_FORMAT pixFormat;
    int             i;
    
    configName[0] = '\0';
    vconf[0] = '\0';
    strcat(vconf, "-width=1152 -height=720");
    for( i = 1; i < argc; i++ ) {
        if( strncmp(argv[i], "-config=", 8) == 0 ) {
            strcpy(configName, &argv[i][8]);
        }
        /*else {
            if( vconf[0] != '\0' ) strcat(vconf, " ");
            strcat(vconf, argv[i]);
        }*/
    }
    if( configName[0] == '\0' ) strcpy(configName, CONFIG_NAME);
    
    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoGetSize(&xsize, &ysize) < 0 ) exit(0);
    ARLOGi("Image size (x,y) = (%d,%d)\n", xsize, ysize);
    if( (pixFormat=arVideoGetPixelFormat()) < 0 ) exit(0);
    
    /* set the initial camera parameters */
    if( arParamLoad(CPARA_NAME, 1, &cparam) < 0 ) {
        ARLOGe("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &cparam, xsize, ysize, &cparam );
    ARLOG("*** Camera Parameter ***\n");
    arParamDisp( &cparam );
    if ((gCparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL) {
        ARLOGe("Error: arParamLTCreate.\n");
        exit(-1);
    }
    
    if( (arHandle=arCreateHandle(gCparamLT)) == NULL ) {
        ARLOGe("Error: arCreateHandle.\n");
        exit(0);
    }
    if( arSetPixelFormat(arHandle, pixFormat) < 0 ) {
        ARLOGe("Error: arSetPixelFormat.\n");
        exit(0);
    }
    
    if( (ar3DHandle=ar3DCreateHandle(&cparam)) == NULL ) {
        ARLOGe("Error: ar3DCreateHandle.\n");
        exit(0);
    }
    
    if( (arPattHandle=arPattCreateHandle()) == NULL ) {
        ARLOGe("Error: arPattCreateHandle.\n");
        exit(0);
    }
    arPattAttach( arHandle, arPattHandle );
    
    if( (config = arMultiReadConfigFile(configName, arPattHandle)) == NULL ) {
        ARLOGe("config data load error !!\n");
        exit(0);
    }
    
    if( (fMarker = arMultiReadConfigFile(MARKER_NAME, arPattHandle)) == NULL ) {
        ARLOGe("config data load error !!\n");
        exit(0);
    }
    
    
    if( config->patt_type == AR_MULTI_PATTERN_DETECTION_MODE_TEMPLATE ) {
        arSetPatternDetectionMode( arHandle, AR_TEMPLATE_MATCHING_COLOR );
    } else if( config->patt_type == AR_MULTI_PATTERN_DETECTION_MODE_MATRIX ) {
        arSetPatternDetectionMode( arHandle, AR_MATRIX_CODE_DETECTION );
    } else { // AR_MULTI_PATTERN_DETECTION_MODE_TEMPLATE_AND_MATRIX
        arSetPatternDetectionMode( arHandle, AR_TEMPLATE_MATCHING_COLOR_AND_MATRIX );
    }
    
    /* open the graphics window */
    viewport.sx = 0;
    viewport.sy = 0;
    viewport.xsize = xsize;
    viewport.ysize = ysize;
    if( (vp=argCreateViewport(&viewport)) == NULL ) exit(0);
    argViewportSetCparam( vp, &cparam );
    argViewportSetPixFormat( vp, pixFormat );
    
    
    glutMouseFunc(onMouseClick);
    glutPassiveMotionFunc(onMouseDrag);
}

/* cleanup function called when program exits */
static void cleanup(void){
    arParamLTFree(&gCparamLT);
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

static void draw( ARdouble trans1[3][4], ARdouble trans2[3][4], int mode ){
    ARdouble  gl_para[16];
    GLfloat   light_position[]  = {100.0f, -200.0f, 200.0f, 0.0f};
    GLfloat   light_ambi[]      = {0.1f, 0.1f, 0.1f, 0.0f};
    GLfloat   light_color[]     = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat   mat_flash[]       = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat   mat_flash_shiny[] = {50.0f};
    GLfloat   mat_diffuse[]     = {0.0f, 1.0f, 0.0f, 1.0f};
    GLfloat   mat_diffuse1[]    = {1.0f, 0.0f, 0.0f, 1.0f};
    int       debugMode;
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* load the camera transformation matrix */
    glMatrixMode(GL_MODELVIEW);
    argConvGlpara(trans1, gl_para);
#ifdef ARDOUBLE_IS_FLOAT
    glLoadMatrixf( gl_para );
#else
    glLoadMatrixd( gl_para );
#endif
    argConvGlpara(trans2, gl_para);
#ifdef ARDOUBLE_IS_FLOAT
    glMultMatrixf( gl_para );
#else
    glMultMatrixd( gl_para );
#endif
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
    if( mode%2 == 0 ) {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_diffuse);
    }
    else {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_diffuse1);
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef( 0.0f, 0.0f, 20.0f );
    arGetDebugMode( arHandle, &debugMode );
    
    if(mode >= 2){
        if( debugMode == 0 ) glutSolidCube(40.0);
        else                glutWireCube(40.0);
    }else{
        glutWireCube(40.0);
    }
    glPopMatrix();
    
    glDisable( GL_LIGHT0 );
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
}