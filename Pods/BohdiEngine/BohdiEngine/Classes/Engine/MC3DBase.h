#ifndef _MC3DBase
#define _MC3DBase

#include <math.h>
#include "MCMath.h"
#include "MCGLBase.h"

#define MCBUFFER_OFFSET(i)         ((char *)NULL + (i))
#define MCARRAY_COUNT(array, type) (sizeof(array)/sizeof(type))

typedef enum {
    MC3DSuccess = 0,
    MC3DErrChildrenFull,
    MC3DErrChildNotFound,
    MC3DErrUniformNotFound = -1,
} MC3DErrCode;

//world is right hand y on top, local is left hand z on top
MCInline MCVector3 MCWorldCoorFromLocal(MCVector3 localvertex, MCVector3 modelposition) {
    return (MCVector3){modelposition.x+localvertex.y,
                      modelposition.y+localvertex.z,
                      modelposition.z+localvertex.x};
}

MCInline MCVector3 MCLocalCoorFromWorld(MCVector3 worldvertex, MCVector3 modelposition) {
    return (MCVector3){worldvertex.z-modelposition.z,
                      worldvertex.x-modelposition.x,
                      worldvertex.y-modelposition.y};
}

//R[0,unlimited) tht[0, M_PI), fai[0, 2M_PI)
MCInline MCVector3 MCVertexFromSpherical(double R, double tht, double fai) {
#if (defined(__APPLE__) || defined(__ANDROID__))
    double sinT = MCSinDegrees(tht);
    double sinF = MCSinDegrees(fai);
    double cosT = MCCosDegrees(tht);
    double cosF = MCCosDegrees(fai);
    double x = R * sinT * cosF;
    double y = R * sinT * sinF;
    double z = R * cosT;
#else
    double x = R * sin(tht) * cos(fai);
    double y = R * sin(tht) * sin(fai);
    double z = R * cos(tht);
#endif
    return (MCVector3){x,y,z};
}

MCInline MCMatrix4 MCMatrix4MakePerspective(float fovyRadians, float aspect, float nearZ, float farZ)
{
    float cotan = 1.0f / tanf(fovyRadians / 2.0f);
    
    MCMatrix4 m = { cotan / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, cotan, 0.0f, 0.0f,
        0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
        0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f };
    
    return m;
}

MCInline MCMatrix4 MCMatrix4MakeTranslation(float tx, float ty, float tz)
{
    MCMatrix4 m = MCMatrix4Identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

MCInline MCMatrix4 MCMatrix4MakeScale(float sx, float sy, float sz)
{
    MCMatrix4 m = MCMatrix4Identity();
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}

//NEW
static inline MCMatrix3 MCMatrix4GetMatrix3(MCMatrix4 matrix)
{
    MCMatrix3 m = { matrix.m[0], matrix.m[1], matrix.m[2],
        matrix.m[4], matrix.m[5], matrix.m[6],
        matrix.m[8], matrix.m[9], matrix.m[10] };
    return m;
}

static inline MCMatrix3 MCMatrix3Scale(MCMatrix3 matrix, float sx, float sy, float sz)
{
    MCMatrix3 m = { matrix.m[0] * sx, matrix.m[1] * sx, matrix.m[2] * sx,
        matrix.m[3] * sy, matrix.m[4] * sy, matrix.m[5] * sy,
        matrix.m[6] * sz, matrix.m[7] * sz, matrix.m[8] * sz };
    return m;
}

static inline MCMatrix3 MCMatrix3Transpose(MCMatrix3 matrix)
{
    MCMatrix3 m = { matrix.m[0], matrix.m[3], matrix.m[6],
        matrix.m[1], matrix.m[4], matrix.m[7],
        matrix.m[2], matrix.m[5], matrix.m[8] };
    return m;
}

static inline MCMatrix3 MCMatrix3Invert(MCMatrix3 matrix, int* isInvertible) {
    float determinant = (matrix.m00 * (matrix.m11 * matrix.m22 - matrix.m12 * matrix.m21)) + (matrix.m01 * (matrix.m12 * matrix.m20 - matrix.m22 * matrix.m10)) + (matrix.m02 * (matrix.m10 * matrix.m21 - matrix.m11 *matrix.m20));
    
    if (isInvertible) {
        *isInvertible = (determinant != 0.0f)? 1 : 0;
    }
    
    if (!(determinant != 0.0f)) {
        return MCMatrix3Identity;
    }
    
    return MCMatrix3Scale(MCMatrix3Transpose(matrix), determinant, determinant, determinant);
}

static inline MCMatrix3 MCMatrix3InvertAndTranspose(MCMatrix3 matrix, void* isInvertible) {
    return MCMatrix3Transpose(MCMatrix3Invert(matrix, isInvertible));
}

static inline MCMatrix4 MCMatrix4MakeLookAt(float eyeX, float eyeY, float eyeZ,
                                           float centerX, float centerY, float centerZ,
                                           float upX, float upY, float upZ)
{
    MCVector3 ev = { eyeX, eyeY, eyeZ };
    MCVector3 cv = { centerX, centerY, centerZ };
    MCVector3 uv = { upX, upY, upZ };
    MCVector3 n = MCVector3Normalize(MCVector3Add(ev, MCVector3Reverse(cv)));
    MCVector3 u = MCVector3Normalize(MCVector3Cross(uv, n));
    MCVector3 v = MCVector3Cross(n, u);
    
    MCMatrix4 m = { u.v[0], v.v[0], n.v[0], 0.0f,
        u.v[1], v.v[1], n.v[1], 0.0f,
        u.v[2], v.v[2], n.v[2], 0.0f,
        MCVector3Dot(MCVector3Reverse(u), ev),
        MCVector3Dot(MCVector3Reverse(v), ev),
        MCVector3Dot(MCVector3Reverse(n), ev),
        1.0f };
    
    return m;
}

#endif
