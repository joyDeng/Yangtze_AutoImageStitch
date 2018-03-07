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


void PanoImage::calculatePatches(float sigma) {
    // 0. clear patches
    // 1. use luminance value
    // 2. blur image a little bit
    // 3. calculate base on patch size
    // and correct for brightness/contrast

    m_patches.clear();


    FloatImage lumi, blurred, image;
    std::vector<FloatImage> lc = lumiChromi(m_image);
    lumi = lc[0];
    blurred = gaussianBlur_seperable(lumi, sigma);
    image = normalizeBySD(blurred);


    for (int p = 0; p < m_featurePoints.size(); ++p) {
        Vecxf patch(m_patchSize, m_patchSize);
        for (int i = 0; i < m_patchSize; ++i) {
            for (int j = 0; j < m_patchSize; ++j) {
                patch(i * m_patchSize + j) = image.smartAccessor(m_featurePoints[p].x() + i,
                                                                 m_featurePoints[p].y() + j, 0);
            }
        }
        m_patches.push_back(patch);
    }

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


