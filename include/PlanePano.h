//
//  PlanePano.h
//  Yangtze
//
//  Created by DENG XI on 3/9/18.
//
//

#ifndef PlanePano_h
#define PlanePano_h
#include "Pano.hpp"

class PlanePano : public Pano{
public:
    float m_harris_th;
    float m_match_th;
    float m_sigma;
    int m_pwindow;
    int m_window;
    float m_portion;
    bool m_blur;
    bool m_norm;
    
    PlanePano():Pano(){};
    
    
    // cat 2 image given corresponding poin sets
    virtual FloatImage cat2images(const FloatImage &im1, const FloatImage &im2, Mat3f homo);
    virtual FloatImage catnimages(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos);
    virtual FloatImage cat2imageBlend(const FloatImage &im1, const FloatImage &im2, Mat3f homo);
    virtual FloatImage catnimagesBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos);
    FloatImage mancat2images(const FloatImage &im1, const FloatImage &im2, std::vector<std::vector<Vec2f>> pairs);
    virtual FloatImage catnimagesTwoScaleBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos, float sigma = 3.f, bool lin=true);
    
    FloatImage autocrop(std::vector<ImageBound> bs, Vec2i offset, const FloatImage &im);
    
    // calculate the shift offset when conbining two images
    Canvas calculateCanvas(ImageBound a, ImageBound b);
    // calculate canvas of n images
    Canvas calculateCanvas(std::vector<ImageBound> bs);
    
};

#endif /* PlanePano_h */
