#include "util.hpp"



int sgn(double x){
	if (x > 0)return 1;
	else if (x < 0) return -1;
	else if (x==0) return 0;
	return 0;
}
std::tuple<double, double, double> findCircle(double x1, double y1, double x2, double y2, double x3, double y3) {
	double x12 = x1 - x2, x13 = x1 - x3;
    double y12 = y1 - y2, y13 = y1 - y3;
	double y31 = y3 - y1, y21 = y2 - y1;
	double x31 = x3 - x1, x21 = x2 - x1;

	double sx13 = pow(x1, 2) - pow(x3, 2), sy13 = pow(y1, 2) - pow(y3, 2);
    double sx21 = pow(x2, 2) - pow(x1, 2), sy21 = pow(y2, 2) - pow(y1, 2);

	double f = ((sx13) * (x12)
			+ (sy13) * (x12)
			+ (sx21) * (x13)
			+ (sy21) * (x13))
			/ (2 * ((y31) * (x12) - (y21) * (x13)));
	double g = ((sx13) * (y12)
			+ (sy13) * (y12)
			+ (sx21) * (y13)
			+ (sy21) * (y13))
			/ (2 * ((x31) * (y12) - (x21) * (y13)));

	double c = -pow(x1, 2) - pow(y1, 2) - 2 * g * x1 - 2 * f * y1;

	double h = -g, k = -f;
	double sqr_of_r = h * h + k * k - c;
	double r = sqrt(sqr_of_r);

	return std::make_tuple(r, h, k);

}
std::pair<double, double> getPoint(double x1, double y1, double r, double a1){ // a has to be between -180 and 180
    double a2 = 90-a1;
    double y2 = sin(a2*M_PI/180)*r;
    double x2 = cos(a2*M_PI/180)*r;
    y2 += y1;
    x2 += x1;
    return std::make_pair(x2+0.0001, y2+0.0001);
}
