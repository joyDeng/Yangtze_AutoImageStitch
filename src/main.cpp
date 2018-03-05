//
//Created by Xi Deng on 2018.03.03
//
//
//

#include <iostream>
#include <Eigen/Geometry>
#include "floatimage.h"
#include "utils.h"

using namespace std;

int main(){
    FloatImage im(DATA_DIR "/input/testread.jpeg");
    im.write(DATA_DIR "/output/testwrite.png");

    vector<vector<int>> ref1;
    ref1.push_back({351,224});
    ref1.push_back({301,384});
    ref1.push_back({388,397});
    ref1.push_back({385,227});

    vector<vector<int>> ref2;
    ref2.push_back({465,226});
    ref2.push_back({420,397});
    ref2.push_back({507,389});
    ref2.push_back({499,225});

    return 0;
}
