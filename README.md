# CS189 Computational Photography Final Project
## Group 6: Xi Deng, Zhenli WU

Yangtze is a Auto Panorama Image Stitch Project developed by Deng,Xi  and Wu, Zhenli.

## What we have:
We used Eigen for linear algebra computations. We also adapted the FloatImage Class  from computational photography assignment base code to read and write images.
Some basic image processing code from our previous assignment are merged into this project, including computing gradient, separate intensity from colors, gaussian blur and some methods in common.cpp.
### References:
1. MIT slides: http://stellar.mit.edu/S/course/6/sp15/6.815/materials.html?showKind=lectureNotes
2. Papers: ”Recognising Panoramas” and ”Automatic Panoramic Image Stitching using Invariant Features” by M. Brown and D. G. Lowe. 



## What we implemented:

### Main Functionalities:
1. Stitch two images with user inputs.
2. Auto Stitch two images.
3. Auto Stitch n images both within Plane Projection Space and Sphere Projection Space.
4. 360 degree Panorama.
// details methods.
5. Add Smooth Blending for stitching both within Plane Projection Space and Sphere Projection Space.
6. Add Two Scale Blending for stitching in Plane Projection Space.
7. Implemented Harris Corner detector and patch descriptor to detect and describe feature points.
8. Implemented RANSAC method to select best Homography.
9. Allowed Straightening of Sphere surface projection image.
10. Use SVD to solve Homography.

### Details:
To store images with their feature points and descriptor patches, and to operate feature detection and discription individually within each image, we defined a `PanoImage` class:
It should be initialized as follow:
		`PanoImage(FloatImage x)`

Pano is an abstract class who has two children class which are `PlanePano` and `SpherePano`:
#### PanoImage Class:
PanoImage Class contain two main functionalities:
1. Feature detector:
This function implemented harris corner detector algorithm to detect points that located at corner content.
`FloatImage harrisCornerDetector(int window, float threshold);`
 2. Feature descriptor:
 This function calculates and stores local patches of feature points.
 	```c++
    void calculatePatches(float sigma = 2.f, int size = 9, bool blur = true, bool norm = true);
 	```
#### Pano Class:

We mainly implemented Pano class which express a type of panorama image projection.
Pano has following functionalities(APIs) that could be call directly in main:
1. manually cat 2 images: 
takes two images and user selected feature point as input and output stitched version of im1 and im2:
	```c++
	FloatImage mancat2images(const FloatImage &im1, const FloatImage &im2, 	std::vector<std::vector<Vec2f>> pairs);
	```
2. automatically cat 2 images:
	```c++
	FloatImage autocat2images(PanoImage &pim1, PanoImage &pim2, bool blend=true);
	``` 
	takes two images as input automatically detect the feature, match the feather points, calculate homography and output stitched version of im1 and im2
3. automatically cat n images
	```c++
	FloatImage autocatnimages(std::vector<PanoImage> &pims, bool center=true, bool blend=true, bool twoscale=true);
	```
	takes n ordered images as input, the reference image could be either the first one or the centered one which decided by the parameter center. This function calculate the homographies that transform each image to the space of reference image, and stitches them as one image. The parameter `blend` could control whether it blends the color at image bounds, and the parameter `twoscale` refer to the type of blending that it use.

We also implemented some wraped functionalities in Pano Class:
1. Match feature points from of images:
	```c++
    std::vector<std::vector<Vec2i>> matchDescriptors(PanoImage &pim1, PanoImage &pim2, float threshold=0.5);
    ```
	takes two PanoImages as input, match their patches and output as pairs.
2. RANSAC: evaluate homography of two images.
	```c++
    Mat3f RANSAC( PanoImage &pim1, PanoImage &pim2, float match_th = 0.5, float portion = 0.5, float accuracy = 0.1, float threshold = 1);
    ```
	takes two PanoImage as input, call matchDescriptors to get matched pairs. Then uniformly select 4 pairs to calculate homography, evaluate the homography by keeping largest inliners set, run this multiple round until the failure probability deduce to less than the parameter accuracy bound. Then recompute the homography with the best inliners and return.

#### PlanePano:
This is the class that operate plane space projection stitching.

1. Functions to stitch two images with and without smooth blending:
    ```c++
    FloatImage PlanePano::cat2images(const FloatImage &im1, const FloatImage &im2, Mat3f homo);
    FloatImage PlanePano::cat2imageBlend(const FloatImage &im1, const FloatImage &im2, Mat3f homo);
    ```

2. Functions to stitch n images without blending/ with smooth blending/ with two-scale blending:
	```c++
    FloatImage PlanePano::catnimages(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos);
    FloatImage PlanePano::catnimagesBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos);
    FloatImage PlanePano::catnimagesTwoScaleBlend(FloatImage ref, std::vector<FloatImage> ims, std::vector<Mat3f> homos, float sigma, bool lin);
    ```
3. Function to stitch two images based on user inputs:
	```c++
    FloatImage PlanePano::mancat2images(const FloatImage &im1, const FloatImage &im2, std::vector<std::vector<Vec2f>> pairs);
    ```
4. Functions to calculate canvas size for auto panorama:
	```c++
    Canvas PlanePano::calculateCanvas(ImageBound a, ImageBound b);
	Canvas PlanePano::calculateCanvas(std::vector<ImageBound> bs);
    ```

#### SpherePano:
This is the class that handle sphere space projection stitching.

```c++
FloatImage SpherePano::catnimagesBlend(FloatImage fref, vector<FloatImage> fims, vector<Mat3f> homos)
FloatImage SpherePano::catnimages(FloatImage fref, vector<FloatImage> fims, vector<Mat3f> homos)
```
This function basicly took reference image, ordered image sequence and coorisponding homograhies as input,  transfrom all the image into reference image space as image Planes, and trace ray over spherical directions from camera to see whether it intersects with image planes. If the ray intersect with plane, then project it onto sphere. It finally return spherecial space image as output.



#### Math:

```c++
Mat3f solveHomo(MatrixXf);
```
Solve Ax = 0 least square regression with SVD. called by computeHomo. Return Homography.


```c++
Mat3f computeHomo(std::vector<std::vector<Vec2f>> pairs);
```
Compute homography based on 2D position of pixel points. Return Homography.

```c++
bool Plane::intersect(Ray &ray, Intersect &it);
```
Return true if the ray vector intersect with plane, and update intersect point info to Intersect &it.
return false if there will be no intersection.
Ray line: p = ray.o + t * ray.dir;
Plane: _base, _normal
Intersection:
t = -(ray.o - _base).dot(_normal) / ray.dir.dot(_normal);
```c++
Vec3f computeY(vector<Mat3f> homos);
```
Return estimated Y axis that perpendicular to all the homos x plane.

#### Validation Functions:

To debug, we also implemented some vitualization tool for feature detection, discreption, pairs matching, and ray tracing.

