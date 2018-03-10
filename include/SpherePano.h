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
    Vec3f t;
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
    float m_res;
    
public:
    
    //initialize parameters
    SpherePano();
    SpherePano(float f, int m_res);
    
    // cat two images in spherecoordinates
    virtual FloatImage cat2images(const FloatImage &re1, const FloatImage &im, Mat3f homo);
    virtual FloatImage catnimages(FloatImage ref, vector<FloatImage> ims, vector<Mat3f> homos);
    virtual FloatImage cat2imageBlend(const FloatImage &im1, const FloatImage &im2, Mat3f homo);
    virtual FloatImage catnimagesBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos);
    virtual FloatImage catnimagesTwoScaleBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos, float sigma = 3.f, bool lin=true);
    
;
    
//    FloatImage autocat2imagesInSphere(PanoImage ref, PanoImage im);
//    FloatImage autocatnimagesSphere(std::vector<PanoImage> &pims, bool center, bool blend, bool twoscale = false);
    
    Vec3f computeY(vector<Mat3f> homos);
    Mat3f computeRotationMatrix(Vec3f y, Vec3f u);
    
};

#endif /* SpherePano_h */
