#ifndef _COLORTRANSFER_H_
#define _COLORTRANSFER_H_
#include <Eigen\Dense>
#include"ExtractPalette.h"
using namespace Eigen;
class NaiveTransfer{



public:
	typedef Eigen::Matrix<Vector3f, Dynamic, Dynamic> Image;

	NaiveTransfer();
	~NaiveTransfer();
	
	Vector3f Offset(const Palette& porginal, const Palette& paltered);
	void Transfer(const Vector3f& temp_offset, const Image& Img, Image & output);

protected:
	


};


#endif