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
    FloatImage yuv = rgb2yuv(image);
    vector<FloatImage> t = lumiChromi(other);
    m_intensity = t[0];
    m_chrom = t[1];
    m_patchSize = patchsize;
}

void PanoImage::calculatePatches(float sigma, int size, bool blur, bool norm) {
    // 0. clear patches
    // 1. use luminance value
    // 2. blur image a little bit
    // 3. calculate base on patch size
    // and correct for brightness/contrast
    m_patchSize = size;
    m_patches.clear();
    int s = size/2;


    FloatImage image;
    std::vector<FloatImage> lc = lumiChromi(m_image);
    image = lc[0];
    if(blur){
        image = gaussianBlur_seperable(image, sigma);
    }
    if(norm){
        image = normalizeBySD(image);
    }


    for (int p = 0; p < m_featurePoints.size(); ++p) {
        Vecxf patch(m_patchSize * m_patchSize);
        for (int i = -s; i <= s; ++i) {
            for (int j = -s; j <= s; ++j) {
                patch((i+s) * m_patchSize + (j+s)) = image.smartAccessor(m_featurePoints[p].x() + i,
                                                                 m_featurePoints[p].y() + j, 0);
            }
        }
        m_patches.push_back(patch);
        //std::cout << patch << std::endl << std::endl;
    }

}

FloatImage PanoImage::harrisCornerDetector(int k, float threshold){
    float emk = 0.05;// range from 0.04 ~ 0.06
    // initialize sliding window
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

    return output;
}


