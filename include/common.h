//
//  Header.h
//  Yangtze
//
//  Created by DENG XI on 3/5/18.
//  reference: Nori basecode of typedef vector in eigen
//

#ifndef Header_h
#define Header_h

#include <Eigen/Core>
#include <Eigen/LU>
#include <vector>

///* Import some common Eigen types */
//using Eigen::Vector2f;
//using Eigen::Vector3f;
//using Eigen::Vector4f;
//using Eigen::Vector2i;
//using Eigen::Vector3i;
//using Eigen::Vector4i;
//using Eigen::Matrix3f;
//using Eigen::Matrix4f;
//using Eigen::VectorXf;
//using Eigen::MatrixXf;
//using Eigen::JacobiSVD;


template <typename Scalar, int Dimension>  struct TVector;
typedef Eigen::Matrix<float,    Eigen::Dynamic, Eigen::Dynamic> MatrixXf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;

typedef TVector<float, 1>       Vec1f;
typedef TVector<float, 2>       Vec2f;
typedef TVector<float, 3>       Vec3f;
typedef TVector<float, 4>       Vec4f;
typedef TVector<double, 1>      Vec1d;
typedef TVector<double, 2>      Vec2d;
typedef TVector<double, 3>      Vec3d;
typedef TVector<double, 4>      Vec4d;
typedef TVector<int, 1>         Vec1i;
typedef TVector<int, 2>         Vec2i;
typedef TVector<int, 3>         Vec3i;
typedef TVector<int, 4>         Vec4i;
typedef Eigen::Matrix3f Mat3f;
typedef Eigen::Matrix4f Mat4f;
typedef Eigen::VectorXf Vecxf;
typedef Eigen::JacobiSVD<MatrixXf> SvdXf;

template <typename _Scalar, int _Dimension> struct TVector : public Eigen::Matrix<_Scalar, _Dimension, 1> {
public:
    enum {
        Dimension = _Dimension
    };
    
    typedef _Scalar                             Scalar;
    typedef Eigen::Matrix<Scalar, Dimension, 1> Base;
    typedef TVector<Scalar, Dimension>          VectorType;
    
    /// Create a new vector with constant component vlaues
    TVector(Scalar value = (Scalar) 0) { Base::setConstant(value); }
    
    /// Create a new 2D vector (type error if \c Dimension != 2)
    TVector(Scalar x, Scalar y) : Base(x, y) { }
    
    /// Create a new 3D vector (type error if \c Dimension != 3)
    TVector(Scalar x, Scalar y, Scalar z) : Base(x, y, z) { }
    
    /// Create a new 4D vector (type error if \c Dimension != 4)
    TVector(Scalar x, Scalar y, Scalar z, Scalar w) : Base(x, y, z, w) { }
    
    /// Construct a vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> TVector(const Eigen::MatrixBase<Derived>& p)
    : Base(p) { }
    
    /// Assign a vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> TVector &operator=(const Eigen::MatrixBase<Derived>& p) {
        this->Base::operator=(p);
        return *this;
    }
    
    /// Return a human-readable string summary
    std::string toString() const {
        std::string result;
        for (size_t i=0; i<Dimension; ++i) {
            result += std::to_string(this->coeff(i));
            if (i+1 < Dimension)
                result += ", ";
        }
        return "[" + result + "]";
    }
};




#endif /* Header_h */
