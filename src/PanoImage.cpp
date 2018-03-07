//
//  PanoImage.cpp
//  Yangtze
//
//  Created by DENG XI on 3/6/18.
//
//

#include "PanoImage.hpp"



PanoImage::PanoImage(const FloatImage &other, const int patchsize){
    FloatImage image(other);
    m_image = image;
}


void PanoImage::calculatePatches() {
    // 1. use luminance value
    // 2. blur image a little bit
    // 3. calculate base on patch size
    // and correct for brightness/contrast


}
void PanoImage::horrisCornerDetector(int k, Vec2i step){
    // initialize sliding window
    MatrixXf window;
    window.resize(k,k);
    window.setOnes();
    
    // Compute image gradients Ix and Iy for all pixels
    
    // For each pixel compute Matrix M, compute R
    // Find points with large corner response function R
    // Take the points of locally maximum R as the detected feature points
}


