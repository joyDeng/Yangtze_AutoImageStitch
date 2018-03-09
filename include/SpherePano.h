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

struct cameraRay{
    //origin point position is Vec3f(0,0,0)
    Vec3f dir;
    float maxt;
    float mint;
};

typedef cameraRay Ray3f;

class SpherePano : public Pano{
    
    //initialize parameters
    SpherePano();
    
    // cat two images in spherecoordinates
    FloatImage cat2images(PanoImage re1, PanoImage im, Mat3f homo);
    
};


#endif /* SpherePano_h */
