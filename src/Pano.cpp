//
//  Pano.cpp
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


FloatImage Pano::autocatnimages(std::vector<PanoImage> &pims, bool center, bool blend, bool twoscale){
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

    if(blend){
        if(twoscale)
            output = catnimagesTwoScaleBlend(ref, ims, homos);
        else
            output = catnimagesBlend(ref, ims, homos);

    }else{
        output = catnimages(ref, ims, homos);
    }
    
    return output;
}

FloatImage Pano::autocatnimages(std::vector<std::vector<PanoImage>> &pims, bool center, bool blend, bool twoscale){
    // make sure pims is not empty and pims size is not 1
    assert(pims.size() > 0 && pims[0].size() > 0);
    for (int i = 0; i < pims.size() - 1; ++i) {
        assert(pims[i].size() == pims[i].size());
    }

    // initImages (calculate features and patches)

    FloatImage output;
    vector<vector<Mat3f>> homosx;
    vector<vector<FloatImage>> ims;

    for (int j = 0; j < pims.size(); ++j) {
        for (int i = 0; i < pims[0].size(); ++i) {
            pims[j][i].harrisCornerDetector(m_window, m_harris_th);
            pims[j][i].calculatePatches(m_sigma, m_pwindow, m_blur, m_norm);
        }
    }
    int refx, refy;
    if(center){
        refy = pims.size()/2;
        refx = pims[0].size()/2;
    }
    else{
        refy = 0;
        refx = 0;
    }

    FloatImage ref(pims[refy][refx].getImage());

    // get homos on x direction
    for (int j = 0; j < pims.size(); ++j) {
        vector<FloatImage> imsx;
        vector<Mat3f> homos;
        Mat3f lhomo = Mat3f::Identity();
        for (int i = refx; i < pims[0].size() - 1; ++i) {
            Mat3f nhomo = lhomo * RANSAC(pims[j][i], pims[j][i+1], m_match_th, m_portion);
            imsx.push_back(pims[j][i+1].getImage());
            homos.push_back(nhomo);
            lhomo << nhomo;
        }

        lhomo = Mat3f::Identity();
        for (int i = refx; i > 0; --i) {
            printf("now running (%d, %d) \n", j, i);
            Mat3f nhomo = lhomo * RANSAC(pims[j][i], pims[j][i-1], m_match_th, m_portion);
            imsx.push_back(pims[j][i-1].getImage());
            homos.push_back(nhomo);
            lhomo << nhomo;
        }

        homosx.push_back(homos);
        ims.push_back(imsx);
    }

    Mat3f lhomo = Mat3f::Identity();
    for (int j = refy; j < pims.size() - 1; ++j) {
        vector<Mat3f> nhomosx;
        Mat3f nhomo = lhomo * RANSAC(pims[j][refx], pims[j + 1][refx], m_match_th, m_portion);

        Mat3f nhomos = homosx[j+1][0] * nhomo;
        nhomosx.push_back(nhomos);
        for (int i = 1; i < pims[0].size() - refx - 1; ++i) {
            printf("after x, now running (%d, %d) \n", j, i);
            Mat3f update = homosx[j+1][i] * nhomosx[i - 1];
            nhomosx.push_back(update);
        }

        Mat3f nhomos2 = homosx[j+1][pims[0].size() - refx - 1] * nhomo;
        nhomosx.push_back(nhomos2);

        for (int i = pims[0].size() - refx; i < homosx[j+1].size(); ++i) {
            printf("after x, now running (%d, %d) \n", j, i);
            Mat3f update = homosx[j+1][i] * nhomosx[i - 1];
            nhomosx.push_back(update);
        }
        nhomosx.push_back(nhomo);
        homosx[j+1] = nhomosx;
        vector<FloatImage> newims(ims[j + 1]);
        newims.push_back(pims[j+1][refx].getImage());
        ims[j + 1] = newims;
        lhomo << nhomo;
    }

    lhomo = Mat3f::Identity();
    for (int j = refy; j > 0; --j) {
        vector<Mat3f> nhomosx;

        Mat3f nhomo = lhomo * RANSAC(pims[j][refx], pims[j - 1][refx], m_match_th, m_portion);

        Mat3f nhomos = homosx[j-1][0] * nhomo;
        nhomosx.push_back(nhomos);
        for (int i = 1; i < pims[0].size() - refx - 1; ++i) {
            printf("after x, now running (%d, %d) \n", j, i);
            Mat3f update = homosx[j-1][i] * nhomosx[i - 1];
            nhomosx.push_back(update);
        }

        Mat3f nhomos2 = homosx[j-1][pims[0].size() - refx - 1] * nhomo;
        nhomosx.push_back(nhomos2);

        for (int i = pims[0].size() - refx; i < homosx[j-1].size(); ++i) {
            printf("after x, now running (%d, %d) \n", j, i);
            Mat3f update = homosx[j-1][i] * nhomosx[i - 1];
            nhomosx.push_back(update);
        }
        nhomosx.push_back(nhomo);
        homosx[j-1] = nhomosx;
        vector<FloatImage> newims(ims[j - 1]);
        newims.push_back(pims[j-1][refx].getImage());
        ims[j - 1] = newims;
        lhomo << nhomo;
    }

    vector<FloatImage> imsxy;
    vector<Mat3f> homosxy;
    for (int j = 0; j < ims.size(); ++j) {
        for (int i = 0; i < ims[j].size(); ++i) {
            imsxy.push_back(ims[j][i]);
            homosxy.push_back(homosx[j][i]);
        }
    }

    if(blend){
        if(twoscale)
            output = catnimagesTwoScaleBlend(ref, imsxy, homosxy);
        else
            output = catnimagesBlend(ref, imsxy, homosxy);

    }else{
        output = catnimages(ref, imsxy, homosxy);
    }

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

ImageBound Pano::boundBoxCrop(const FloatImage &im, Mat3f homo){
    ImageBound hb;

    Vec3f tl,tr,bl,br,ctl,ctr,cbl,cbr;
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

    ctl << std::fmax(tl.x(), bl.x()), std::fmax(tl.y(), tr.y()),1;
    ctr << std::fmax(tr.x(), br.x()), std::fmax(tl.y(), tr.y()),1;
    cbl << std::fmax(tl.x(), bl.x()), std::fmin(bl.y(), br.y()),1;
    cbr << std::fmax(tr.x(), br.x()), std::fmin(bl.y(), br.y()),1;


    hb.grow(ctl);
    hb.grow(ctr);
    hb.grow(cbl);
    hb.grow(cbr);

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
    //matchesImage.write(DATA_DIR "/output/matchesImage.png");
    matchesImage.debugWrite();

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


    for (int i = 0; i < matches.size(); ++i) {
        output.drawLine(matches[i][0].x(), matches[i][0].y(), matches[i][1].x() + offsetX, matches[i][1].y());
        output.drawSquare(matches[i][0].x(), matches[i][0].y());
        output.drawSquare(matches[i][1].x() + offsetX, matches[i][1].y());
    }



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

FloatImage Pano::autocrop(std::vector<ImageBound> bs, Vec2i offset, const FloatImage &im){
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

FloatImage Pano::devidebyWeight(FloatImage im, FloatImage weight_sum){
    FloatImage output(im);
    for(int i = 0 ; i < im.sizeX() ; i++)
        for(int j = 0 ; j < im.sizeY() ; j++){
            float w = weight_sum(i,j,0);
            for(int c = 0 ; c < im.channels() ; c++)
                output(i,j,c) = im(i,j,c) / w;
        }
    return output;
}









