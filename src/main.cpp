//
//Created by Xi Deng on 2018.03.03
//
//
//

#include <iostream>
#include <Eigen/Geometry>
#include "common.h"
#include "floatimage.h"

#include "stitches.hpp"
#include "PanoImage.hpp"
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

int main(){

//    char buffer[255];
//    for (int n = 0; n < 21; n++)
//    {
//        FloatImage im = imMorph[n];
//        int classIndex = n + 20 * studentNumber;
//
//        sprintf(buffer, DATA_DIR "/output/class-morph%03d.jpg", classIndex);
//
//        im.write(buffer);
//    }

//    int window = 9;
//    float harris_th = 0.3f, sigma = 2.f;
//
//    FloatImage im1(DATA_DIR "/input/right.png");
//    FloatImage im2(DATA_DIR "/input/left.png");

//    FloatImage im1(DATA_DIR "/input/yosemite1.jpg");
//    FloatImage im2(DATA_DIR "/input/yosemite2.jpg");

//    FloatImage im1(DATA_DIR "/input/pano.tga");
//    FloatImage im2(DATA_DIR "/input/pano1.tga");

//    FloatImage im1(DATA_DIR "/input/lily1.jpg");
//    FloatImage im2(DATA_DIR "/input/lily2.jpg");

//    FloatImage im1(DATA_DIR "/input/pano3.tga");
//    FloatImage im2(DATA_DIR "/input/pano2.tga");

//    std::vector<PanoImage> pims;
//    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite1.jpg")));
//    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite2.jpg")));
//    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite3.jpg")));
//    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite4.jpg")));

//<<<<<<< HEAD



//    Pano pano;
//    pano.setWindow(9);
//    pano.setPatchWindow(31);
//    pano.setMatchTh(0.8f);
//    pano.setHarrisTh(0.3f);
//    pano.setSigma(3.f);
//    pano.setNorm(true);
//    pano.setPortion(0.2f);
//>>>>>>> b93c624c05cdae70ba18afdb832bcc07a30ee569
//
//    Pano pano;
//    pano.setWindow(9);
//    pano.setPatchWindow(21);
//    pano.setMatchTh(0.6f);
//    pano.setHarrisTh(0.5f);
//    pano.setSigma(2.f);
//    pano.setNorm(true);
//    pano.setPortion(0.2f);
//=======
//    Pano pano;
//    pano.setWindow(9);
//    pano.setPatchWindow(31);
//    pano.setMatchTh(0.6f);
//    pano.setHarrisTh(0.3f);
//    pano.setSigma(2.f);
//    pano.setNorm(true);
//    pano.setPortion(0.2f);
//
//>>>>>>> 08e37563e0839e0d1818936dd328d70dad80605c
    
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


//    std::vector<std::vector<Vec2f>> pairs;
//    std::vector<Vec2f> ref1,ref2,ref3,ref4;
//    ref1.push_back(Vec2f(457,99));
//    ref2.push_back(Vec2f(451,417));
//    ref3.push_back(Vec2f(543,426));
//    ref4.push_back(Vec2f(545,95));
//
//    ref1.push_back(Vec2f(34,95));
//    ref2.push_back(Vec2f(38,429));
//    ref3.push_back(Vec2f(135,419));
//    ref4.push_back(Vec2f(124,99));
//
//    pairs.push_back(ref1);
//    pairs.push_back(ref2);
//    pairs.push_back(ref3);
//    pairs.push_back(ref4);



//    FloatImage cat = pano.mancat2images(im1, im2, pairs);
//    cat.write(DATA_DIR "/output/left_right.png");


//    FloatImage autocat = pano.autocat2images(pim, pim2);
//    autocat.write(DATA_DIR "/output/auto_table_left_right.png");
//    FloatImage autocat = pano.autocatnimages(pims);
//    autocat.write(DATA_DIR "/output/auto_yosemite.png");


    //testCatYosemite();
    //testCatNYosemite();
    //testCatTable();
//    testCatNPan();
//
//    testCatPan();
//    testHome();

    Pano pano;
    FloatImage ref(DATA_DIR "/input/lily1.jpg");
    FloatImage refWeight = pano.calweight(ref.sizeX(), ref.sizeY());
    refWeight.write(DATA_DIR "/output/weightmap.png");

    //testNHome(5148, 5155);

    return 0;

}

void testCatTable(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(false);
    pano.setPortion(0.1f);
    PanoImage pim1(FloatImage(DATA_DIR "/input/table1.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/table2.jpg"));
    //FloatImage autocat = pano.autocat2images(pim1, pim2);
    FloatImage autocat = pano.autocat2images(pim1, pim2, true);
    autocat.write(DATA_DIR "/output/auto_pano_left_right.png");

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

//<<<<<<< HEAD
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
    pano.setMatchTh(0.5f);
    pano.setHarrisTh(0.3f);
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
    autocat.write(DATA_DIR "/output/auto_home.png");

}

void testHome(){
    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.5f);
    pano.setHarrisTh(0.3f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);


    PanoImage pim1(FloatImage(DATA_DIR "/input/home/IMG_5142.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/home/IMG_5143.jpg"));
    FloatImage autocat = pano.autocat2images(pim1, pim2, true);
    autocat.write(DATA_DIR "/output/auto_home_left_right_blend.png");

}

