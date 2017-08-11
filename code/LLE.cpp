#include"LLE.h"
#include<algorithm>
#include<map>
#include<iostream>
#include<opencv2/opencv.hpp>
static float Distance(float x1, float y1, float x2, float y2){
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

LLE::LLE(){

}

LLE::~LLE(){

}

void LLE::FindNeighbors(const std::vector<Superpixel>& input,
	std::vector<LLEPoint>& output){


	/* Transfer values and index, which is important to weight matrix construction */
	int data_size = input.size();
	for (int i = 0; i < data_size; i++){
		LLEPoint temp;
		temp.point = input[i].color;
		temp.index = i;
		output.push_back(temp);
	}

	for (int i = 0; i < data_size; i++){

		/* find the K-nearest neighbors*/
		std::multimap<float, LLEPoint> dists;
		for (int j = 0; j < data_size; j++){
			if (i != j){
				dists.insert(std::make_pair(Distance(input[i].x, input[i].y, input[j].x, input[j].y), output[j]));
			}
		}
		//	std::sort(dists.begin(), dists.end());
		//std::cout<<dists.size();
		//for (std::multimap<float, Eigen::Vector3f>::iterator iter = dists.begin();
		//	iter != dists.end();
		//	++iter) {
		//	std::cout << "key" << iter->first<< " value: "<< iter->second.transpose() << std::endl;
		//}
		//std::cout << std::endl;

		auto it = dists.begin();
		for (int k = 0; k < num_neighbors; k++){

			output[i].neighbors.push_back(it->second.point);
			output[i].neighbors_index.push_back(it->second.index);
			it++;
			//insert K-nearest neighbors
		}
	}
}
void LLE::FindNeighborsPerPixel(const cv::Mat& img, const std::vector<Superpixel>& input,
	std::vector<LLEPoint>& output){

	/* Loop over all pixels */
	cv::Mat lookinto_image(img.rows, img.cols, CV_8UC3);

	for (int row = 0; row < img.rows; row++){
		for (int col = 0; col < img.cols; col++){
			LLEPoint temp;
			cv::Vec3b temp_color = img.at<cv::Vec3b>(row, col);
			temp.point = Vector3f(temp_color[0], temp_color[1], temp_color[2]);

			lookinto_image.at<cv::Vec3b>(row, col) =
				cv::Vec3b(temp_color[0], temp_color[1], temp_color[2]);

			std::multimap<float, int> dists;
			for (int i = 0; i < input.size(); i++){

				dists.insert(std::make_pair(Distance(col, row,
					input[i].x, input[i].y), i));
			}

			auto it = dists.begin();
			for (int k = 0; k < num_neighbors_pixel; k++){
				//insert K-nearest neighbors
				temp.neighbors.push_back(input[it->second].color);
				temp.neighbors_index.push_back(it->second);
				it++;

			}
			output.push_back(temp);
		}
	}

//	imwrite("test_img_pre.jpg", lookinto_image);

}
void LLE::GetWeights(std::vector<LLEPoint>& input, FloatMat& output) {
	/*
	Pseudocode from http://www.cs.nyu.edu/~roweis/lle/algorithm.html
	for i=1:N
	create matrix Z consisting of all neighbours of Xi [d]
	subtract Xi from every column of Z
	compute the local covariance C=Z'*Z [e]
	solve linear system C*w = 1 for w [f]
	set Wij=0 if j is not a neighbor of i
	set the remaining elements in the ith row of W equal to w/sum(w);
	end
	*/
	output = Eigen::MatrixXf::Zero(input.size(), input.size());

	for (int i = 0; i < input.size(); i++){
		int num_col = input[i].neighbors.size();
		
		

		// regularized tolerance
		float tol = 0;

		if (num_col > 3){
			tol = 0.001;
		}

		FloatMat temp_Z(3, num_col);
		//construct temp_Z
		for (int j = 0; j < num_col; j++){
			temp_Z.col(j) = input[i].neighbors[j]
							- input[i].point;
		}
		FloatMat C = temp_Z.transpose()*temp_Z;
		C = C + Eigen::MatrixXf::Identity(num_col, num_col) 
			* tol * C.trace();
		Eigen::MatrixXf w = 
			C.ldlt().solve(Eigen::MatrixXf::Ones(num_col, 1));

		w = w / w.sum();
		for (int k = 0; k < w.size(); k++){
			output(input[i].index, input[i].neighbors_index[k]) = w(k, 0);

		}
	}
}
void LLE::GetWeightsPerPixel(const cv::Mat& img,std::vector<LLEPoint>& inputp, std::vector<Superpixel>& inputs, FloatMat& output){

	int temp_rows = img.rows;
	int temp_cols = img.cols;
	cv::Mat lookinto_image(temp_rows, temp_cols, CV_8UC3);
	output = MatrixXf::Zero(inputp.size(), inputs.size());
	int pixel_index = 0;
	//33
	//209
	for (int row = 0; row < img.rows; row++){
		for (int col = 0; col < img.cols; col++){

			pixel_index = row * img.cols + col;
			int num_col = inputp[pixel_index].neighbors.size();

			float tol = 0;

			if (num_col > 3){
				tol = 0.001;
			}

			FloatMat temp_Z(3, num_col);

			LLEPoint temp_lookinto = inputp[pixel_index];

			lookinto_image.at<cv::Vec3b>(row, col) = 
				cv::Vec3b(temp_lookinto.point[0], temp_lookinto.point[1],
				temp_lookinto.point[2]);

			//cout << "color: "<<temp_lookinto.point.transpose() << endl;
			//for (int i = 0; i < temp_lookinto.neighbors_index.size(); i++){
			//	cout << temp_lookinto.neighbors_index[i] << " ";
			//}

			//cout << endl;

			//cout << "point:" << inputp[pixel_index].point << endl;

			for (int j = 0; j < num_col; j++){
				temp_Z.col(j) = inputp[pixel_index].neighbors[j] -
					inputp[pixel_index].point;
			}
	/*		cout << "Z: "<< temp_Z << endl;*/

			FloatMat C = temp_Z.transpose()*temp_Z;
			C = C + Eigen::MatrixXf::Identity(num_col, num_col)
				* tol * C.trace();
			Eigen::MatrixXf w =
				C.ldlt().solve(Eigen::MatrixXf::Ones(num_col, 1));

			w = w / w.sum();
			for (int k = 0; k < w.size(); k++){
				output(pixel_index, inputp[pixel_index].neighbors_index[k]) = w(k, 0);
			}

			//cout << "C: "<< C << endl;
			//cout << "w: "<< w << endl;

			pixel_index++;
		}

	}
	
//	cout << output;
	//cv::imshow("test",lookinto_image);
	//imwrite("test_img.jpg", lookinto_image);

}
void LLE::SetNumNeighbors(int num){
	num_neighbors = num;
}
void LLE::SetNumNeighborsPerPixel(int num){
	num_neighbors_pixel = num;
}