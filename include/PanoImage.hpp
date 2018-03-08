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
#include "filtering.h"

class PanoImage{
public:
    PanoImage(const FloatImage &other, const int patchsize = 9);
    
    const FloatImage getImage(){return FloatImage(m_image);}
    int getPatchSize(){return m_patchSize;}
    int getPointCount(){return m_pointCount;}
    Vecxf getPatches(int i){return m_patches[i];}
    Vec2i getFeaturePoint(int i){return m_featurePoints[i];}
    
    // corner detector
    FloatImage harrisCornerDetector(int window, float threshold);
    
    // feature descriptor
    void calculatePatches(float sigma = 2.f, int size = 9, bool blur = true, bool norm = true);
    
    ~PanoImage(){
        m_patches.clear();
        m_featurePoints.clear();
    }

    
    
private:
    std::vector<Vecxf> m_patches;
    std::vector<Vec2i> m_featurePoints;
    int m_patchSize;
    int m_pointCount;
    FloatImage m_image;
    FloatImage m_intensity;
    FloatImage m_chrom;
};

#endif /* PanoImage_hpp */
