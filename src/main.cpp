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
void panof();

int main(){

//    int window = 9;
//    float harris_th = 0.3f, sigma = 2.f;
//
//    FloatImage im1(DATA_DIR "/input/left.png");
//    FloatImage im2(DATA_DIR "/input/right.png");

//    FloatImage im1(DATA_DIR "/input/yosemite1.jpg");
//    FloatImage im2(DATA_DIR "/input/yosemite2.jpg");

//    FloatImage im1(DATA_DIR "/input/pano.tga");
//    FloatImage im2(DATA_DIR "/input/pano1.tga");

//    FloatImage im1(DATA_DIR "/input/lily1.jpg");
//    FloatImage im2(DATA_DIR "/input/lily2.jpg");

//    FloatImage im1(DATA_DIR "/input/pano3.tga");
//    FloatImage im2(DATA_DIR "/input/pano2.tga");


    FloatImage im1(DATA_DIR "/input/left.png");
    FloatImage im2(DATA_DIR "/input/right.png");

    Pano pano;
    pano.setWindow(9);
    pano.setPatchWindow(21);
    pano.setMatchTh(0.6f);
    pano.setHarrisTh(0.3f);
    pano.setSigma(2.f);
    pano.setNorm(true);
    pano.setPortion(0.2f);
    
    PanoImage pim(im1);
//    FloatImage detected = pim.harrisCornerDetector(window, harris_th);
//    detected.write(DATA_DIR "/output/leftRthreshold.png");
//    pim.calculatePatches(sigma, 21);
//
    PanoImage pim2(im2);
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

    FloatImage autocat = pano.autocat2images(pim, pim2);
    autocat.write(DATA_DIR "/output/auto_table_left_right.png");

//    panof();


    return 0;

}

//void panof(){
//    FloatImage left(DATA_DIR "/input/table2.jpg");
//    FloatImage right(DATA_DIR "/input/table1.jpg");
//
//
//    Eigen::MatrixXf ma(9, 9);
//    Eigen::VectorXf v(9);
//
//
//
//
//
//
//
//    vector<vector<int>> ref1;
//    ref1.push_back({465,395});
//    ref1.push_back({381,384});
//    ref1.push_back({310,448});
//    ref1.push_back({435,482});
////
////
////
//    vector<vector<int>> ref2;
//    ref2.push_back({287,419});
//    ref2.push_back({211,408});
//    ref2.push_back({138,475});
//    ref2.push_back({262,499});
////
//    ma <<   ref2[0][0], ref2[0][1], 1, 0, 0, 0, -ref2[0][0] * ref1[0][0], -ref1[0][0] * ref2[0][1], -ref1[0][0],
//            0, 0, 0, ref2[0][0], ref2[0][1], 1, -ref2[0][0] * ref1[0][1], -ref1[0][1] * ref2[0][1], -ref1[0][1],
//            ref2[1][0], ref2[1][1], 1, 0, 0, 0, -ref2[1][0] * ref1[1][0], -ref1[1][0] * ref2[1][1], -ref1[1][0],
//            0, 0, 0, ref2[1][0], ref2[1][1], 1, -ref2[1][0] * ref1[1][1], -ref1[1][1] * ref2[1][1], -ref1[1][1],
//            ref2[2][0], ref2[2][1], 1, 0, 0, 0, -ref2[2][0] * ref1[2][0], -ref1[2][0] * ref2[2][1], -ref1[2][0],
//            0, 0, 0, ref2[2][0], ref2[2][1], 1, -ref2[2][0] * ref1[2][1], -ref1[2][1] * ref2[2][1], -ref1[2][1],
//            ref2[3][0], ref2[3][1], 1, 0, 0, 0, -ref2[3][0] * ref1[3][0], -ref1[3][0] * ref2[3][1], -ref1[3][0],
//            0, 0, 0, ref2[3][0], ref2[3][1], 1, -ref2[3][0] * ref1[3][1], -ref1[3][1] * ref2[3][1], -ref1[3][1],
//            0, 0, 0, 0, 0, 0, 0, 0, 1;
//
//    cout << ma << endl;
//    v << 0, 0, 0, 0, 0, 0, 0, 0, 1;
//
//    Eigen::MatrixXf H = ma.inverse() * v;
//    Eigen::Matrix3f hm = Eigen::Map<Eigen::Matrix<float, 3, 3, Eigen::RowMajor>>(H.data());
//    cout << H << endl;
//    cout << hm << endl;
//
//    Eigen::Vector3f test;
//    Eigen::Vector3f res;
//    Eigen::Matrix3f hmi = hm.inverse();
//
//    vector<vector<int>> ref3;
//
//
//    test << 0, 0, 1;
//    test = hm * test;
//    ref3.push_back({(int)ceilf(test(0)/test(2)), (int)ceilf(test(1)/test(2))});
////    cout << "LU: " << test(0)/test(2) << " "<< test(1)/test(2) << endl;
//    test << 0, 444, 1;
//    test = hm * test;
//    ref3.push_back({(int)ceilf(test(0)/test(2)), (int)ceilf(test(1)/test(2))});
////    cout << "LL: " << test(0)/test(2) << " "<< test(1)/test(2) << endl;
//
//    test << 588, 0, 1;
//    test = hm * test;
//    ref3.push_back({(int)ceilf(test(0)/test(2)), (int)ceilf(test(1)/test(2))});
////    cout << "RU: " << test(0)/test(2) << " "<< test(1)/test(2) << endl;
//    test << 588, 444, 1;
//    test = hm * test;
//    ref3.push_back({(int)ceilf(test(0)/test(2)), (int)ceilf(test(1)/test(2))});
////    cout << "RL: " << test(0)/test(2) << " "<< test(1)/test(2) << endl;
//
//    int lvb = min(0, min(ref3[0][0], ref3[1][0])), rvb = max(left.sizeX(), max(ref3[2][0], ref3[3][0]));
//    int uhb = min(0, min(ref3[0][1], ref3[2][1])), lhb = max(left.sizeY(), max(ref3[1][1], ref3[3][1]));
//
//    FloatImage output(rvb - lvb, lhb - uhb, right.channels());
//    for (int w = 0; w < output.sizeX(); ++w) {
//        for (int h = 0; h < output.sizeY(); ++h) {
//            for (int i = 0; i < output.channels(); ++i) {
//                test << w + lvb, h + uhb, 1;
//                res = hmi * test;
//                float r = right.smartAccessor((int) res(0)/res(2), (int) res(1)/res(2), i);
//                float hh = left.smartAccessor(w + lvb, h + uhb, i);
//                if(r != 0){
//                    output(w, h, i) = r;
//                }else{
//                    output(w, h, i) = hh;
//                }
//
//            }
//        }
//    }
//
//    output.debugWrite();
//
//}
