//
//Created by Xi Deng on 2018.03.03
//
//
//

#include <iostream>
#include <Eigen/Geometry>
#include "common.h"
#include "floatimage.h"

#include "Pano.hpp"
#include "PanoImage.hpp"
#include "SpherePano.h"
#include "utils.h"

//using namespace std;
//using namespace Eigen;



void testCatYosemite();
void testCatNYosemite();
void testCatTable();
void testCatPan();
void testCatNPan();
void testHome();
void testNHome(int start, int end);
void testSphere();
void testNSphere(int start, int end);

int main(){

//    testNHome(5181, 5186);
    testNSphere(5181, 5186);

    return 0;

}

void testCatTable(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.1f);
    PanoImage pim1(FloatImage(DATA_DIR "/input/table1.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/table2.jpg"));
    //FloatImage autocat = pano.autocat2images(pim1, pim2);
    FloatImage autocat = pano.autocat2images(pim1, pim2, true);
    autocat.write(DATA_DIR "/output/auto_table_left_right.png");

}

void testCatLily(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);

    PanoImage pim1(FloatImage(DATA_DIR "/input/lily1.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/lily2.jpg"));
//    vector<PanoImage> pims;
//    pims.push_back(pim1);
//    pims.push_back(pim2);
    FloatImage autocat = pano.autocat2images(pim1, pim2);
//    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_lily_left_right.png");

}

void testCatPan(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.1f);
    PanoImage pim1(FloatImage(DATA_DIR "/input/pan12.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/pan13.jpg"));
    FloatImage autocat = pano.autocat2images(pim1, pim2);
    autocat.write(DATA_DIR "/output/auto_pan_left_right.png");

}

void testCatNPan(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.1f);
    std::vector<PanoImage> pims;
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/pan11.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/pan12.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/pan13.jpg")));
    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_pan.png");

}


void testCatYosemite(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(21);
    pano.setMatchTh(0.6f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.1f);
    PanoImage pim1(FloatImage(DATA_DIR "/input/yosemite1.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/yosemite2.jpg"));
    FloatImage autocat = pano.autocat2images(pim1, pim2);
    autocat.write(DATA_DIR "/output/auto_yosemite_left_right.png");

}

void testCatNYosemite(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.5f);
    pano.setHarrisTh(0.3f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    std::vector<PanoImage> pims;
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite1.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite2.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite3.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite4.jpg")));

    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_yosemite.png");

}

void testNHome(int start, int end){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);


    std::vector<PanoImage> pims;
    for (int n = start; n <= end; n++) {
        char buffer[255];
        sprintf(buffer, DATA_DIR "/input/home/IMG_%d.jpg", n);
        pims.push_back(PanoImage(FloatImage(buffer)));
    }

    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_home_lego_lin.png");

}

void testHome(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.3f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);


    PanoImage pim1(FloatImage(DATA_DIR "/input/home/IMG_5142.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/home/IMG_5143.jpg"));
    FloatImage autocat = pano.autocat2images(pim1, pim2, true);
    autocat.write(DATA_DIR "/output/auto_home_left_right_blend.png");

}


void testSphere(){
    SpherePano pano(400);
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    
    PanoImage pim1(FloatImage(DATA_DIR "/input/left.png"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/right.png"));
    FloatImage autocat = pano.autocat2imagesInSphere(pim1, pim2);
    autocat.write(DATA_DIR "/output/auto_sphere_left_right.png");
    
//    FloatImage autonos = pano.autocat2images(pim1, pim2);
//    autonos.write(DATA_DIR "/output/auto_normal_left_right.png");
}

void testNSphere(int start, int end){
    SpherePano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    
    
    std::vector<PanoImage> pims;
    for (int n = start; n <= end; n++) {
        char buffer[255];
        sprintf(buffer, DATA_DIR "/input/home/IMG_%d.jpg", n);
        pims.push_back(PanoImage(FloatImage(buffer)));
    }
    
    FloatImage autocat = pano.autocatnimagesSphere(pims, true, false);
    autocat.write(DATA_DIR "/output/auto_home_sphere_lego_lin.png");
    
}


void testWeightMap(){
    Pano pano;
    FloatImage im(DATA_DIR "/input/yosemite1.jpg");
    FloatImage weight = pano.calweight(im.sizeX(), im.sizeY());
    weight.write(DATA_DIR "/output/new_weight_map.png");
}

void testVizPatch(){
    PanoImage pim1(FloatImage(DATA_DIR "/input/building.png"));

    FloatImage detected = pim1.harrisCornerDetector(9, 0.6);
    //detected.write(DATA_DIR "/output/testFeature.png");
    pim1.calculatePatches(3.f, 15, false);
    FloatImage desp = pim1.vizPatches();
    desp.write(DATA_DIR "/output/testVizPatch.png");

}

