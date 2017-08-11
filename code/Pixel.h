#ifndef _PIXEL_
#define _PIXEL_
#include<Eigen/Dense>
using namespace Eigen;
struct Pixel{
public:
	Pixel();
	Pixel(float _x, float _y, const Vector3f& _c);
	float x;
	float y;
	Vector3f color;
};

#endif