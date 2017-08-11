#ifndef _ENERGY_FUNCTION_
#define _ENERGY_FUNCTION_
#include<Eigen/Dense>
#include"LLE.h"
#include<vector>
#include"Superpixel.h"
#include"ExtractPalette.h"
#include<opencv2\opencv.hpp>
using namespace Eigen;
using namespace std;
class EnergyFunction{

public:

	EnergyFunction(float m, float r, float u, float e, int nl);

	//Manifold consistency
	void SetLambdaM(float m);
	void GetMatrixM(const vector<Superpixel>&);

	void SetLambdaR(float r);
	void GetMatrixR(const vector<Superpixel>&, const Palette& p);

	void SetLambdaU(float u);
	void GetMatrixU(const vector<Superpixel>&);

	void SetLambdaE(float e);
	void GetMatrixE(const vector<Superpixel>&);

	void GetMatrixB(const vector<Superpixel>& spixels);
	void GetMatrixLAndX(const vector<Superpixel>& spixels);

	void GetMatrixQ(const cv::Mat& img, vector<Superpixel>& spixels);

	void ShowMatrixL(vector<cv::Mat>& layers, const Slic& slic, int rows, int cols);
	void ShowMatrixX(vector<cv::Mat>& layers, const Slic& slic, int rows, int cols);

	float lambda_m, lambda_r, lambda_u, lambda_e;
	int num_layer;
	int num_pixels;
	int num_spixels;

	//see Layer builder paper for detail
	FloatMat M; FloatMat R; FloatMat U; FloatMat E;
	FloatMat B; FloatMat L; FloatMat Q; FloatMat X;
	FloatMat show_X;
};


#endif