/*
Local linear Embedding (LLE)

Approximate a data point usng its K-nearest neighbors 

Solve for reconstruction weights
*/

#ifndef _LLE_
#define _LLE_
#include<vector>
#include<Eigen/Dense>
#include"Superpixel.h"

typedef Eigen::MatrixXf FloatMat;

class LLE{

public:
	
 	struct LLEPoint{

		std::vector<Eigen::Vector3f> neighbors;
		std::vector<int> neighbors_index;
		Eigen::Vector3f point;
		int index;
	};

	LLE();
	~LLE();

	void FindNeighbors(const std::vector<Superpixel>& input,
		std::vector<LLEPoint>& output);

	void FindNeighborsPerPixel(const cv::Mat& img,
		const std::vector<Superpixel>& input,
		std::vector<LLEPoint>& output);

	void GetWeights(std::vector<LLEPoint>&, FloatMat&);
	void GetWeightsPerPixel(const cv::Mat&,
							std::vector<LLEPoint>&,
							std::vector<Superpixel>&, 
							FloatMat&);


//	void GetWeightMatrix(const std::vector<LLEPoint>&, FloatMat&);
	void SetNumNeighbors(int num);
	void SetNumNeighborsPerPixel(int num);
protected:
	int num_neighbors;
	int num_neighbors_pixel;

};

#endif