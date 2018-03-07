//
//  stiches.cpp
//  Yangtze
//
//  Created by DENG XI on 3/4/18.
//
//
#include <iostream>
#include "stitches.hpp"
#include <Eigen/SVD>

FloatImage Pano::cat2images(const FloatImage &im1, const FloatImage &im2, std::vector<Vec2f> ref1, std::vector<Vec2f> ref2){
    
    // construct Ax = b homogenous equation systems
    MatrixXf A;
    A.resize(8,9);
    for(int i = 0; i < 4 ; i++){
        float x1 = ref1[i].x();
        float y1 = ref1[i].y();
        float x = ref2[i].x();
        float y = ref2[i].y();
        A.row(i * 2) << x, y, 1, 0, 0, 0, -x * x1, -y * x1, -x1;
        A.row(i * 2 + 1) << 0, 0, 0, x, y, 1, -x * y1, -y1 * y, -y1;
    }
    
//     solve by svd
    Mat3f homo;
    homo = solveHomo(A);
    Mat3f homo_inverse = homo.inverse();

    
    // calculate canvas of output image
    ImageBound im1bound = boundBox(im1);
    ImageBound im2bound = boundBoxHomo(im2, homo);
    Canvas canv = calculateCanvas(im1bound, im2bound);
    
    
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
    return output;
}



Mat3f Pano::solveHomo(MatrixXf m){
    SvdXf svd(m, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Vecxf b, x;
    b.resize(8);
    x.resize(9);
    b << 0, 0, 0, 0, 0, 0, 0, 0;
   
    // Then the solution x is the eigenvector corresponding to the only zero
    // eigenvalue of ATA which corresponds to the right most column of V
    MatrixXf t,s;
    x = (svd.matrixV()).col(8);

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

std::vector<std::vector<Vec2i>> Pano::matchDescriptors(PanoImage &pim1, PanoImage &pim2, float threshold){
    std::vector<std::vector<Vec2i>> output;

    // how to init these min?
    float min, smin, dist = 0, ratio = 0;
    Vecxf d;
    int min_index = -1, smin_index = -1;

    for (int i = 0; i < pim1.getPointCount(); ++i) {
        // for any feature point in panoImage 1, find its correspondence in panoImage 2
        // by 2-NN test
        min = pim1.getPatchSize();
        smin = min;
        min_index = -1, smin_index = -1;
        for (int j = 0; j < pim2.getPointCount(); ++j) {
            d = pim1.getPatches(i) - pim2.getPatches(j);
            dist = d.squaredNorm();
            if(dist < min){
                smin_index = min_index;
                min_index = j;
                smin = min;
                min = dist;
            }else if(dist < smin && dist != min){
                smin_index = j;
                smin = dist;
            }
        }
        ratio = min / smin;
        if(ratio < threshold){
            // good match
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

Mat3f Pano::RANSAC(PanoImage &pim1, PanoImage &pim2){
    Mat3f H;
    return H;
}

FloatImage Pano::vizMatches(PanoImage &pim1, PanoImage &pim2, std::vector<std::vector<Vec2i>> matches){
    FloatImage im1 = pim1.getImage(), im2 = pim2.getImage();
    FloatImage output(im1.sizeX() + im2.sizeX(), std::max(im1.sizeY(), im2.sizeY()),
                      im1.channels());
    int offsetX = im1.sizeX();

    for (int i = 0; i < im1.sizeX(); ++i) {
        for (int j = 0; j < im1.sizeY(); ++j) {
            for (int k = 0; k < im1.channels(); ++k) {
                output(i, j, k) = im1(i, j, k);
            }
        }
    }

    for (int i = 0; i < im2.sizeX(); ++i) {
        for (int j = 0; j < im2.sizeY(); ++j) {
            for (int k = 0; k < im2.channels(); ++k) {
                output(i + offsetX, j, k) = im2(i, j, k);
            }
        }
    }

    for (int i = 0; i < matches.size(); ++i) {
        output.drawLine(matches[i][0].x(), matches[i][0].y(), matches[i][1].x() + offsetX, matches[i][1].y());
    }

    return output;


}










