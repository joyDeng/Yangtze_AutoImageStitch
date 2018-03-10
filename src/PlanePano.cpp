//
//  PlanePano.cpp
//  Yangtze
//
//  Created by DENG XI on 3/9/18.
//
//

#include <stdio.h>
#include <PlanePano.h>

// cat 2 image given corresponding poin sets
FloatImage PlanePano::cat2images(const FloatImage &im1, const FloatImage &im2, Mat3f homo){
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

FloatImage PlanePano::catnimages(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos){
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
    cout << "image bound:"<<endl;
    printf("(%d, %d)", canv.length, canv.height);
    
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
        output.debugWrite();
    }
    
    
    return output;
}

FloatImage PlanePano::cat2imageBlend(const FloatImage &im1, const FloatImage &im2, Mat3f homo){
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

FloatImage PlanePano::catnimagesBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos){
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
    printf("(%d, %d)", canv.length, canv.height);
    
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

FloatImage PlanePano::mancat2images(const FloatImage &im1, const FloatImage &im2, std::vector<std::vector<Vec2f>> pairs){
    
    // construct Ax = b homogenous equation systems
    Mat3f homo = computeHomo(pairs);
    return cat2images(im1, im2, homo);
}

FloatImage PlanePano::catnimagesTwoScaleBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos, float sigma, bool lin){
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
    
    vector<ImageBound> cbounds;
    
    Canvas canv;
    
    for (int n = 0; n < homos.size(); ++n) {
        bounds.push_back(boundBoxHomo(ims[n], homos[n]));
        cbounds.push_back(boundBoxCrop(ims[n], homos[n]));
    }
    bounds.push_back(im1bound);
    
    cbounds.push_back(im1bound);
    
    // calculate canvas of output image
    canv = calculateCanvas(bounds);
    
    cout << "image bound"<<endl;
    printf("(%d, %d)", canv.length, canv.height);
    
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
    
    
    //return autocrop(cbounds, canv.offset, outputlow + outputhigh);
    return outputlow + outputhigh;

}

FloatImage PlanePano::autocrop(std::vector<ImageBound> bs, Vec2i offset, const FloatImage &im){
    Vec2f topleft, btmright;
    topleft << bs[0].topleft;
    btmright << bs[0].btnright;
    for (int i = 1; i < bs.size(); ++i) {
        topleft.x() = std::fmin(topleft.x(), bs[i].topleft.x());
        topleft.y() = std::fmax(topleft.y(), bs[i].topleft.y());
        btmright.x() = std::fmax(btmright.x(), bs[i].btnright.x());
        btmright.y() = std::fmin(btmright.y(), bs[i].btnright.y());
    }
    cout << topleft << endl;
    cout << btmright << endl;
    printf("(%d, %d) to (%d, %d)", (int)topleft.x() - offset.x(), (int)topleft.y() - offset.y(),
           (int)btmright.x() - offset.x(), (int)btmright.y() - offset.y());
    printf("dimension is (%d, %d)", im.sizeX(), im.sizeY());
    return cropImage(im, topleft.x() - offset.x(), topleft.y() - offset.y(),
                     btmright.x() - offset.x(), btmright.y() - offset.y());
}


// calculate the shift offset when conbining two images
Canvas PlanePano::calculateCanvas(ImageBound a, ImageBound b){
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

// calculate canvas of n images
Canvas PlanePano::calculateCanvas(std::vector<ImageBound> bs){
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
