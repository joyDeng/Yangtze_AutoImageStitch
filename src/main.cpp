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


void testCatNYosemite(){
    PlanePano pano;
    std::vector<PanoImage> pims;
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite1.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite2.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite3.jpg")));
    pims.push_back(PanoImage(FloatImage(DATA_DIR "/input/yosemite4.jpg")));

    FloatImage autocat = pano.autocatnimages(pims);
    autocat.write(DATA_DIR "/output/auto_yosemite_two_scale_blending.png");

    autocat = pano.autocatnimages(pims, true, false);
    autocat.write(DATA_DIR "/output/auto_yosemite_no_blending.png");

    autocat = pano.autocatnimages(pims, true, true, false);
    autocat.write(DATA_DIR "/output/auto_yosemite_smooth_blending.png");
    

}


void testMatch(){
    PlanePano pano;

    PanoImage pim1(FloatImage(DATA_DIR "/input/home/IMG_5142.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/home/IMG_5143.jpg"));
    FloatImage matches = pano.vizMatches(pim1, pim2, pano.matchDescriptors(pim1, pim1, pano.m_match_th));

    matches.write(DATA_DIR "/output/viz_matches.png");

}


void testSphere(){
    SpherePano pano(0.1,4);
    pano.setWindow(9);
    pano.setPatchWindow(31);
    pano.setMatchTh(0.7f);
    pano.setHarrisTh(0.2f);
    pano.setSigma(3.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    
    PanoImage pim1(FloatImage(DATA_DIR "/input/checker.jpg"));
    PanoImage pim2(FloatImage(DATA_DIR "/input/checker.jpg"));
    FloatImage autocat = pano.autocat2images(pim1, pim2);
    autocat.write(DATA_DIR "/output/auto_sphere_left_right.png");
    
//    FloatImage autonos = pano.autocat2images(pim1, pim2);
//    autonos.write(DATA_DIR "/output/auto_normal_left_right.png");
}

void testNSphere(int start, int end){
    SpherePano pano(1, 4);
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
        sprintf(buffer, DATA_DIR "/input/tree/IMG_%d.JPG", n);
        pims.push_back(PanoImage(FloatImage(buffer)));
    }
    
//    FloatImage autocat = pano.autocatnimagesSphere(pims, true, true);
//    autocat.write(DATA_DIR "/output/auto_tree_sphere_b.png");
    FloatImage autocat_s = pano.autocatnimages(pims, true, false);
    autocat_s.write(DATA_DIR "/output/auto_tree_straight_sphere_nb.png");
    
}

void testNSphereGreen(int start, int end){
    SpherePano pano(1, 4);
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
    
    FloatImage autocat = pano.autocatnimages(pims, true, true);
    autocat.write(DATA_DIR "/output/auto_tree_sphere_b.png");
    FloatImage autocat_s = pano.autocatnimages(pims, true, false);
    autocat_s.write(DATA_DIR "/output/auto_greendata_straight_sphere_nb.png");
    
}


void testWeightMap(){
    PlanePano pano;
    FloatImage im(DATA_DIR "/input/yosemite1.jpg");
    FloatImage weight = pano.calweight(im.sizeX(), im.sizeY());
    weight.write(DATA_DIR "/output/viz_weight_map.png");
}

void testVizPatch(){
    FloatImage im(DATA_DIR "/input/building.png");
    PanoImage pim1(im);
    FloatImage detected = pim1.harrisCornerDetector(9, 0.6);
    detected.write(DATA_DIR "/output/viz_hcd_features.png");
    pim1.calculatePatches(3.f, 15, true);
    FloatImage desp = pim1.vizPatches();
    desp.write(DATA_DIR "/output/viz_patches.png");
}

void testNImage(int start, int end, char* folder, bool c=true, int cropX=50, int cropY=0){
    PlanePano pano;
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

void testNTower(int start, int end){
    SpherePano pano(0.7,4);

    
    std::vector<PanoImage> pims;
    for (int n = start; n <= end; n++) {
        char buffer[255];
        sprintf(buffer, DATA_DIR "/input/tower/IMG_%d.jpg", n);
        pims.push_back(PanoImage(FloatImage(buffer)));
    }
    
    FloatImage autocat = pano.autocatnimages(pims,true, true,false);
    autocat.write(DATA_DIR "/output/auto_tower_plane_lin.png");
}

int main(){
    // auto panorama for yosemite images (horizonal in sequence)
    // no blending version/ smooth blending version/ two-scale blending version
    // source image set from:
    // http://www.cs.cornell.edu/courses/cs6670/2011sp/projects/p2/project2.html
    // results:
    // auto_yosemite_two_scale_blending.png
    // auto_yosemite_smooth_blending.png
    // auto_yosemite_smooth_blending.png
    testCatNYosemite();


    // auto panorama for snow images
    // results:
    testNImage(5180, 5186, "snow", false);

    // auto panorama for the Green images
    // to reduce the effects of distortion
    // we cropped 150 pixels on both the left and right side of each image
    // results:
    testNImage(5001, 5006, "green", true, 150, 0);


    // auto panorama for baker tower (vertical in sequence)
    // results:
    testNImage(5332, 5335, "vertical", true, 0, 0);


    // auto panorama for baker tower (vertical and horizontal, but in sequence)
    // the result might not be very ideal due to the image quality
    // results:
    testXYImage({5306, 5314, 5321},{5308, 5316, 5323},"multi", true, 20);




    // results:
    //testNTower(5313, 5319);




    //    testNSphere(5181, 5186);

//     testNSphere(5000, 5017);
//     testNSphereGreen(5253, 5277);

//    testSphere();

    //testNSphere(5254, 5263);



    // validation functions
    // results:
    testVizPatch();
    testWeightMap();
    testMatch();








    return 0;

}


