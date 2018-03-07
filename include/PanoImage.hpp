//
//  PanoImage.hpp
//  Yangtze
//
//  Created by DENG XI on 3/6/18.
//
//

#ifndef PanoImage_hpp
#define PanoImage_hpp

#include <stdio.h>
#include "common.h"
#include "floatimage.h"

class PanoImage{
public:
    PanoImage(const FloatImage &other, const int patchsize = 3);
    
    FloatImage getImage(){return FloatImage(m_image);}
    int getPatchSize(){return m_patchSize;}
    int getPointCount(){return m_pointCount;}
    Vecxf getPatches(int i){return m_patches[i];}
    Vec2i getFeaturePoint(int i){return m_featurePoints[i];}
    
    // corner detector
    void horrisCornerDetector(int window, Vec2i step);
    
    // feature descriptor
    void calculatePatches();
    
    
private:
    std::vector<Vecxf> m_patches;
    std::vector<Vec2i> m_featurePoints;
    int m_patchSize;
    int m_pointCount;
    FloatImage m_image;
};

#endif /* PanoImage_hpp */
