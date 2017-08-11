#ifndef _SUPERPIXEL_
#define _SUPERPIXEL_
#include<Eigen/Dense>
#include<vector>
#include"ExtractPalette.h"
#include"slic.h"
#include"Pixel.h"

using namespace Eigen;

/* Properties of a pixel and a superpixel */
class Superpixel{
public:
	Superpixel();
	float x;
	float y;
	Vector3f color;
	std::vector<Pixel> pixels;
	int pixel_count;
};
void ConvertFromSlic(const Slic& s, const cv::Mat& img, vector<Superpixel>& spixels);


#endif