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
#include "PanoImage.hpp"

class SpherePano : public Pano{
    
    SpherePano();
    
    FloatImage cat2images(PanoImage re1, PanoImage im, Mat3f homo);
};


#endif /* SpherePano_h */
