#include<opencv2/opencv.hpp>
#include<Eigen/Dense>

using namespace cv;
using namespace Eigen;

//3 channals
//Normalized into [0,1]
void Mat2Eig(const Mat& m, Matrix<Vector3f, Dynamic, Dynamic>& eig){

	auto ppixel = m.data;
	if (m.channels() == 3){

		for (int row = 0; row < m.rows; row++){
			for (int col = 0; col < m.cols; col++){

				eig(row, col)[0] = ppixel[row*m.cols*3 + col*3 + 0]; // B
				eig(row, col)[1] = ppixel[row*m.cols*3 + col*3 + 1]; // G
				eig(row, col)[2] = ppixel[row*m.cols*3 + col*3 + 2]; // R
			}
		}
	}
	else{
		std::cerr << "Invalid Channels" << std::endl;
	}
}