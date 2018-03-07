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

int main(){
    
    //test generate a random float number in (0-1)
    pcg32 rng;
    std::cout<<" "<<rng.nextFloat()<<endl;

    FloatImage im1(DATA_DIR "/input/left.png");
    FloatImage im2(DATA_DIR "/input/right.png");
    Pano pano;
    
    PanoImage pim(im1);
    FloatImage detected = pim.harrisCornerDetector(9, 0.02);
    detected.write(DATA_DIR "/output/leftRthreshold.png");

//
//    std::vector<Vec2f> ref1,ref2;
//    ref1.push_back(Vec2f(457,99));
//    ref1.push_back(Vec2f(451,417));
//    ref1.push_back(Vec2f(543,426));
//    ref1.push_back(Vec2f(545,95));
//
//    ref2.push_back(Vec2f(34,95));
//    ref2.push_back(Vec2f(38,429));
//    ref2.push_back(Vec2f(135,419));
//    ref2.push_back(Vec2f(124,99));
//    
//    FloatImage cat = pano.cat2images(im1, im2, ref1, ref2);
//    cat.write(DATA_DIR "/output/left_right.png");

    return 0;
}

//void pano(){
//    FloatImage left(DATA_DIR "/input/left.png");
//    FloatImage right(DATA_DIR "/input/right.png");
//
//
//    MatrixXf ma(9, 9);
//    VectorXf v(9);
//
//
//
//    vector<vector<int>> ref1;
//    ref1.push_back({457,99});
//    ref1.push_back({451,417});
//    ref1.push_back({543,426});
//    ref1.push_back({545,95});
//
//
//
//    vector<vector<int>> ref2;
//    ref2.push_back({34,95});
//    ref2.push_back({38,429});
//    ref2.push_back({135,419});
//    ref2.push_back({124,99});
//
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
//    MatrixXf H = ma.inverse() * v;
//    Matrix3f hm = Map<Matrix<float, 3, 3, RowMajor>>(H.data());
//    cout << H << endl;
//    cout << hm << endl;
//
//    Vector3f test;
//    Vector3f res;
//    Matrix3f hmi = hm.inverse();
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
