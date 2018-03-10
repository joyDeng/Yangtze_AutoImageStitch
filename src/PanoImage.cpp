//
//  PanoImage.cpp
//  Yangtze
//
//  Created by DENG XI on 3/6/18.
//
//


#include "PanoImage.hpp"



ImageBound::imagebound(){
    Vec2f v1, v2;
    v1 << INFINITY,INFINITY;
    v2 << -INFINITY,-INFINITY;
    topleft = v1;
    btnright = v2;
}

void ImageBound::grow(Vec3f point){
    Vec2f v1, v2;
    v1 << std::min(topleft.x(), point.x()), std::min(topleft.y(), point.y());
    v2 << std::max(btnright.x(),point.x()), std::max(btnright.y(), point.y());
    topleft = v1;
    btnright = v2;
}


bool ImageBound::inbound(Vec3f point){
    if(point.x() < topleft.x() || point.x() > btnright.x() || point.y() < topleft.y() || point.y() > btnright.y()) return false;
    else return true;
}

PanoImage::PanoImage(const FloatImage &other, const int patchsize){
    FloatImage image(other);
    m_image = image;
    FloatImage yuv = rgb2yuv(image);
    vector<FloatImage> t = lumiChromi(other);
    m_intensity = t[0];
    m_chrom = t[1];
    m_patchSize = patchsize;
}

void PanoImage::calculatePatches(float sigma, int size, bool blur, bool norm) {
    // clear patches
    m_patchSize = size;
    m_patches.clear();
    int s = size/2;

    FloatImage image;
    // use luminance value
    std::vector<FloatImage> lc = lumiChromi(m_image);
    image = lc[0];
    if(blur){
        // blur image
        image = gaussianBlur_seperable(image, sigma);
    }

    // calculate base on patch size
    for (int p = 0; p < m_featurePoints.size(); ++p) {
        Vecxf patch(m_patchSize * m_patchSize);
        for (int i = -s; i <= s; ++i) {
            for (int j = -s; j <= s; ++j) {
                patch((j+s) * m_patchSize + (i+s)) = image.smartAccessor(m_featurePoints[p].x() + i,
                                                                 m_featurePoints[p].y() + j, 0, true);
            }
        }
        // and correct for brightness/contrast
        if(norm){
            float sd = 0, mean = patch.mean();
            for (int n = 0; n < patch.size(); ++n) {
                sd += powf(patch(n) - mean, 2);
            }
            sd = sqrtf(sd / (patch.size() - 1));
            for (int n = 0; n < patch.size(); ++n) {
                patch(n) = (patch(n) - mean)/sd;
            }
        }

        m_patches.push_back(patch);
    }

}

FloatImage PanoImage::harrisCornerDetector(int k, float threshold){
    m_featurePoints.clear();

    float emk = 0.05;// range from 0.04 ~ 0.06
    // initialize sliding window
    m_featurePoints.clear();
    MatrixXf window;
    window.resize(k,k);
    window.setOnes();
    FloatImage output(m_image);
    
    // Compute image gradients Ix and Iy for all pixels
    FloatImage Ix = gradientXY(m_intensity, true, true);
    FloatImage Iy = gradientXY(m_intensity, false, true);
    
    FloatImage R(m_intensity.sizeX(), m_intensity.sizeY(),1);
    // For each pixel compute Matrix M, compute R
    int offset = k/2;
    for(int i = offset ; i < m_image.sizeX() - offset ; i++){
        for(int j = offset ; j < m_image.sizeY() - offset; j++){
            Mat2f M;
            M.setZero();
            int offsetx = i - offset;
            int offsety = j - offset;
            for(int x = 0 ; x < k ; x++)
                for(int y = 0 ; y < k ; y++){
                    Mat2f GIxy;
                    float ix = Ix(offsetx + x, offsety + y, 0);
                    float iy = Iy(offsetx + x, offsety + y, 0);
                    GIxy << ix * ix, ix * iy, ix * iy, iy*iy;
                    M = M + GIxy;
                }
            
            Eigen::SelfAdjointEigenSolver<MatrixXf> es(M);
            Vec2f lamda = es.eigenvalues();
            float traceM = lamda.x() + lamda.y();
            R(i,j,0) = lamda.x() * lamda.y() - emk * (traceM) * (traceM);
        }
    }

    // Find points with large corner response function R
    // Take the points of locally maximum R as the detected feature points
    for(int i = offset ; i < m_image.sizeX() ; i++)
        for(int j = offset ; j < m_image.sizeY() ; j++){
            if( R(i,j,0) > threshold){
                float curR = R(i,j);
                int offsetx = i - offset;
                int offsety = j - offset;
                bool max = true;
                for(int x = 0 ; x < k && max; x++)
                    for(int y = 0 ; y < k && max; y++){
                        if(R(offsetx + x, offsety + y) > curR)
                            max = false;
                    }
                if(max){
                    m_featurePoints.push_back(Vec2i(i,j));
                    //Debug: mark points for debug
                    int offx = i - 1;
                    int offy = j - 1;
                    for(int s = 0 ; s < 3 ; s++)
                        for(int t = 0 ; t < 3 ; t++){
                            output(offx + s, offy + t, 0) = 1;
                            output(offx + s, offy + t, 1) = 0;
                            output(offx + s, offy + t, 2) = 0;
                        }
                    //Debug End
                }
            }
                
        }

    m_pointCount = m_featurePoints.size();

    // visualize feature for debugging
//    output.debugWrite();

    return output;
}

// visualize patches
// red is negative and green is positive
FloatImage PanoImage::vizPatches(){
    FloatImage output(m_image);
    int s = m_patchSize / 2;
    for (int p = 0; p < m_patches.size(); ++p) {
        for (int i = -s; i <= s; ++i) {
            for (int j = -s; j <= s; ++j) {
                if(output.inBound(i + m_featurePoints[p].x(), j + m_featurePoints[p].y()))
                    if(m_patches[p]((j+s) *m_patchSize +i+s) > 0){
                            output(i + m_featurePoints[p].x(), j + m_featurePoints[p].y(), 1) = 1.f;

                    }else{
                            output(i + m_featurePoints[p].x(), j + m_featurePoints[p].y(), 0) = 1.f;
                    }
            }
        }
    }
    return output;
}


