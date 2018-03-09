//
//  SpherePano.h
//  Yangtze
//
//  Created by DENG XI on 3/8/18.
//
//

#ifndef SpherePano_h
#define SpherePano_h
#include "common.h"
#include "floatimage.h"
#include "Pano.hpp"
#include "PanoImage.hpp"

// trace ray from camera
struct cameraRay{
    //origin point position is Vec3f(0,0,0)
    Vec3f o;
    Vec3f dir;
    float mint;
    float maxt;
};

//record the intersection with plane
struct itersection{
    Vec3f p;
    Vec2f uv;
};

typedef cameraRay Ray;
typedef itersection Intersect;

// smallest 3D box that bound the image plane

struct plane{
    Vec3f _base;
    Vec3f _edge0;
    Vec3f _edge1;
    Vec3f normal;

    plane(Vec3f pos0, Vec3f pos1, Vec3f pos2);
    bool intersect(Ray &ray, Intersect &it);
};

struct planebound{
    Vec3f celRightBack;
    Vec3f btnLeftFront;
    
    planebound();
    void growBound(Vec3f point);
    bool inbound(Vec3f point);
};

typedef planebound PlaneBound;
typedef plane Plane;

class SpherePano : public Pano{
    float m_f;
    Mat3f m_S;
    Mat3f m_K;
    
public:
    
    //initialize parameters
    SpherePano();
    SpherePano(float f);
    
    // cat two images in spherecoordinates
    FloatImage cat2imagesInSphere(FloatImage re1, FloatImage im, Mat3f homo);
    FloatImage autocat2imagesInSphere(PanoImage ref, PanoImage im);
    
};

#endif /* SpherePano_h */
