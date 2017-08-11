#include"EnergyFunction.h"
#include"LLE.h"
#include<iostream>
#include"ExtractPalette.h"
#include <fstream>


const int NUM_SUPERPIXEL_NEIGHBORS = 30;
const int NUM_PIXEL_NEIGHBORS = 10;
void Normalize(const MatrixXf& m,MatrixXf& nm){


	float min = FLT_MAX; float max = -FLT_MAX;

	//find min and max 
	for (int i = 0; i < m.rows(); i++){
		for (int j = 0; j < m.cols(); j++){
			if (min >m(i, j)){
				min = m(i, j);
			}
			if (max < m(i, j)){
				max = m(i, j);
			}
		}
	}

	nm = MatrixXf::Zero(m.rows(), m.cols());
	//map into [0, 1]
	for (int i = 0; i < m.rows(); i++){
		for (int j = 0; j < m.cols(); j++){
			nm(i, j) = (m(i, j) + min) / (max - min);
		}
	}

}

EnergyFunction::EnergyFunction(float m, float r, float u, float e, int nl){
	lambda_m = m;
	lambda_r = r;
	lambda_u = u;
	lambda_e = e;
	num_layer = nl;
}
//void EnergyFunction::SetLambdaM(float m){
//	lambda_m = m;
//}
void EnergyFunction::GetMatrixM(const vector<Superpixel>& spixels){
	num_spixels = spixels.size();
	//still not tested 
	int num_spixel = spixels.size();
	LLE lle;
	FloatMat W;
	vector<LLE::LLEPoint> vec_llep;
	lle.SetNumNeighbors(NUM_SUPERPIXEL_NEIGHBORS);
	lle.FindNeighbors(spixels, vec_llep);
	lle.GetWeights(vec_llep, W);
//	cout << W << endl;

	M = MatrixXf::Zero(num_layer*num_spixel, num_layer*num_spixel);
	for (int i = 0; i < num_layer; i++){
		int start = i * num_spixel;
		int block_size = num_spixel;

	//	cout << M.block(start, start, block_size, block_size).rows();
	//	cout << " "<< W.rows();
		M.block(start, start, block_size, block_size) = W;

	//	cout << "nrow: "<< M.rows() << "ncol: " << M.cols() << endl;
		// W;

		//std::cout << W << std::endl;

	}

	//for (int i = 0; i < M.rows(); i++){
	//	for (int j = 0; j < M.cols(); j++){
	//		cout<< M(i, j) << ",";

	//	}
	//	cout << endl;
	//}
}
//
//void EnergyFunction::SetLambdaR(float r){
//	lambda_r = r;
//}
//
void EnergyFunction::GetMatrixR(const vector<Superpixel>& spixels, const Palette& p){

	//still not tested 
	int num_spixel = spixels.size();
	R = Eigen::MatrixXf::Zero(3 * num_spixel, num_spixel*num_layer);

	//Note here color channals are B, G, R
	for (int ic = 0; ic < 3; ic++){
		for (int il = 0; il < num_layer; il++){
			int r_start = ic*num_spixel;
			int block_size = num_spixel;
			int c_start = il*num_spixel;
			R.block(r_start, c_start, block_size, block_size) =
				MatrixXf::Identity(block_size, block_size)*p.PalettePack[il](ic);

			//		cout << p.PalettePack[il] << endl;
		}
	}
	//	cout.precision(2);
	//	cout.width(5);
	//	for (int i = 0; i < R.rows(); i++){
	//		for (int j = 0; j < R.cols(); j++){
	//			cout << R(i, j)<<", ";
	//
	//		}
	//		cout << endl;
	//	}
}
	
void EnergyFunction::SetLambdaU(float u){
	lambda_u = u;
}
void EnergyFunction::GetMatrixU(const vector<Superpixel>& spixels){

	int num_spixel = spixels.size();
	U = MatrixXf::Zero(num_spixel, num_spixel*num_layer);
	for (int i = 0; i < U.rows(); i++){
		for (int j = 0; j < U.cols(); j++){
			if ((j - i) % num_spixel == 0){
				U(i, j) = 1;
			}
		}
	}

	//for (int i = 0; i < U.rows(); i++){
	//	for (int j = 0; j < U.cols(); j++){
	//		cout << U(i, j) << ", ";
	//	}
	//	cout << endl;
	//}


}

//void EnergyFunction::SetLambdaE(float e){
//	lambda_e = e;
//}
//void EnergyFunction::GetMatrixE(const vector<Superpixel>&){
//	//not implement yet
//}

void EnergyFunction::GetMatrixB(const vector<Superpixel>& spixels){
	int num_spixel = spixels.size();
	B = MatrixXf::Zero(num_spixel * 3, 1);

	for (int i = 0; i < num_spixel; i++){

		B(i, 0) = spixels[i].color(0)/255.0;
		B(i + num_spixel, 0) = spixels[i].color(1)/255.0;
		B(i + 2 * num_spixel, 0) = spixels[i].color(2)/255.0;

	}

	//for (int i = 0; i < B.rows(); i++){
	//	for (int j = 0; j < B.cols(); j++){
	//		cout << B(i, j) << ", ";
	//	}
	//	cout << endl;
	//}
}
void EnergyFunction::GetMatrixLAndX(const vector<Superpixel>& spixels){
	//

	int num_spixel = spixels.size();
	L = MatrixXf::Zero(num_spixel * num_layer, 1);
	MatrixXf A = lambda_m *
		(MatrixXf::Identity(M.rows(), M.cols()) - M).transpose()*
		(MatrixXf::Identity(M.rows(), M.cols()) - M) + lambda_r * R.transpose()*R +
		lambda_u * U.transpose()*U;
	MatrixXf b = lambda_r * R.transpose() * B +
		lambda_u * U.transpose() * MatrixXf::Ones(num_spixel, 1);

	L = A.colPivHouseholderQr().solve(b);

	//for (int i = 0; i < L.rows(); i++){
	//	for (int j = 0; j < L.cols(); j++){
	//		cout << L(i, j) << ", ";
	//	}
	//	cout << endl;
	//}

	X = MatrixXf::Zero(num_layer*num_pixels, 1);



	show_X = MatrixXf::Zero(num_layer*num_pixels, 1);

	for (int i = 0; i < num_layer; i++){
		MatrixXf temp;

		 X.block(i*num_pixels, 0, num_pixels, 1) = Q *L.block(i*num_spixel, 0, num_spixel, 1);
		 Normalize(X.block(i*num_pixels, 0, num_pixels, 1), 
			 temp); 
		 show_X.block(i*num_pixels, 0, num_pixels, 1) = temp;
	}
	//ofstream outf;
	//outf.open("X.txt");
	//for (int i = 0; i < X.rows(); i++){
	//	for (int j = 0; j < X.cols(); j++){
	//		outf << X(i, j) << " ";
	//	}
	//	outf << endl;
	//}
	//outf.close();

	//outf.open("Q.txt");
	//for (int i = 0; i < Q.rows(); i++){
	//	for (int j = 0; j < Q.cols(); j++){
	//		outf << Q(i, j)<<" ";
	//	}
	//	outf << endl;
	//}
	//outf.close();

	//outf.open("L.txt");
	//for (int i = 0; i < L.rows(); i++){
	//	for (int j = 0; j < L.cols(); j++){
	//		outf << L(i, j) << " ";
	//	}
	//	outf << endl;
	//}
	//outf.close();

}
void EnergyFunction::GetMatrixQ(const cv::Mat& img, vector<Superpixel>& spixels){

	num_pixels = img.cols*img.rows;
	int num_spixel = spixels.size();
	LLE lle;
	vector<LLE::LLEPoint> vec_llep;
	lle.SetNumNeighborsPerPixel(NUM_PIXEL_NEIGHBORS);
	lle.FindNeighborsPerPixel(img, spixels, vec_llep);

	//int count = 0;
	//for (int row = 0; row < img.rows;row++){
	//	for (int col = 0; col < img.cols; col++){
	//		cout << "row: " << row << ", col: "<< col
	//			<< "_color: " << vec_llep[count].point.transpose() << endl;

	//		for (int j = 0; j < vec_llep[count].neighbors.size(); j++){
	//			cout << "index: " << vec_llep[count].neighbors_index[j] << endl;
	//		}
	//		
	//		count++;
	//	}
	//	
	//}

	lle.GetWeightsPerPixel(img, vec_llep, spixels, Q);
	//cout << Q << endl;
}

void EnergyFunction::ShowMatrixL(vector<cv::Mat>& layers, const Slic& slic, int rows, int cols){

	//cout << "num_spixel" << num_spixels << endl;
	layers = vector<cv::Mat>(num_layer);


	for (int nl = 0; nl < layers.size(); nl++){
		layers[nl] = cv::Mat::zeros(rows, cols, CV_32FC1);

		for (int i = 0; i < rows; i++){
			for (int j = 0; j < cols; j++){

				int index = slic.clusters[j][i];	
				layers[nl].at<float>(i,j) = L(num_spixels*nl + index, 0);
			}
		}

	}
}

void EnergyFunction::ShowMatrixX(vector<cv::Mat>& layers, const Slic& slic, int rows, int cols){

	int index_x = 0;
	layers = vector<cv::Mat>(num_layer);
	for (int nl = 0; nl < layers.size(); nl++){
		layers[nl] = cv::Mat::zeros(rows, cols, CV_32FC1);

		for (int i = 0; i < rows; i++){
			for (int j = 0; j < cols; j++){

				layers[nl].at<float>(i, j) = X(index_x, 0);
				index_x++;
			}
		}

	}

	//fstream outf;
	//outf.open("p.txt");

	//cout << "why no p.txt" << endl;
	//for (int i = 0; i < layers[1].rows; i++){
	//	for (int j = 0; j < layers[1].cols; j++){
	//		outf << layers[1].at<float>(i, j) << " ";

	//	//	cout << layers[1].at<float>(i, j);
	//	}
	//	outf << endl;
	//}
	//outf.close();
}