#include"Pixel.h"
Pixel::Pixel(){
	x = 0;
	y = 0;
	color = Vector3f(0, 0, 0);
}
Pixel::Pixel(float _x, float _y, const Vector3f& _c){
	x = _x;
	y = _y;
	color = _c;
}