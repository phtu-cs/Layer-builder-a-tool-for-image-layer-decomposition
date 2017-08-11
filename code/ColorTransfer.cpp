#include"ColorTransfer.h"
#include"ExtractPalette.h"
#include <Eigen\Dense>
#include<iostream>

using namespace Eigen;

NaiveTransfer::NaiveTransfer(){

}
NaiveTransfer::~NaiveTransfer(){

}

Vector3f NaiveTransfer::Offset(const Palette& porginal, const Palette& paltered){
	Vector3f temp_offset(0, 0, 0);
	for (int ip = 0; ip < porginal.PalettePack.size();ip++){
		temp_offset += (porginal.PalettePack[ip] - paltered.PalettePack[ip]);
	}
	return temp_offset;
}
void NaiveTransfer::Transfer(const Vector3f& temp_offset, 
								const Image& Img,Image & output){
	output = Img;
	for (int row = 0; row < Img.rows(); row++){
		for (int col = 0; col < Img.cols(); col++){
			output(row, col) = output(row, col) - temp_offset;
		//	std::cout << output(row, col) << std::endl;
		}
	}
}