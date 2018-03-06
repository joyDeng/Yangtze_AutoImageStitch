//
//Created by Xi Deng on 2018.03.03
//
//
//

#include <iostream>
#include <Eigen/Geometry>
#include "common.h"
#include "floatimage.h"
#include "stiches.hpp"
#include "utils.h"

using namespace std;

int main(){
    FloatImage im1(DATA_DIR "/input/left.png");
    FloatImage im2(DATA_DIR "/input/right.png");
    Pano pano;


    vector<Vec2f> ref1,ref2;
    ref1.push_back(Vec2f(457,99));
    ref1.push_back(Vec2f(451,417));
    ref1.push_back(Vec2f(543,426));
    ref1.push_back(Vec2f(545,95));

    ref2.push_back(Vec2f(34,95));
    ref2.push_back(Vec2f(38,429));
    ref2.push_back(Vec2f(135,419));
    ref2.push_back(Vec2f(124,99));
    
    FloatImage cat = pano.cat2images(im1, im2, ref1, ref2);
    cat.write(DATA_DIR "/output/left_right.png");

    return 0;
}
