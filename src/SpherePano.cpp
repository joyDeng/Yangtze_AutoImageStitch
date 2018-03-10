//
//  SpherePano.cpp
//  Yangtze
//
//  Created by DENG XI on 3/8/18.
//
//


#include "spherePano.h"


//PLANEBOUND FUNCTIONS

PlaneBound::planebound(){
    btnLeftFront << INFINITY, INFINITY, INFINITY;
    celRightBack << -INFINITY, -INFINITY, -INFINITY;
}


void PlaneBound::growBound(Vec3f point){
    Vec3f v1,v2;
    v1 << min(point.x(), btnLeftFront.x()),
          min(point.y(), btnLeftFront.y()),
          min(point.z(), btnLeftFront.z());
    v2 << max(point.x(), celRightBack.x()),
          max(point.y(), celRightBack.y()),
          max(point.z(), celRightBack.z());
    btnLeftFront = v1;
    celRightBack = v2;
}

bool PlaneBound::inbound(Vec3f point){
    if( point.x() < btnLeftFront.x() || point.x() >= celRightBack.x()) return false;
    if( point.y() < btnLeftFront.y() || point.y() >= celRightBack.y()) return false;
    if( point.z() < btnLeftFront.z() || point.z() >= celRightBack.z()) return false;
    return true;
}

//PLANE FUNCTIONS

bool Plane::intersect(Ray &ray, Intersect &it){
    float t = -(ray.o - _base).dot(normal) / ray.dir.dot(normal);
    if(t < ray.mint || t - __FLT_EPSILON__ * 100 > ray.maxt) return false;
    else{
        it.p = ray.o + t * ray.dir;
        //caluv on the imageplane
        Vec3f d = it.p - _base;
        float u = d.dot(_edge0) / (_edge0.norm() * _edge0.norm());
        float v = d.dot(_edge1) / (_edge1.norm() * _edge1.norm());
        it.uv = Vec2f(u, v);
        ray.maxt = t;
        return true;
    }
}

Plane::plane(Vec3f pos0, Vec3f pos1, Vec3f pos2){
    _base = pos0;
    _edge0 = pos1 - pos0;
    _edge1 = pos2 - pos0;
    normal =  _edge0.cross(_edge1) / (_edge1.norm() * _edge0.norm());
}

//SPHEREPANO FUNCTIONS

SpherePano::SpherePano():Pano(){
    m_f = 1;//focal length
    m_res = 4;
    m_straight = false;
}

SpherePano::SpherePano(float f, int res, bool s):Pano(){
    m_f = f;
    m_res = res;
    m_straight = s;
}


FloatImage SpherePano::cat2images(const FloatImage &fref, const FloatImage &fim, Mat3f homo){
    Mat3f homo_i = homo.inverse();
    Mat3f S,K;
    float size = std::max(fref.sizeX(), fref.sizeY());
    K.row(0) << m_f,0,0;
    K.row(1) << 0, m_f,0;
    K.row(2) << 0, 0, 1;
    S.row(0) << size, 0, fref.sizeX() / 2;
    S.row(1) << 0,size, fref.sizeY() / 2;
    S.row(2) << 0,0,1;
    Mat3f SK = S * K;
    Mat3f SK_i = SK.inverse();
    
    //create plane in word space for each image
    //refimage:
    Vec3f rp0,rp1,rp2;
    rp0 = SK_i * Vec3f(0, 0, 1);
    rp1 = SK_i * Vec3f(fref.sizeX(),0, 1);
    rp2 = SK_i * Vec3f(0,fref.sizeY(), 1);
    Plane rplane(rp0,rp1,rp2);
//    cout<<"rplane: "<<rplane._base<<std::endl;
    
    //image:
    Vec3f ip0, ip1, ip2;
    ip0 =  homo * Vec3f(0,0,1);
    ip1 =  homo * Vec3f(fim.sizeX(),0,1);
    ip2 =  homo * Vec3f(0,fim.sizeY(),1);
    ip0 = SK_i * (ip0 / abs(ip0.z()));
    ip1 = SK_i * (ip1 / abs(ip1.z()));
    ip2 = SK_i * (ip2 / abs(ip2.z()));
    Plane iplane(ip0, ip1, ip2);
//    cout<<"iplane"<<iplane._base<<std::endl;
//    cout<<"iplane"<<ip1<<std::endl;
//    cout<<"iplane"<<ip2<<std::endl;
    
    Vec2i cansize = Vec2i(m_res * fref.sizeX(), m_res * fref.sizeY());
    FloatImage output(cansize.x(),cansize.y(),fref.channels());
    
    for( int i = 0 ; i < cansize.x() ; i++){
//        std::cout<<i<<" out of "<<cansize.x()<<std::endl;
        float langti = (float)i/(float)cansize.x() - 0.5;
        for(int j = 0 ; j < cansize.y() ; j++){
            Vec3f dir = square2UniformSphere(Vec2f(langti,(float)j/(float)cansize.y() - 0.5));
            
//            for(int c = 0 ; c < fim.channels() ; c++)
//                output(i,j,c) = fabsf(dir[c]);
            
//            // create a new ray for tracing
            Vec3f camerapoint = Vec3f(0,0,0);//Vec3f(fref.sizeX() / 2.0, fref.sizeY() / 2.0, -m_f);
            Ray ray{camerapoint, dir, 0, INFINITY};
            
            Intersect itr, iti;
//
            if(iplane.intersect(ray, iti)){
                Vec3f p2d = homo_i * SK * iti.p;
                Vec2i indexp = Vec2i(floor(p2d.x() / p2d.z()), floor(p2d.y() / p2d.z()));
                //Vec2i(floor(iti.uv.x() * fim.sizeX()), floor(iti.uv.y() * fim.sizeY()));
                if(indexp.x() >= 0 && indexp.x() < fim.sizeX() && indexp.y() >= 0 && indexp.y() < fim.sizeY()){
                    for(int c = 0 ; c < output.channels() ; c++)
                        output(i,j,c) = fim(indexp.x(), indexp.y(), c);
                }
            }
        }
    }
    return output;
}

Vec3f SpherePano::computeY(vector<Mat3f> homos){
    Mat3f X;
    X << 1,0,0,0,1,0,0,0,1;
    
    MatrixXf A;
    A.resize(homos.size() + 1, 3);
    A.row(0) << 1,0,0;
    
    for(int i = 0 ; i < (int) homos.size() ; i++){
        Vec3f xi = homos[i] * Vec3f(1,0,0);
        xi = xi.normalized();
        X += xi * xi.transpose();
        A.row(i + 1) << xi.x(),xi.y(),xi.z();
    }
//    Eigen::ColPivHouseholderQR<Mat3f> dec(X);
//    std::cout<<"X: "<<X<<" "<<std::endl;
    SvdXf svd(X, Eigen::ComputeFullU | Eigen::ComputeFullV);
//    SvdXf svdA(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
//    int index = svdA.matrixV().cols();
//    Vec3f u = svdA.matrixV().col(index-1).normalized();
//    std::cout<<"world normal: "<<u<<" "<<std::endl;
    Vec3f ret;
    ret = svd.matrixV().col(1).normalized();
    if(m_straight) return ret;
    else return Vec3f(0,1,0);
}
void SpherePano::setStraight(bool x){
    m_straight = x;
}

Mat3f SpherePano::computeRotationMatrix(Vec3f y, Vec3f u){
    Mat3f R;
    float costheta = y.dot(u);
    float sintheta = y.cross(u).norm() / (y.norm() * u.norm());
    R.row(0)<< 1,        0,         0;
    R.row(1)<< 0, costheta, -sintheta;
    R.row(2)<< 0, sintheta,  costheta;
    return R;
}

FloatImage SpherePano::catnimages(FloatImage fref, vector<FloatImage> fims, vector<Mat3f> homos){
    vector<Mat3f> homos_i;
    for(int i = 0 ; i < homos.size() ; i++)
        homos_i.push_back(homos[i].inverse());
                          
    Mat3f S,K;
    float size = std::max(fref.sizeX(), fref.sizeY());
    K.row(0) << m_f,0,0;
    K.row(1) << 0, m_f,0;
    K.row(2) << 0, 0, 1;
    S.row(0) << size, 0, fref.sizeX() / 2;
    S.row(1) << 0,size, fref.sizeY() / 2;
    S.row(2) << 0,0,1;
    Mat3f SK = S * K;
    Mat3f SK_i = SK.inverse();
    
    // straighting
    //compute sum of Xi Xi transpose
    Vec3f u = computeY(homos);
    //    std::cout<<u<<std::endl;
    Mat3f R = computeRotationMatrix(Vec3f(0,1,0), u);
    Mat3f R_i = R.inverse();
    
    std::cout<<"Rotation matrix R is "<<R<<" "<<std::endl;
    
    //create plane in word space for each image
    //refimage:
    Vec3f rp0,rp1,rp2;
    rp0 = SK_i * Vec3f(0, 0, 1);
    rp1 = SK_i * Vec3f(fref.sizeX(),0, 1);
    rp2 = SK_i * Vec3f(0,fref.sizeY(), 1);
    Plane rplane(rp0,rp1,rp2);
//    cout<<"rplane: "<<rplane._base<<std::endl;


    //image:
    vector<Plane> planes;
//    vector<Mat3f> Rotates_i;
    for(int i = 0 ; i < fims.size() ; i++){
        Vec3f ip0, ip1, ip2;
//        Vec3f yi = (homos[i] * Vec3f(0,1,0)).normalized();
//        Mat3f R = computeRotationMatrix(yi, u);
//        Rotates_i.push_back(R.inverse());
        
        ip0 =  homos[i] * Vec3f(0,0,1);
        ip1 =  homos[i] * Vec3f(fims[i].sizeX(),0,1);
        ip2 =  homos[i] * Vec3f(0,fims[i].sizeY(),1);
        ip0 = SK_i * ((ip0) / abs(ip0.z()));
        ip1 = SK_i * ((ip1) / abs(ip1.z()));
        ip2 = SK_i * ((ip2) / abs(ip2.z()));
        Plane iplane(ip0, ip1, ip2);
        planes.push_back(iplane);
    }
    
    Vec2i cansize = Vec2i(m_res * fref.sizeX(), m_res * fref.sizeY());
    FloatImage output(cansize.x(),cansize.y(),fref.channels());
    
    for( int i = 0 ; i < cansize.x() ; i++){
        //        std::cout<<i<<" out of "<<cansize.x()<<std::endl;
        float langti = 0.5 - (float)i/(float)cansize.x();
        for(int j = 0 ; j < cansize.y() ; j++){
            Vec3f dir = square2UniformSphere(Vec2f(langti,(float)j/(float)cansize.y() - 0.5));
            
//            for(int c = 0 ; c < fref.channels() ; c++)
//                output(i,j,c) = fabsf(dir[c]);
            
            // create a new ray for tracing
            Vec3f camerapoint = Vec3f(0,0,0);
            Ray ray{camerapoint, R_i * dir, 0, INFINITY};
            
            Intersect itr, iti;

            for(int t = 0 ; t < planes.size() ; t++){
                if(planes[t].intersect(ray, iti)){
                    Vec3f p2d = homos_i[t] * SK * iti.p;
                    Vec2i indexp = Vec2i(floor(p2d.x() / p2d.z()), floor(p2d.y() / p2d.z()));
                    if(indexp.x() >= 0 && indexp.x() < fims[t].sizeX() && indexp.y() >= 0 && indexp.y() < fims[t].sizeY()){
                        for(int c = 0 ; c < output.channels() ; c++)
                            output(i,j,c) = fims[t](indexp.x(), indexp.y(), c);
                    }
                }
            }
            
            if(rplane.intersect(ray, itr)){
                Vec3f p2d =  SK * itr.p;
                Vec2i indexp = Vec2i(floor(p2d.x()), floor(p2d.y()));
                if(indexp.x() >= 0 && indexp.x() < fref.sizeX() && indexp.y() >= 0 && indexp.y() < fref.sizeY()){
                    for(int c = 0 ; c < output.channels() ; c++)
                        output(i,j,c) = fref(indexp.x(), indexp.y(),c);
                }
            }
        }
    }
    return output;
}


FloatImage SpherePano::cat2imageBlend(const FloatImage &im1, const FloatImage &im2, Mat3f homo){
    return FloatImage(im1);
}

FloatImage SpherePano::catnimagesBlend(FloatImage fref, vector<FloatImage> fims, vector<Mat3f> homos){
    FloatImage rw = calweight(fref.sizeX(), fref.sizeY(),false);
    vector<FloatImage> weights;
    vector<Mat3f> homos_i;
    
    for(int i = 0 ; i < homos.size() ; i++)
        homos_i.push_back(homos[i].inverse());
    
    for(int i = 0 ; i < fims.size() ; i++)
        weights.push_back(calweight(fims[i].sizeX(), fims[i].sizeY(),false));
    
    Mat3f S,K;
    float size = std::max(fref.sizeX(), fref.sizeY());
    K.row(0) << m_f,0,0;
    K.row(1) << 0, m_f,0;
    K.row(2) << 0, 0, 1;
    S.row(0) << size, 0, fref.sizeX() / 2;
    S.row(1) << 0,size, fref.sizeY() / 2;
    S.row(2) << 0,0,1;
    Mat3f SK = S * K;
    Mat3f SK_i = SK.inverse();
    
    Vec3f u = computeY(homos);
    //    std::cout<<u<<std::endl;
    Mat3f R = computeRotationMatrix(Vec3f(0,1,0), u);
    Mat3f R_i = R.inverse();
    
    //create plane in word space for each image
    //refimage:
    Vec3f rp0,rp1,rp2;
    rp0 = SK_i * Vec3f(0, 0, 1);
    rp1 = SK_i * Vec3f(fref.sizeX(),0, 1);
    rp2 = SK_i * Vec3f(0,fref.sizeY(), 1);
    Plane rplane(rp0,rp1,rp2);
//    cout<<"rplane: "<<rplane._base<<std::endl;
    
    //image:
    vector<Plane> planes;
    for(int i = 0 ; i < fims.size() ; i++){
        Vec3f ip0, ip1, ip2;
        ip0 =  homos[i] * Vec3f(0,0,1);
        ip1 =  homos[i] * Vec3f(fims[i].sizeX(),0,1);
        ip2 =  homos[i] * Vec3f(0,fims[i].sizeY(),1);
        ip0 = SK_i * ((ip0) / abs(ip0.z()));
        ip1 = SK_i * ((ip1) / abs(ip1.z()));
        ip2 = SK_i * ((ip2) / abs(ip2.z()));
        Plane iplane(ip0, ip1, ip2);
        planes.push_back(iplane);
//        cout<<i<<"th iplane"<<iplane._base<<std::endl;
//        cout<<i<<"th iplane"<<ip1<<std::endl;
//        cout<<i<<"th iplane"<<ip2<<std::endl;
    }
    
    Vec2i cansize = Vec2i(m_res * fref.sizeX(), m_res * fref.sizeY());
    FloatImage output(cansize.x(),cansize.y(),fref.channels());
    FloatImage weight_sum(cansize.x(),cansize.y(),1);
    
    for( int i = 0 ; i < cansize.x() ; i++){
        //  std::cout<<i<<" out of "<<cansize.x()<<std::endl;
        float langti = 0.5 - (float)i/(float)cansize.x();
        for(int j = 0 ; j < cansize.y() ; j++){
            Vec3f dir = square2UniformSphere(Vec2f(langti,(float)j/(float)cansize.y() - 0.5));
            
//            for(int c = 0 ; c < fref.channels() ; c++)
//                output(i,j,c) = fabsf(dir[c]);
            
            // create a new ray for tracing
            Vec3f camerapoint = Vec3f(0,0,0);
            Ray ray{camerapoint, R_i * dir, 0, INFINITY};
            Intersect itr, iti;
            
            for(int t = 0 ; t < planes.size() ; t++){
                 ray.maxt = INFINITY;
                if(planes[t].intersect(ray, iti)){
                    Vec3f p2d = homos_i[t] * SK * iti.p;
                    Vec2i indexp = Vec2i(floor(p2d.x() / p2d.z()), floor(p2d.y() / p2d.z()));
                    if(indexp.x() >= 0 && indexp.x() < fims[t].sizeX() && indexp.y() >= 0 && indexp.y() < fims[t].sizeY()){
                        for(int c = 0 ; c < output.channels() ; c++)
                            output(i,j,c) += fims[t](indexp.x(), indexp.y(), c) * weights[t](indexp.x(), indexp.y(), 0);
                        weight_sum(i,j,0) += weights[t](indexp.x(), indexp.y(), 0);
                    }
                }
            }
//
            ray.maxt = INFINITY;
            if(rplane.intersect(ray, itr)){
                Vec3f p2d =  SK * itr.p;
                Vec2i indexp = Vec2i(floor(p2d.x()), floor(p2d.y()));
                if(indexp.x() >= 0 && indexp.x() < fref.sizeX() && indexp.y() >= 0 && indexp.y() < fref.sizeY()){
                    for(int c = 0 ; c < output.channels() ; c++)
                        output(i,j,c) += fref(indexp.x(), indexp.y(),c) * rw(indexp.x(), indexp.y(), 0);
                    weight_sum(i,j,0) += rw(indexp.x(), indexp.y(), 0);
                }
            }
            ray.maxt = INFINITY;
        }
    }
    
    for(int i = 0 ; i < output.sizeX() ; i++)
        for(int j = 0 ; j < output.sizeY() ; j++){
            float w = weight_sum(i,j,0);
            if(w > 0){
                for(int c = 0 ; c < output.channels() ; c++)
                    output(i,j,c) = output(i,j,c) / w;
            }
            
        }
    
    //weight_sum.debugWrite();
    //rw.debugWrite();
    
    return output;

}
FloatImage SpherePano::catnimagesTwoScaleBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos, float sigma, bool lin){
    return FloatImage(ref);
}











