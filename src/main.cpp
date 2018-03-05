//
//Created by Xi Deng on 2018.03.03
//
//
//

#include <iostream>
#include <Eigen/Geometry>
#include "floatimage.h"
#include "utils.h"

int main(){
    FloatImage im(DATA_DIR "/input/testread.jpeg");
    im.write(DATA_DIR "/output/testwrite.png");

    return 0;
}
