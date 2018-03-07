//
//  common.cpp
//  Yangtze
//
//  Created by DENG XI on 3/4/18.
//
//

#include "common.h"
/*
 CS 89/189 Computational Aspects of Digital Photography C++ basecode.
 
 Adapted from MIT's 6.815/6.865 basecode, written and designed by:
 Frédo Durand
 Katherine L. Bouman
 Gaurav Chaurasia
 Adrian Vasile Dalca
 Neal Wadhwa
 
 With additions & modifications by
 Wojciech Jarosz
 for Dartmouth's CS 89/189.
 */

#include <math.h>
#include "utils.h"
#include <assert.h>
#include <iostream>

using namespace std;

// The implementation of these functions will be given to you after the assignment 1 deadline
FloatImage brightness(const FloatImage &im, float factor)
{
    return im * factor;
}

FloatImage contrast(const FloatImage &im, float factor, float midpoint)
{
    return (im - midpoint) * factor + midpoint;
}

FloatImage changeGamma(const FloatImage &im, float old_gamma, float new_gamma)
{
    FloatImage output(im.width(), im.height(), im.channels());
    // Figure out what power to take the values of im, to get the values of output
    float power = new_gamma / old_gamma;
    for (int i = 0; i < output.size(); ++i)
        output(i) = pow(im(i), power);
    return output;
}

// Change the exposure of the image. This is different than brightness because
// it means you multiply an image's intensity in the linear domain.
FloatImage exposure(const FloatImage &im, float factor)
{
    FloatImage output(im.width(), im.height(), im.channels());
    output = changeGamma(im, 1.0f / 2.2f, 1.0f);
    output = brightness(output, factor);
    output = changeGamma(output, 1.0f, 1.0f / 2.2f);
    return output;
}

FloatImage color2gray(const FloatImage &im, const vector<float> &weights)
{
    if ((int) weights.size() != im.channels())
        throw MismatchedDimensionsException();
    
    FloatImage output(im.width(), im.height(), 1);
    // Convert to grayscale
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            float dot_product = 0.0f;
            for (int z = 0; z < im.channels(); ++z)
                dot_product += weights[z] * im(x, y, z);
            
            output(x, y, 0) = dot_product;
        }
    
    return output;
}

// For this function, we want two outputs, a single channel luminance image
// and a three channel chrominance image. Return them in a vector with luminance first
vector<FloatImage> lumiChromi(const FloatImage &im)
{
    FloatImage im_chrominance(im.width(), im.height(), im.channels());
    FloatImage im_luminance(im.width(), im.height(), 1);
    vector<FloatImage> output;
    
    // Create luminance and chrominance images
    im_luminance = color2gray(im);
    im_chrominance = im / (im_luminance + 1e-12f);
    
    // push the luminance and chrominance images onto the vector
    output.push_back(im_luminance);
    output.push_back(im_chrominance);
    return output;
}

// go from a luminance/chrominance images back to an rgb image
FloatImage lumiChromi2rgb(const FloatImage &lumi, const FloatImage &chromi)
{
    return chromi * lumi;
}

// Modify brightness then contrast
FloatImage brightnessContrastLumi(const FloatImage &im, float brightF, float contrastF, float midpoint)
{
    // Create output image of appropriate size
    FloatImage output(im.width(), im.height(), im.channels());
    
    // Modify brightness, then contrast of luminance image
    vector<FloatImage> lc = lumiChromi(im);
    
    lc[0] = brightness(lc[0], brightF);
    lc[0] = contrast(lc[0], contrastF, midpoint);
    
    output = lc[1] * lc[0];
    return output;
}

FloatImage rgb2yuv(const FloatImage &im)
{
    // Create output image of appropriate size
    FloatImage output(im.width(), im.height(), im.channels());
    
    // Change colorspace
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            output(x, y, 0) = 0.299f * im(x, y, 0) + 0.587f * im(x, y, 1) + 0.114f * im(x, y, 2);
            output(x, y, 1) = -0.147f * im(x, y, 0) - 0.289f * im(x, y, 1) + 0.436f * im(x, y, 2);
            output(x, y, 2) = 0.615f * im(x, y, 0) - 0.515f * im(x, y, 1) - 0.100f * im(x, y, 2);
        }
    
    return output;
}

FloatImage yuv2rgb(const FloatImage &im)
{
    // Create output image of appropriate size
    FloatImage output(im.width(), im.height(), im.channels());
    
    // Change colorspace
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            output(x, y, 0) = 1.0f * im(x, y, 0) + 0.000f * im(x, y, 1) + 1.140f * im(x, y, 2);
            output(x, y, 1) = 1.0f * im(x, y, 0) - 0.395f * im(x, y, 1) - 0.581f * im(x, y, 2);
            output(x, y, 2) = 1.0f * im(x, y, 0) + 2.032f * im(x, y, 1) + 0.000f * im(x, y, 2);
        }
    
    return output;
}

FloatImage saturate(const FloatImage &im, float factor)
{
    // Create output image of appropriate size
    FloatImage output = rgb2yuv(im);
    
    // Saturate image
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            output(x, y, 1) *= factor;
            output(x, y, 2) *= factor;
        }
    
    output = yuv2rgb(output);
    return output;
}

// Return two images in a C++ vector
vector<FloatImage> spanish(const FloatImage &im)
{
    // Remember to create the output images and the output vector
    FloatImage neg_chrom(im.width(), im.height(), im.channels());
    FloatImage lum(im.width(), im.height(), im.channels());
    
    // Do all the required processing
    FloatImage yuv = rgb2yuv(im);
    
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            lum(x, y, 0) = yuv(x, y, 0);
            lum(x, y, 1) = yuv(x, y, 0);
            lum(x, y, 2) = yuv(x, y, 0);
            
            neg_chrom(x, y, 0) = 0.5f;
            neg_chrom(x, y, 1) = -yuv(x, y, 1);
            neg_chrom(x, y, 2) = -yuv(x, y, 2);
        }
    
    neg_chrom = yuv2rgb(neg_chrom);
    
    int mw = floor(im.width() / 2);
    int mh = floor(im.height() / 2);
    lum(mw, mh, 0) = 0.0;
    lum(mw, mh, 1) = 0.0;
    lum(mw, mh, 2) = 0.0;
    
    neg_chrom(mw, mh, 0) = 0.0;
    neg_chrom(mw, mh, 1) = 0.0;
    neg_chrom(mw, mh, 2) = 0.0;
    
    // Push the images onto the vector
    vector<FloatImage> output;
    output.push_back(neg_chrom);
    output.push_back(lum);
    
    // Return the vector
    return output;
}

// White balances an image using the gray world assumption
FloatImage grayworld(const FloatImage &im)
{
    // Your code goes here
    float average[3] = {0.0f, 0.0f, 0.0f};
    float size = 1.0f / (im.width() * im.height());
    
    // compute average color
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            average[0] += im(x, y, 0) * size;
            average[1] += im(x, y, 1) * size;
            average[2] += im(x, y, 2) * size;
        }
    
    FloatImage output = im;
    for (auto y : range(im.height()))
        for (auto x : range(im.width()))
        {
            output(x, y, 0) *= average[1] / average[0];
            output(x, y, 1) *= 1.0f; // average[1] / average[1];
            output(x, y, 2) *= average[1] / average[2];
        }
    
    return output;
}


// Histograms

// Stretches the pixel values of channel c of im so that the minimum value maps to 0,
// and the maximum value maps to 1
void autoLevels(FloatImage &im, int c)
{
    // stretch pixel values of channel c to fill 0..1 range
    // you may find FloatImage::min and FloatImage::max useful
}

// initialize a histogram with numBins bins from the c-th channel of im
Histogram::Histogram(const FloatImage &im, int c, int numBins) :
m_pdf(numBins, 0.0f), m_cdf(numBins, 0.0f)
{
    // populate m_pdf with histogram values
    
    // Grad/extra credit: populate m_cdf with running sum of m_pdf
}

// return the histogram bin that value falls within
int Histogram::inverseCDF(double value) const
{
    return 0; // CHANGEME
}

// Produce a numBins() x 100 x 3 image containing a visualization of the
// red, green and blue histogram pdfs
FloatImage visualizeRGBHistogram(const Histogram &histR,
                                 const Histogram &histG,
                                 const Histogram &histB)
{
    assert(histR.numBins() == histG.numBins() && histR.numBins() == histB.numBins());
    
    // create an image of appropriate size
    FloatImage im(histR.numBins(), 100, 3);
    
    // populate im with RGB histograms
    return im;
}

// Return a FloatImage which is the result of applying histogram equalization to im
FloatImage equalizeRGBHistograms(const FloatImage &im)
{
    int numLevels = 256;
    
    // create an image of appropriate size
    FloatImage output = im;
    
    // apply histogram equalization to each channel
    
    return output;
}

// Return a FloatImage which is the result of transfering the histogram of F2 onto the image data in F1
FloatImage matchRGBHistograms(const FloatImage &F1, const FloatImage &F2)
{
    int numBins = 256;
    
    FloatImage output = F1;
    
    // perform histogram matching
    
    return output;
}

// Normalize a luminance image by standard deviation
FloatImage normalizeBySD(const FloatImage &lumi){
    FloatImage output(lumi.sizeX(), lumi.sizeY(), 1);
    float sum = 0, mean, sd = 0;
    for (int i = 0; i < lumi.size(); ++i) {
        sum += lumi(i);
    }
    mean = sum / lumi.size();
    for (int i = 0; i < lumi.size(); ++i) {
        sd += std::powf((lumi(i) - mean), 2);
    }
    sd = std::sqrtf(sd / lumi.size());
    for (int i = 0; i < lumi.size(); ++i) {
        output(i) = lumi(i) / sd;
    }
    return output;
}


