//
//  stiches.cpp
//  Yangtze
//
//  Created by DENG XI on 3/4/18.
//
//
#include <iostream>
#include "Pano.hpp"
#include <Eigen/SVD>
#include <set>

using std::vector;
using std::cout;
using std::endl;

Mat3f Pano::computeHomo(std::vector<std::vector<Vec2f>> pairs){
    // construct Ax = b homogenous equation systems
    MatrixXf A;
    int rowsize = pairs.size() * 2;
    A.resize(rowsize,9);
    for(int i = 0; i < pairs.size() ; i++){
        float x1 = pairs[i][0].x();
        float y1 = pairs[i][0].y();
        float x = pairs[i][1].x();
        float y = pairs[i][1].y();
//        printf("Best Match: (%d, %d) to (%d, %d)\n", (int)pairs[i][0].x(), (int)pairs[i][0].y(),
//               (int)pairs[i][1].x(), (int)pairs[i][1].y());
        A.row(i * 2) << x, y, 1, 0, 0, 0, -x * x1, -y * x1, -x1;
        A.row(i * 2 + 1) << 0, 0, 0, x, y, 1, -x * y1, -y1 * y, -y1;
    }

    return solveHomo(A);
}


FloatImage Pano::autocat2images(PanoImage &pim1, PanoImage &pim2, bool blend){

    FloatImage im1 = pim1.getImage(), im2 = pim2.getImage();
    pim1.harrisCornerDetector(m_window, m_harris_th);
    pim2.harrisCornerDetector(m_window, m_harris_th);
    pim1.calculatePatches(m_sigma, m_pwindow, m_blur, m_norm);
    pim2.calculatePatches(m_sigma, m_pwindow, m_blur, m_norm);

    Mat3f homo = RANSAC(pim1, pim2, m_match_th, m_portion);

    cout << "ransac done"<<endl;
    
    if(blend) return cat2imageBlend(im1, im2, homo);
    return cat2images(im1, im2, homo);

}

FloatImage Pano::cat2images(const FloatImage &im1, const FloatImage &im2, Mat3f homo) {
    Mat3f homo_inverse = homo.inverse();
    // calculate canvas of output image
    ImageBound im1bound = boundBox(im1);
    ImageBound im2bound = boundBoxHomo(im2, homo);
    Canvas canv = calculateCanvas(im1bound, im2bound);

    cout << "image bound"<<endl;

    //paste image1 onto canvas
    FloatImage output(canv.length, canv.height, im1.channels());
    for(int i = 0 ; i < im1.sizeX() ; i++)
        for(int j = 0 ; j < im1.sizeY() ; j++){
            int nx  = i - canv.offset.x();
            int ny = j - canv.offset.y();
            if(nx >= 0 && ny >= 0 && nx < canv.length && ny < canv.height)
                for(int c = 0 ; c < im1.channels() ; c++)
                    output(nx, ny, c) = im1(i, j, c);
        }
    cout << "image 1 done"<<endl;
    //query image2 and map onto canvas
    Vec2i offsetImage2 = Vec2i(floor(im2bound.topleft.x()), floor(im2bound.topleft.y())) - canv.offset;
    Vec2f sizeTransedImage2 = im2bound.btnright - im2bound.topleft;

    for(int i = 0 ; i < sizeTransedImage2.x(); i++){
        for(int j = 0 ; j < sizeTransedImage2.y() ; j++){
            Vec2f transed_pos = im2bound.topleft + Vec2f(i,j);
            Vec3f pos_f = homo_inverse * Vec3f(transed_pos.x(), transed_pos.y(), 1);
            Vec2i pos(floor(pos_f.x()/pos_f.z()), floor(pos_f.y()/pos_f.z()));
            if(pos.x() >= 0 && pos.y() >= 0 && pos.x() < im2.sizeX() && pos.y() < im2.sizeY()){
                Vec2i canvas_pos = offsetImage2 + Vec2i(i,j);
                if(canvas_pos.x() > 0 && canvas_pos.y() > 0 && canvas_pos.y() < canv.height && canvas_pos.x() < canv.length)
                    for(int c = 0 ; c < im2.channels() ; c++)
                        output(canvas_pos.x(), canvas_pos.y(), c) = im2(pos.x(), pos.y(),c);
            }
        }
    }
    cout << "cat 2 image done"<<endl;
    return output;
}

//<<<<<<< HEAD
FloatImage Pano::autocatnimages(std::vector<PanoImage> &pims, bool center){
    // make sure pims is not empty and pims size is not 1

    // initImages (calculate features and patches)
    vector<FloatImage> ims;
    vector<Mat3f> homos;
    FloatImage output;


    for (int i = 0; i < pims.size(); ++i) {
        pims[i].harrisCornerDetector(m_window, m_harris_th);
        pims[i].calculatePatches(m_sigma, m_pwindow, m_blur, m_norm);
    }
    int refn;
    if(center){
        refn = pims.size()/2;}
    else{
        refn = 0;
    }

    FloatImage ref(pims[refn].getImage());

    Mat3f lhomo = Mat3f::Identity();
    for (int i = refn; i < pims.size() - 1; ++i) {
        Mat3f nhomo = lhomo * RANSAC(pims[i], pims[i+1], m_match_th, m_portion);
        ims.push_back(pims[i+1].getImage());
        homos.push_back(nhomo);
        lhomo << nhomo;
    }

    lhomo = Mat3f::Identity();
    for (int i = refn; i > 0; --i) {
        Mat3f nhomo = lhomo * RANSAC(pims[i], pims[i-1], m_match_th, m_portion);
        ims.push_back(pims[i-1].getImage());
        homos.push_back(nhomo);
        lhomo << nhomo;
    }

    output = catnimagesTwoScaleBlend(ref, ims, homos);
//    }

    return output;
}

FloatImage Pano::catnimages(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos){
    // ims are FloatImages from 0 to n
    // 0 is the reference image

    vector<Mat3f> invHomos;
    for (int n = 0; n < homos.size(); ++n) {
        invHomos.push_back(homos[n].inverse());
    }
    ImageBound im1bound = boundBox(ref);

    vector<ImageBound> bounds;
    Canvas canv;

    for (int n = 0; n < homos.size(); ++n) {
        bounds.push_back(boundBoxHomo(ims[n], homos[n]));
    }
    bounds.push_back(im1bound);
    // calculate canvas of output image
    canv = calculateCanvas(bounds);
    cout << "image bound"<<endl;

    //paste image1 onto canvas
    FloatImage output(canv.length, canv.height, ref.channels());

    for(int i = 0 ; i < ref.sizeX() ; i++) {
        for (int j = 0; j < ref.sizeY(); j++) {
            int nx = i - canv.offset.x();
            int ny = j - canv.offset.y();
            if (nx >= 0 && ny >= 0 && nx < canv.length && ny < canv.height)
                for (int c = 0; c < ref.channels(); c++)
                    output(nx, ny, c) = ref(i, j, c);
        }
    }
    cout << "image ref done"<<endl;

    for (int n = 0; n < homos.size(); ++n) {
        Vec2i offsetImage = Vec2i(floor(bounds[n].topleft.x()), floor(bounds[n].topleft.y())) - canv.offset;
        Vec2f sizeTransedImage = bounds[n].btnright - bounds[n].topleft;

        for (int i = 0; i < sizeTransedImage.x(); ++i) {
            for (int j = 0; j < sizeTransedImage.y(); ++j) {
                Vec2f transed_pos = bounds[n].topleft + Vec2f(i,j);

                Vec3f pos_f = invHomos[n] * Vec3f(transed_pos.x(), transed_pos.y(), 1);
                Vec2i pos(floor(pos_f.x()/pos_f.z()), floor(pos_f.y()/pos_f.z()));

                if(ims[n].inBound(pos.x(), pos.y())){
                    Vec2i canvas_pos = offsetImage + Vec2i(i,j);
                    if(canvas_pos.x() >= 0 && canvas_pos.y() >= 0 && canvas_pos.y() < canv.height && canvas_pos.x() < canv.length)
                        for(int c = 0 ; c < ims[n].channels() ; c++)
                            output(canvas_pos.x(), canvas_pos.y(), c) = ims[n](pos.x(), pos.y(),c);
                }
            }
        }
        cout << "image " << n + 1<< " done"<<endl;
    }


    return output;

}

FloatImage Pano::catnimagesBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos){
    // ims are FloatImages from 0 to n
    // 0 is the reference image

    // init weight maps
    FloatImage refWeight = calweight(ref.sizeX(), ref.sizeY());
    vector<FloatImage> weights;
    for (int n = 0; n < ims.size(); ++n) {
        weights.push_back(calweight(ims[n].sizeX(), ims[n].sizeY()));
    }

    // init inv homos
    vector<Mat3f> invHomos;
    for (int n = 0; n < homos.size(); ++n) {
        invHomos.push_back(homos[n].inverse());
    }

    // init imagebounds
    ImageBound im1bound = boundBox(ref);
    vector<ImageBound> bounds;
    Canvas canv;

    for (int n = 0; n < homos.size(); ++n) {
        bounds.push_back(boundBoxHomo(ims[n], homos[n]));
    }
    bounds.push_back(im1bound);
    // calculate canvas of output image
    canv = calculateCanvas(bounds);
    cout << "image bound"<<endl;

    FloatImage canv_w(canv.length, canv.height, 1);

    //paste image1 onto canvas
    FloatImage output(canv.length, canv.height, ref.channels());

    canv_w.debugWrite();

    for(int i = 0 ; i < ref.sizeX() ; i++) {
        for (int j = 0; j < ref.sizeY(); j++) {
            int nx = i - canv.offset.x();
            int ny = j - canv.offset.y();
            if (nx >= 0 && ny >= 0 && nx < canv.length && ny < canv.height) {
                canv_w(nx, ny, 0) += refWeight(i, j, 0);
                for (int c = 0; c < ref.channels(); c++) {
                    output(nx, ny, c) += (ref(i, j, c) * refWeight(i, j, 0));

                }
            }
        }
    }
    cout << "image ref done"<<endl;


    for (int n = 0; n < homos.size(); ++n) {
        Vec2i offsetImage = Vec2i(floor(bounds[n].topleft.x()), floor(bounds[n].topleft.y())) - canv.offset;
        Vec2f sizeTransedImage = bounds[n].btnright - bounds[n].topleft;

        for (int i = 0; i < sizeTransedImage.x(); ++i) {
            for (int j = 0; j < sizeTransedImage.y(); ++j) {
                Vec2f transed_pos = bounds[n].topleft + Vec2f(i,j);

                Vec3f pos_f = invHomos[n] * Vec3f(transed_pos.x(), transed_pos.y(), 1);
                Vec2i pos(floor(pos_f.x()/pos_f.z()), floor(pos_f.y()/pos_f.z()));

                if(ims[n].inBound(pos.x(), pos.y())){
                    Vec2i canvas_pos = offsetImage + Vec2i(i,j);
                    if(canvas_pos.x() >= 0 && canvas_pos.y() >= 0 && canvas_pos.y() < canv.height && canvas_pos.x() < canv.length) {
                        canv_w(canvas_pos.x(), canvas_pos.y(), 0) += weights[n](pos.x(), pos.y(), 0);
                        for (int c = 0; c < ims[n].channels(); c++) {
                            output(canvas_pos.x(), canvas_pos.y(), c) +=
                                    (ims[n](pos.x(), pos.y(), c) * weights[n](pos.x(), pos.y(), 0));

                        }
                    }
                }
            }
        }
        cout << "image " << n + 1<< " done"<<endl;
    }


    canv_w.debugWrite();
    for (int i = 0; i < canv.length; ++i) {
        for (int j = 0; j < canv.height; ++j) {
            if(canv_w(i, j, 0) > 0){
                for (int c = 0; c < output.channels(); ++c) {
                    cout << canv_w(i, j, 0) <<endl;
                    output(i, j, c) = output(i, j, c) / canv_w(i, j, 0);
                }
            }
        }
    }


    return output;

}

FloatImage Pano::catnimagesTwoScaleBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos, float sigma, bool lin){
    // ims are FloatImages from 0 to n
    // 0 is the reference image

    // two-scale separation
    // using separable gaussian
    // init filters
    vector<float> fData = gauss1DFilterValues(sigma, 3.0);
    Filter gaussX(fData, fData.size(), 1);
    Filter gaussY(fData, 1, fData.size());

    // init low pass
    FloatImage low_ref = gaussY.Convolve(gaussX.Convolve(ref));
    vector<FloatImage> lows;
    for (int n = 0; n < ims.size(); ++n) {
        lows.push_back(gaussY.Convolve(gaussX.Convolve(ims[n])));
    }

    // init high pass
    FloatImage hi_ref = ref - low_ref;
    vector<FloatImage> highs;
    for (int n = 0; n < ims.size(); ++n) {
        highs.push_back(ims[n] - lows[n]);
    }

    // init weight maps
    FloatImage refWeight = calweight(ref.sizeX(), ref.sizeY());
    vector<FloatImage> weights;
    for (int n = 0; n < ims.size(); ++n) {
        weights.push_back(calweight(ims[n].sizeX(), ims[n].sizeY()));
    }

    // init inv homos
    vector<Mat3f> invHomos;
    for (int n = 0; n < homos.size(); ++n) {
        invHomos.push_back(homos[n].inverse());
    }

    // init imagebounds
    ImageBound im1bound = boundBox(ref);
    vector<ImageBound> bounds;
    Canvas canv;

    for (int n = 0; n < homos.size(); ++n) {
        bounds.push_back(boundBoxHomo(ims[n], homos[n]));
    }
    bounds.push_back(im1bound);
    // calculate canvas of output image
    canv = calculateCanvas(bounds);
    cout << "image bound"<<endl;

    FloatImage canv_w(canv.length, canv.height, 1);
    FloatImage canv_maxw(canv.length, canv.height, 1);

    //paste image1 onto canvas
    FloatImage outputlow(canv.length, canv.height, ref.channels());
    FloatImage outputhigh(canv.length, canv.height, ref.channels());

    //canv_w.debugWrite();

    bool ismax;
    for(int i = 0 ; i < ref.sizeX() ; i++) {
        for (int j = 0; j < ref.sizeY(); j++) {
            int nx = i - canv.offset.x();
            int ny = j - canv.offset.y();
            if (nx >= 0 && ny >= 0 && nx < canv.length && ny < canv.height) {
                canv_w(nx, ny, 0) += refWeight(i, j, 0);
                ismax = refWeight(i, j, 0) > canv_maxw(nx, ny, 0);
                if(ismax) canv_maxw(nx, ny, 0) = refWeight(i, j, 0);
                for (int c = 0; c < ref.channels(); c++) {
                    outputlow(nx, ny, c) += (low_ref(i, j, c) * refWeight(i, j, 0));
                    if (ismax) outputhigh(nx, ny, c) = hi_ref(i, j, c);
                }
            }
        }
    }
    cout << "image ref done"<<endl;


    for (int n = 0; n < homos.size(); ++n) {
        Vec2i offsetImage = Vec2i(floor(bounds[n].topleft.x()), floor(bounds[n].topleft.y())) - canv.offset;
        Vec2f sizeTransedImage = bounds[n].btnright - bounds[n].topleft;

        for (int i = 0; i < sizeTransedImage.x(); ++i) {
            for (int j = 0; j < sizeTransedImage.y(); ++j) {
                Vec2f transed_pos = bounds[n].topleft + Vec2f(i,j);

                Vec3f pos_f = invHomos[n] * Vec3f(transed_pos.x(), transed_pos.y(), 1);
                pos_f /= pos_f.z();
                Vec2i pos(floor(pos_f.x()/pos_f.z()), floor(pos_f.y()/pos_f.z()));

                if(ims[n].inBound(pos.x(), pos.y())){
                    Vec2i canvas_pos = offsetImage + Vec2i(i,j);
                    if(canvas_pos.x() >= 0 && canvas_pos.y() >= 0 && canvas_pos.y() < canv.height && canvas_pos.x() < canv.length) {
                        canv_w(canvas_pos.x(), canvas_pos.y(), 0) += weights[n](pos.x(), pos.y(), 0);
                        if(lin)
                            ismax = interpolateLin(weights[n], pos.x(), pos.y(), 0) > canv_maxw(canvas_pos.x(), canvas_pos.y(), 0);
                        else
                            ismax = refWeight(pos.x(), pos.y(), 0) > canv_maxw(canvas_pos.x(), canvas_pos.y(), 0);

                        if(ismax) canv_maxw(canvas_pos.x(), canvas_pos.y(), 0) =
                                          lin ? interpolateLin(weights[n], pos_f.x(), pos_f.y(), 0): weights[n](pos.x(), pos.y(), 0);
                        for (int c = 0; c < ims[n].channels(); c++) {
                            outputlow(canvas_pos.x(), canvas_pos.y(), c) +=
                                    (lows[n](pos.x(), pos.y(), c) * weights[n](pos.x(), pos.y(), 0));
                            if(lin)
                                if(ismax) outputhigh(canvas_pos.x(), canvas_pos.y(), c) = interpolateLin(highs[n], pos_f.x(), pos_f.y(), c);
                            else
                                if(ismax) outputhigh(canvas_pos.x(), canvas_pos.y(), c) = highs[n](pos.x(), pos.y(), c);

                        }
                    }
                }
            }
        }
        cout << "image " << n + 1<< " done"<<endl;
    }


    canv_w.debugWrite();
    for (int i = 0; i < canv.length; ++i) {
        for (int j = 0; j < canv.height; ++j) {
            if(canv_w(i, j, 0) > 0){
                for (int c = 0; c < outputlow.channels(); ++c) {
                    outputlow(i, j, c) = outputlow(i, j, c) / canv_w(i, j, 0);
                }
            }
        }
    }


    return outputlow + outputhigh;

}


FloatImage Pano::cat2imageBlend(const FloatImage &im1, const FloatImage &im2, Mat3f homo){
    //calculate weight map for both image
    FloatImage w1 = calweight(im1.sizeX(), im1.sizeY());
    FloatImage w2 = calweight(im2.sizeX(), im2.sizeY());
    Mat3f homo_i = homo.inverse();
    
    //calculate outputimage size
    ImageBound im1bound = boundBox(im1);
    ImageBound im2bound = boundBoxHomo(im2, homo);
    Canvas canv = calculateCanvas(im1bound, im2bound);
    FloatImage output(canv.length, canv.height, im1.channels());
    
    //write area of image1
    Vec2f im1area = im1bound.btnright - im1bound.topleft;
    int area1x = (int)im1area.x();
    int area1y = (int)im1area.y();
    //attach image1
    for(int i = 0 ; i < area1x ; i++){
        for(int j = 0 ; j < area1y ; j++){
            Vec2i pos = Vec2i(i,j) - canv.offset;
            if(pos.x() >= 0 && pos.y() >= 0 && pos.x() < canv.length && pos.y() < canv.height){
                float weightsum = w1(i,j,0);
                Vec3f curpos = Vec3f(i,j,1), re_pos;
                int ix,iy;
                bool in2 = false;
                // if overlap with im2
                if(im2bound.inbound(curpos)){
                    re_pos = homo_i * curpos;
                    re_pos /= re_pos.z();
                    ix = floor(re_pos.x());
                    iy = floor(re_pos.y());
                    if(ix < 0 || iy < 0 || ix >= im2.sizeX() || iy >= im2.sizeY()) in2 = false;
                    else in2 = true;
                    if(in2) weightsum += w2(ix, iy, 0);
                }
                for(int c = 0 ; c < im1.channels() ; c++){
                    float nume = im1(i,j,c) *  w1(i,j,0);
                    if(in2) nume += im2(ix, iy, c) * w2(ix, iy, 0);
                    output(pos.x(), pos.y(), c) = nume / weightsum;
                }
            }
        }
    }
    //Debug: output
    cout << "image 1 done"<<endl;
    //query image2 and map onto canvas
    Vec2i offsetImage2 = Vec2i(floor(im2bound.topleft.x()), floor(im2bound.topleft.y())) - canv.offset;
    Vec2f sizeTransedImage2 = im2bound.btnright - im2bound.topleft;
    
    // attach image2
    for(int i = 0 ; i < sizeTransedImage2.x(); i++){
        for(int j = 0 ; j < sizeTransedImage2.y() ; j++){
            Vec2f transed_pos = im2bound.topleft + Vec2f(i,j);
            Vec3f pos_f = homo_i * Vec3f(transed_pos.x(), transed_pos.y(), 1);
            Vec2i pos(floor(pos_f.x()/pos_f.z()), floor(pos_f.y()/pos_f.z()));
            if(pos.x() >= 0 && pos.y() >= 0 && pos.x() < im2.sizeX() && pos.y() < im2.sizeY()){
                Vec2i canvas_pos = offsetImage2 + Vec2i(i,j);
                bool in1 = im1bound.inbound(Vec3f(transed_pos.x(), transed_pos.y(), 1));
                float weightsum = w2(pos.x(), pos.y(), 0);
                if(in1) weightsum += w1(transed_pos.x(), transed_pos.y(), 0);
                if(canvas_pos.x() > 0 && canvas_pos.y() > 0 && canvas_pos.y() < canv.height && canvas_pos.x() < canv.length)
                    for(int c = 0 ; c < im2.channels() ; c++){
                        float nemo = im2(pos.x(), pos.y(),c) * w2(pos.x(), pos.y(), 0);
                        if(in1) nemo += im1(transed_pos.x(), transed_pos.y(), c) * w1(transed_pos.x(), transed_pos.y(), 0);
                        output(canvas_pos.x(), canvas_pos.y(), c) = nemo / weightsum;
                    }
            }
        }
    }
    cout << "cat 2 image done"<<endl;
    return output;
}


FloatImage Pano::calweight(int sizex, int sizey, bool gau, float ratio){
    FloatImage weightmap(sizex,sizey,1);
    float cx = sizex / 2;
    float cy = sizey / 2;

    if(gau){
        float sigmax = cx * ratio, sigmay = cy * ratio;
        for (int i = 0; i < sizex; ++i) {
            for (int j = 0; j < sizey; ++j) {
                weightmap(i, j, 0) =  exp(-powf(i - cx, 2) / (2.0 * powf(sigmax, 2))) *
                                      exp(-powf(j - cy, 2) / (2.0 * powf(sigmay, 2)));
            }

        }
    }else{
        for(int i = 0 ; i < sizex ; i++){
            float k = fabsf(i - cx) / (float)cx;
            for(int j = 0 ; j < sizey ; j++){
                weightmap(i,j,0) = lerp(k , 0, 1);
            }
        }
        for(int j = 0 ; j < sizey ; j++){
            float k = fabsf(j - cy) / (float)cy;
            for(int i = 0 ; i < sizex ; i++){
                weightmap(i,j,0) = sqrt(weightmap(i,j,0) * lerp(k , 0, 1)) ;
            }
        }
    }
//Debug write: weightmap.write(DATA_DIR "/output/test_weightmap.png");
    return weightmap;
}


FloatImage Pano::mancat2images(const FloatImage &im1, const FloatImage &im2, std::vector<std::vector<Vec2f>> pairs){

    // construct Ax = b homogenous equation systems
    Mat3f homo = computeHomo(pairs);
    return cat2images(im1, im2, homo);


}

Mat3f Pano::solveHomo(MatrixXf m){

    SvdXf svd(m, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Vecxf  x;
    x.resize(9);

    // Then the solution x is the eigenvector corresponding to the only zero
    // eigenvalue of ATA which corresponds to the right most column of V
    MatrixXf t,s;
    s = (svd.matrixV());
    int colcount = s.cols();
    x = s.col(colcount - 1);

    Mat3f H;
    float k = 1.0 / x[8];
    H.row(0) <<  k * x[0], k * x[1],k * x[2];
    H.row(1) <<  k * x[3], k * x[4],k * x[5];
    H.row(2) << k * x[6], k * x[7], k * x[8];


    return H;
}


ImageBound Pano::boundBox(const FloatImage &im){
    ImageBound hb;
    
    Vec3f tl,tr,bl,br;
    tl << 0,  0, 1;
    tr << im.sizeX(), 0, 1;
    bl << 0, im.sizeY(), 1;
    br << im.sizeX(), im.sizeY(), 1;
    
    
    hb.grow(tl);
    hb.grow(tr);
    hb.grow(bl);
    hb.grow(br);
    
    return hb;
}

ImageBound Pano::boundBoxHomo(const FloatImage &im, Mat3f homo){
    ImageBound hb;
    
    Vec3f tl,tr,bl,br;
    tl << 0,  0, 1;
    tr << im.sizeX(), 0, 1;
    bl << 0, im.sizeY(), 1;
    br << im.sizeX(), im.sizeY(), 1;
    
    tl = homo * tl;
    tr = homo * tr;
    bl = homo * bl;
    br = homo * br;
    
    tl = tl / tl.z();
    tr = tr / tr.z();
    bl = bl / bl.z();
    br = br / br.z();

    hb.grow(tl);
    hb.grow(tr);
    hb.grow(bl);
    hb.grow(br);
    
    return hb;
    
}

Canvas Pano::calculateCanvas(ImageBound a, ImageBound b){
    
    ImageBound ab;
    ab.grow(Vec3f(a.topleft.x(),  a.topleft.y(), 1));
    ab.grow(Vec3f(a.btnright.x(), a.btnright.y(),1));
    ab.grow(Vec3f(b.topleft.x(),  b.topleft.y(), 1));
    ab.grow(Vec3f(b.btnright.x(), b.btnright.y(),1));
    
    Canvas canv;
    
    // calculate offset of image canvas and the size of canvas
    Vec2i can;
    can << floor(ab.topleft.x()), floor(ab.topleft.y());
    canv.offset = can;
    canv.length = ceil(ab.btnright.x() - ab.topleft.x());
    canv.height = ceil(ab.btnright.y() - ab.topleft.y());
    return canv;
    
}

Canvas Pano::calculateCanvas(vector<ImageBound> bs){


    ImageBound b;
    for (int i = 0; i < bs.size(); ++i) {
        b.grow(Vec3f(bs[i].topleft.x(), bs[i].topleft.y(), 1));
        b.grow(Vec3f(bs[i].btnright.x(), bs[i].btnright.y(),1));

    }
    Canvas canv;
    Vec2i can;
    can << floor(b.topleft.x()), floor(b.topleft.y());
    canv.offset = can;
    canv.length = ceil(b.btnright.x() - b.topleft.x());
    canv.height = ceil(b.btnright.y() - b.topleft.y());
    return canv;

}

std::vector<std::vector<Vec2i>> Pano::matchDescriptors(PanoImage &pim1, PanoImage &pim2, float threshold){
    std::vector<std::vector<Vec2i>> output;
    // how to init these min?
    float min, smin, dist = 0, ratio = 0;
    Vecxf d;
    int min_index = -1, smin_index = -1;
    vector<int> found;

    for (int i = 0; i < pim1.getPointCount(); ++i) {
        // for any feature point in panoImage 1, find its correspondence in panoImage 2
        // by 2-NN test
        min = pim1.getPatchSize();
        smin = min;
        min_index = -1, smin_index = -1;
        for (int j = 0; j < pim2.getPointCount(); ++j) {
            if(std::find(found.begin(), found.end(), j) == found.end()) {
                d = pim1.getPatches(i) - pim2.getPatches(j);
                dist = d.norm();
                if (dist < min) {
                    smin_index = min_index;
                    min_index = j;
                    smin = min;
                    min = dist;
                } else if (dist < smin && dist != min) {
                    smin_index = j;
                    smin = dist;
                }
            }
        }
        ratio = min / smin;
        if(ratio < threshold){
            // good match
            found.push_back(min_index);
            std::vector<Vec2i> refs;
            Vec2i ref1, ref2;
            ref1 << pim1.getFeaturePoint(i).x(), pim1.getFeaturePoint(i).y();
            ref2 << pim2.getFeaturePoint(min_index).x(), pim2.getFeaturePoint(min_index).y();
            printf("Match: (%d, %d) to (%d, %d)\n", ref1.x(), ref1.y(), ref2.x(), ref2.y());
            printf("min_index is: %d, min_index y is: %d\n", min_index, pim2.getFeaturePoint(min_index).y());


            refs.push_back(ref1);
            refs.push_back(ref2);
            output.push_back(refs);
        }
    }
    return output;
}

//RANSAC for estimatimating homography
Mat3f Pano::RANSAC( PanoImage &pim1,PanoImage &pim2, float match_th, float portion, float accuBound, float threshold){
    Mat3f H;
    // need to make sure pairs > 4s
    vector<vector<Vec2i>> pairs = matchDescriptors(pim1, pim2, match_th);

    FloatImage matchesImage = vizMatches(pim1, pim2, pairs);
    matchesImage.write(DATA_DIR "/output/matchesImage.png");

    vector<vector<Vec2f>> Largest_inliers;
    Mat3f Homo;
    float Prob = 1;
    float failProb = 1 - std::powf(portion,4);
    uint32_t maxInlinerSize = 0;
    
    pcg32 rng;
    uint32_t rndBound = pairs.size();
    vector<vector<Vec2f>> bestPairs;
    //Ransac loop: stop when the failure probability
    //of finding the correct H is low

    int iter = (int)(logf(0.05) / logf(1 - powf(portion, 4)));
    int iterx = 0;
    cout << iter << endl;
    cout << "number of pairs: "<<rndBound<<endl;

    while(Prob > accuBound && iterx < iter){
        std::cout<<"iter: "<<iterx<<" of"<<iter<<std::endl;
        vector<vector<Vec2f>> inliers;
        //select four feature pairs(at random)
        vector<vector<Vec2f>> ranPairs;
        std::set<int> has;
        std::set<int>::iterator it;
        for(int i = 0 ; i < 4 ; i++){
            int index = rng.nextUInt(rndBound);
            while(has.find(index) != has.end())
                index = rng.nextUInt(rndBound);
            has.insert(index);
            std::vector<Vec2i> p = pairs[index];
            Vec2f p0 = Vec2f(p[0].x(), p[0].y());
            Vec2f p1 = Vec2f(p[1].x(), p[1].y());
            vector<Vec2f> pair;
            pair.push_back(p0);
            pair.push_back(p1);
            ranPairs.push_back(pair);
        }
        Mat3f H = computeHomo(ranPairs);
        //compute inliers where ||pi, Hpi|| < epsillon
        for(int i = 0 ; i < (int)rndBound ; i++){
            Vec3f hp = H * Vec3f(pairs[i][1].x(),pairs[i][1].y(),1);
            hp = hp / hp.z();
            Vec3f ep =  hp - Vec3f(pairs[i][0].x(),pairs[i][0].y(),1);

            if(ep.norm() < threshold){
                std::vector<Vec2i> p = pairs[i];
                Vec2f p0 = Vec2f(p[0].x(), p[0].y());
                Vec2f p1 = Vec2f(p[1].x(), p[1].y());
                vector<Vec2f> pair;
                pair.push_back(p0);
                pair.push_back(p1);
                inliers.push_back(pair);
            }
        }
        
        //Keep largest set of inliers
        if(inliers.size() > maxInlinerSize){
            maxInlinerSize = inliers.size();
            Homo = H;
            //Largest_inliers.clear();
            Largest_inliers = inliers;
            bestPairs = ranPairs;

        }
        iterx++;
    }

    // visualize
    FloatImage viz = vizMatches(pim1, pim2, bestPairs);
    viz.debugWrite();

    // output best
    for (int i = 0; i < 4; ++i) {
        printf("Best Match: (%d, %d) to (%d, %d)\n", (int)bestPairs[i][0].x(), (int)bestPairs[i][0].y(),
               (int)bestPairs[i][1].x(), (int)bestPairs[i][1].y());
    }

    cout << Homo << endl;
    //Homo = computeHomo(Largest_inliers);
    viz = vizMatches(pim1, pim2, Largest_inliers);
    viz.debugWrite();

    return Homo;
}


FloatImage Pano::vizMatches(PanoImage &pim1, PanoImage &pim2, std::vector<std::vector<Vec2i>> matches){
    FloatImage im1 = pim1.getImage(), im2 = pim2.getImage();
    FloatImage output(im1.sizeX() + im2.sizeX(), std::max(im1.sizeY(), im2.sizeY()),
                      im1.channels());
    int offsetX = im1.sizeX();

    for (int i = 0; i < im1.sizeX(); ++i)
        for (int j = 0; j < im1.sizeY(); ++j)
            for (int k = 0; k < im1.channels(); ++k)
                output(i, j, k) = im1(i, j, k);


    for (int i = 0; i < im2.sizeX(); ++i)
        for (int j = 0; j < im2.sizeY(); ++j)
            for (int k = 0; k < im2.channels(); ++k)
                output(i + offsetX, j, k) = im2(i, j, k);


    for (int i = 0; i < matches.size(); ++i)
        output.drawLine(matches[i][0].x(), matches[i][0].y(), matches[i][1].x() + offsetX, matches[i][1].y());

    return output;


}

FloatImage Pano::vizMatches(PanoImage &pim1, PanoImage &pim2, std::vector<std::vector<Vec2f>> matches){
    std::vector<std::vector<Vec2i>> matchesi;
    for (int i = 0; i < matches.size(); ++i) {
        std::vector<Vec2i> match;
        match.push_back(Vec2i(matches[i][0].x(), matches[i][0].y()));
        match.push_back(Vec2i(matches[i][1].x(), matches[i][1].y()));
        matchesi.push_back(match);
    }
    return vizMatches(pim1, pim2, matchesi);


}









