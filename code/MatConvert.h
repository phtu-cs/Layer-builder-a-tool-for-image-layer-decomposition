#ifndef _MATCONVERT_
#define _MATCONVERT_
#include<opencv2/opencv.hpp>
#include<Eigen\Dense>

using namespace Eigen;
using namespace cv;
void Mat2Eig(const Mat& m, Matrix<Vector3f, Dynamic, Dynamic>& eig);


#endif