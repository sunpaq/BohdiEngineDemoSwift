#ifndef _MCCamera
#define _MCCamera

#include "monkc.h"
#include "MC3DBase.h"
#include "MC3DNode.h"

class(MCCamera, MC3DNode,
    double ratio;
    double focal_length;
    double view_angle;
    double max_distance;
    MCVector3 lookat;
    //local spherical coordinate
    //R[0,unlimited) tht[0, 180.0), fai[0, 360.0)
    double R_value;
    double R_percent;
    double tht;
    double fai;
    
    computing(double, Radius);
    computing(MCMatrix3, normal);
    computing(MCMatrix4, viewMatrix);
    computing(MCMatrix4, projectionMatrix);
    computing(MCVector3, currentPosition);
    
    MCBool isReverseMovement;
    MCBool isLockRotation;
);

method(MCCamera, MCCamera*, initWithWidthHeight, unsigned width, unsigned height);
method(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht);
method(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY);
method(MCCamera, void, pull, MCFloat deltaR);
method(MCCamera, void, reset, voida);
method(MCCamera, void, update, MCGLContext* ctx);//override
method(MCCamera, void, distanceScale, MCFloat scale);
method(MCCamera, void, printDebugInfo, voida);

#define MCLensStandard50mm (0.050)
#define MCLensWide24mm     (0.024)
#define MCLensLong100mm    (0.100)
#define MCLensLong200mm    (0.200)

#define MCRatioCameraFilm3x2    (3.0/2.0)
#define MCRatioOldTV4x3         (4.0/3.0)
#define MCRatioHDTV16x9         (16.0/9.0)
#define MCRatioCinema239x100    (2.39/1.0)
#define MCRatioMake(w, h)       ((double)w / (double)h)

#define MCLensStandard50mmViewAngle (45.0)
#endif
