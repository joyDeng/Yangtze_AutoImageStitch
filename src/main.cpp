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
#include "PlanePano.h"
#include "utils.h"

//using namespace std;
//using namespace Eigen;



void testCatTable(){
    PlanePano pano;
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

void testCatNB(){
    PlanePano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.1f);
    PanoImage pim1(FloatImage(DATA_DIR "/input/left.png"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/right.png"));
    FloatImage autocat = pano.autocat2images(pim1, pim2, false);
    autocat.write(DATA_DIR "/output/auto_pano_no_blending.png");

}

void testCatLily(){
    PlanePano pano;
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
    PlanePano pano;
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
    PlanePano pano;
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
    PlanePano pano;
    pano.setWindow(9);
    pano.setPatchWindow(21);
    pano.setMatchTh(0.6f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.1f);
    PanoImage pim1(FloatImage(DATA_DIR "/input/yosemite3.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/yosemite4.jpg"));
    FloatImage autocat = pano.autocat2images(pim1, pim2);
    autocat.write(DATA_DIR "/output/auto_yosemite_left_right.png");
    pim1.vizPatches().debugWrite();
    pim2.vizPatches().debugWrite();

}

void testCatNYosemite(){
    PlanePano pano;
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
    PlanePano pano;
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
        sprintf(buffer, DATA_DIR "/input/xi/IMG_%d.jpg", n);
        pims.push_back(PanoImage(FloatImage(buffer)));
    }

    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_xi_lin.png");

}

void testHome(){
    PlanePano pano;
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
    SpherePano pano(2);
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
        sprintf(buffer, DATA_DIR "/input/greendata/IMG_%d.JPG", n);
        pims.push_back(PanoImage(FloatImage(buffer)));
    }
    
    FloatImage autocat = pano.autocatnimagesSphere(pims, true, false);
    autocat.write(DATA_DIR "/output/auto_green_sphere.png");
    
}


void testWeightMap(){
    PlanePano pano;
    FloatImage im(DATA_DIR "/input/yosemite1.jpg");
    FloatImage weight = pano.calweight(im.sizeX(), im.sizeY());
    weight.write(DATA_DIR "/output/new_weight_map.png");
}

void testVizPatch(){
    FloatImage im(DATA_DIR "/input/building.png");
//    PanoImage pim1(cropImage(im, 0, 150, im.sizeX() - 300, im.sizeY()));
    PanoImage pim1(im);
    FloatImage detected = pim1.harrisCornerDetector(9, 0.6);
    //detected.write(DATA_DIR "/output/testFeature.png");
    pim1.calculatePatches(3.f, 15, true);
    FloatImage desp = pim1.vizPatches();
    desp.write(DATA_DIR "/output/testVizPatch.png");

}


void testNHomeCropped(int start, int end){
    PlanePano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    std::vector<PanoImage> pims;
    int crop = 120;
    for (int n = start; n <= end; n++) {
        char buffer[255];
        sprintf(buffer, DATA_DIR "/input/greendata/IMG_%d.jpg", n);
        FloatImage ori = FloatImage(buffer);
        pims.push_back(PanoImage(cropImage(ori, crop, 0, ori.sizeY(), ori.sizeX() - 2 * crop)));
    }

    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_cropped_lin.png");
}

void testNImage(int start, int end, char* folder, bool c=true, int cropX=50, int cropY=0){
    PlanePano pano;
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
        sprintf(buffer, DATA_DIR "/input/%s/IMG_%d.jpg", folder, n);
        FloatImage ori = FloatImage(buffer);
        if(c){
            pims.push_back(PanoImage(cropImage(ori, cropX, cropY, ori.sizeX() - 2 * cropX, ori.sizeY() - 2 * cropY)));
        }else{
            pims.push_back(PanoImage(ori));
        }

    }
    char buffer2[255];
    sprintf(buffer2, DATA_DIR "/output/auto_%s_result.jpg", folder);
    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(buffer2);
}

void testXYImage(std::vector<int> start, std::vector<int> end, char* folder, bool c=true, int cropX=50, int cropY=0){
    assert(start.size() == end.size());

    PlanePano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    std::vector<std::vector<PanoImage>> pimsxy;
    for (int i = 0; i < start.size(); ++i) {
        std::vector<PanoImage> pims;
        for (int n = start[i]; n <= end[i]; n++) {
            char buffer[255];
            sprintf(buffer, DATA_DIR "/input/%s/IMG_%d.jpg", folder, n);
            FloatImage ori = FloatImage(buffer);
            if(c){
                pims.push_back(PanoImage(cropImage(ori, cropX, cropY, ori.sizeX() - 2 * cropX, ori.sizeY() - 2 * cropY)));
            }else{
                pims.push_back(PanoImage(ori));
            }

        }
        pimsxy.push_back(pims);
    }

    char buffer2[255];
    sprintf(buffer2, DATA_DIR "/output/auto_%s_result.jpg", folder);
    FloatImage autocat = pano.autocatnimages(pimsxy);
    autocat.write(buffer2);
}



int main(){


//    PanoImage pim(im1);
//    FloatImage detected = pim.harrisCornerDetector(window, harris_th);
//    detected.write(DATA_DIR "/output/leftRthreshold.png");
//    pim.calculatePatches(sigma, 21);
//
//    PanoImage pim2(im2);
//    FloatImage detected2 = pim2.harrisCornerDetector(window, harris_th);
//    detected2.write(DATA_DIR "/output/rightRthreshold.png");
//    pim2.calculatePatches(sigma, 21);

//
//    std::vector<std::vector<Vec2i>> matches = pano.matchDescriptors(pim, pim2, match_th);
//    std::cout << matches.size() << endl;
//    for (int i = 0; i < matches.size(); ++i) {
//        printf("Match: (%d, %d) to (%d, %d)\n", matches[i][0].x(), matches[i][0].y(), matches[i][1].x(), matches[i][0].y());
//    }
//
//    FloatImage matchesImage = pano.vizMatches(pim, pim2, matches);
//    matchesImage.write(DATA_DIR "/output/matchesImage.png");


//
    testCatYosemite();
   // testCatNYosemite();
    //testCatTable();
    //testCatNPan();
    //
    //testCatPan();
//    testHome();

//    testNHome(5180, 5186);
//    testWeightMap();
//    testVizPatch();
//    testCatTable();
//    testCatLily();
//    testNHome(5217, 5224);
//    testNHome(5200, 5204);
    //testNHomeCropped(5212, 5230);
//    testNImage(5100, 5101, "sxi", false, 0, 0);
//    testNSphere(5181, 5186);
    //testNSphere(5254, 5263);



    // code for title image
//    testNImage(5000, 5006, "green", true, 150, 0);

    // code for auto
    //testCatNB();
//    testVizPatch();
//    testXYImage({5306, 5314},{5308, 5316},"multi", false);
//    testXYImage({5306, 5314, 5321},{5308, 5316, 5323},"multi", true, 20);
//    testXYImage({5305, 5313, 5320},{5309, 5317, 5324},"multi", false);
//
//    testNImage(5314, 5316, "multi", false);
//    testNImage(5000, 5004, "tree", false);



    return 0;

}


