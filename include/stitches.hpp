//
//  stiches.hpp
//  Yangtze
//
//  Created by DENG XI on 3/4/18.
//
//

#ifndef stiches_hpp
#define stiches_hpp

#include <stdio.h>
#include "floatimage.h"
#include "common.h"
#include "PanoImage.hpp"

// smallest box that bound the image
struct imagebound{
    Vec2f topleft;
    Vec2f btnright;
    
    imagebound();
    void grow(Vec3f point);
    
};

// canvas that contain two images
struct canvas{
    Vec2i offset;
    int length;
    int height;
};


typedef struct canvas Canvas;
typedef struct imagebound ImageBound;

class Pano{
public:
    
    // cat 2 image given corresponding poin sets
    FloatImage cat2images(const FloatImage &im1, const FloatImage &im2, std::vector<Vec2f> ref1, std::vector<Vec2f> ref2);
    
    // solve Ax = 0 with svd
    Mat3f solveHomo(MatrixXf);
    
    // calculate new canvas given image and homo
    ImageBound boundBox(const FloatImage &im);
    ImageBound boundBoxHomo(const FloatImage &im, Mat3f homo);
    
    // calculate the shift offset when conbining two images
    Canvas calculateCanvas(ImageBound a, ImageBound b);

    std::vector<std::vector<Vec2f>> matchDescriptors(PanoImage &pim1, PanoImage &pim2, float threshold=0.8);
    Mat3f RANSAC( PanoImage &pim1, PanoImage &pim2, float portion = 0.5, float accuracy = 0.01);
    
    Mat3f computeHomo(std::vector<std::vector<Vec2f>> pairs);
    Mat3f recomputeHomoByInliners(std::vector<std::vector<Vec2f>> pairs);

};



#endif /* stiches_hpp */


