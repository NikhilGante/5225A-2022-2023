#include "tracking.hpp"




double gA = 0, gX = 0, gY = 0;
double distance_lr = 13.75, distance_b = 7.5;

void tracking(){

  double left, right, back;

  double last_left = 0;
  double last_right = 0;
  double last_back = 0;

  double Θ = 0.0, β = 0.0;
  double radius_r, radius_b, h_lr, h_b;
  double lr_x, lr_y, b_y, b_x;


  double sin_alpha, cos_alpha, sin_beta; // locals to avoid duplicate computations

  while(true){
    left = (LeftEncoder.get_value()*TICKS_TO_INCHES)-last_left;
    right = (RightEncoder.get_value()*TICKS_TO_INCHES)-last_right;
    back = (BackEncoder.get_value()*TICKS_TO_INCHES)-last_back;


	

    Θ = (left-right)/distance_lr; // change in robot's angle
    if (Θ != 0){  // if the robot has travelled in an arc
    	radius_r = right/Θ;
    	radius_b = back/Θ;
    	β = Θ/2.0;

    	h_lr = (radius_r + distance_lr/2) * 2 * sin(β); // distance travelled by local y (right wheel) arc
    	h_b = (radius_b + distance_b) * 2 * sin(β); // distance travelled by local x (back wheel) arc
    }
    else{
      h_lr = right;
      h_b = back;
      β = 0.0;
    }
    // alpha = tracking.g_pos.a + beta;  // angle of local x and y vectors
    // sin_alpha = sin(alpha);
    // cos_alpha = cos(alpha);

    b_x = h_b * cos(gA+β); // global x movement detected by local x (back wheel) arc
    b_y = h_b * -sin(gA+β); // global y movement detected by local x (back wheel) arc
    lr_x = h_lr * sin(gA+β); // global x movement detected by local y (right wheel) arc
    lr_y = h_lr * cos(gA+β); // global y movement detected by local y (right wheel) arc

    gX += lr_x + b_x;
    gY += lr_y + b_y;
    gA += Θ;


	last_left = LeftEncoder.get_value()*TICKS_TO_INCHES;
    last_right = RightEncoder.get_value()*TICKS_TO_INCHES;
    last_back = BackEncoder.get_value()*TICKS_TO_INCHES;

    delay(10);
	}
}

void moveDrive(double x, double y, double a){
	front_l.move(x + y + a);
	front_r.move(-x + y - a);
	back_l.move(-x + y + a);
	back_r.move(x + y - a);
}

void xDriveMTT(double x, double y, double a, E_Brake_Modes brake_mode, double end_error){ 

	int min_power = 40;
	int max_power = 60;
	double globalXOffset = 0.0, globalYOffset = 0.0, globalAOffset = 0.0;
	double h = 0.0, β = 0.0, Θ = 0.0;
	double localX = 0.0, localY = 0.0, localA = 0.0;
	double xMov = 0.0, yMov = 0.0, aMov = 0.0;
	double scale_factor = 1;

	
	do{
		globalXOffset = x-gX;
		globalYOffset = y-gY;
		globalAOffset = a-(gA*180/M_PI);

		h = sqrt(globalXOffset*globalXOffset+globalYOffset*globalYOffset);
		β = atan2(globalXOffset, globalYOffset)*180/M_PI,  Θ = (β)+(0-(gA*180/M_PI));


		localX = sin(Θ*M_PI/180)*h, localY = cos(Θ*M_PI/180)*h, localA = globalAOffset;
	/*
		localX *= 2;
		localY *= 2;
		localA /= 4;

		if (fabs(localX)<min_power) localX = min_power*sgn(localX);
		if (fabs(localY)<min_power) localY = min_power*sgn(localY);
		if (fabs(localA)<min_power) localA = min_power*sgn(localA);

		screen::print(TEXT_MEDIUM, 1, "%.4f  %.4f  %.4f  %.4f  %.4f  %.4f",localX, localY, localA, h,  Θ, scale_factor);

		if (fabs(localX)+fabs(localY)+fabs(localA) > 90.0){
			scale_factor = (fabs(localX)+fabs(localY)+fabs(localA))/90.0;
		} else scale_factor = 1;

		localX /= scale_factor;
		localY /= scale_factor;
		localA /= scale_factor;


		moveDrive(localX, localY, localA);
	*/
		xMov = 2*localX;
		yMov = 2*localY;
		aMov = localA/4;

		if (fabs(xMov)+fabs(yMov)+fabs(aMov) > max_power){
			scale_factor = (fabs(xMov)+fabs(yMov)+fabs(aMov))/max_power;
		} else if (fabs(xMov)+fabs(yMov)+fabs(aMov) < min_power){
			scale_factor = (fabs(xMov)+fabs(yMov)+fabs(aMov))/min_power;
		} else scale_factor = 1;

		xMov /= scale_factor;
		yMov /= scale_factor;
		aMov /= scale_factor;


		moveDrive(xMov, yMov, aMov);

		std::cout << localX << ' ' << localY << ' ' << localA << ' ' << h << ' ' << Θ << ' ' <<  β << ' ' << gA << std::endl;
		std::cout << xMov << ' ' << yMov << ' ' << aMov << ' ' << scale_factor << std::endl;
		screen::print(TEXT_MEDIUM, 3, "%.4f  %.4f  %.4f  %.4f  %.4f  %.4f",localX, localY, localA, h,  Θ, scale_factor);
		screen::print(TEXT_MEDIUM, 4, "%.5f  %.5f  %.5f   ",gX, gY, (gA*180/M_PI));
		screen::print(TEXT_MEDIUM, 6, "%.5f  %.5f  %.5f   ",xMov, yMov, aMov);

		printf("X: %lf  , Y: %lf   , A: %lf      , LR: %lf  \n", gX, gY, gA*(180/M_PI), distance_lr);

		delay(10);

	} while (h>end_error || fabs(globalAOffset)>1);


	std::cout << "Motion Algorithm Stopped" << std::endl;
    switch(brake_mode){
    case E_Brake_Modes::none:
    	break;
    case E_Brake_Modes::coast:
      	moveDrive(0, 0, 0);
      	break;
    case E_Brake_Modes::brake:
		front_l.move_relative(0, 200);
		front_r.move_relative(0, 200);
		back_l.move_relative(0, 200);
		back_r.move_relative(0, 200);
    	break;
  	}


}

void xDriveTTA(double a, E_Brake_Modes brake_mode, double end_error){
	double offset, cur_angle, min_power = 15, max_power = 60;
	double aMov;

	do {
		cur_angle = fmod((gA*180/M_PI),360.0);
		offset = a-cur_angle;
		if (fabs(offset)>180) offset -= 360*sgn(offset);
		
		aMov = offset*2;
		if (fabs(aMov)<min_power) aMov = min_power*sgn(aMov);
		if (fabs(aMov)>max_power) aMov = max_power*sgn(aMov);

		screen::print(E_TEXT_MEDIUM, 3, "%.5f  %.5f  %.5f", aMov, offset, cur_angle);
		moveDrive(0, 0, aMov);
	} while (fabs(offset) > end_error);

	std::cout << "Motion Algorithm Stopped" << std::endl;
    switch(brake_mode){
    case E_Brake_Modes::none:
    	break;
    case E_Brake_Modes::coast:
      	moveDrive(0, 0, 0);
      	break;
    case E_Brake_Modes::brake:
		front_l.move_relative(0, 200);
		front_r.move_relative(0, 200);
		back_l.move_relative(0, 200);
		back_r.move_relative(0, 200);
    	break;
  	}
}

void xDriveTTT(double x, double y, E_Brake_Modes brake_mode, double end_error){
	double offset, cur_angle, min_power = 15, max_power = 60;
	double aMov;
	double a = atan2(x-gX, y-gY);

	do {

		cur_angle = fmod((gA*180/M_PI),360.0);
		offset = a-cur_angle;
		if (fabs(offset)>180) offset -= 360*sgn(offset);
		
		aMov = offset*2;
		if (fabs(aMov)<min_power) aMov = min_power*sgn(aMov);
		if (fabs(aMov)>max_power) aMov = max_power*sgn(aMov);

		screen::print(E_TEXT_MEDIUM, 3, "%.5f  %.5f  %.5f", aMov, offset, cur_angle);
		moveDrive(0, 0, aMov);
	} while (fabs(offset) > end_error);

	std::cout << "Motion Algorithm Stopped" << std::endl;
    switch(brake_mode){
    case E_Brake_Modes::none:
    	break;
    case E_Brake_Modes::coast:
      	moveDrive(0, 0, 0);
      	break;
    case E_Brake_Modes::brake:
		front_l.move_relative(0, 200);
		front_r.move_relative(0, 200);
		back_l.move_relative(0, 200);
		back_r.move_relative(0, 200);
    	break;
  	}
}


int sgn(double x){
	if (x > 0)return 1;
	else if (x < 0) return -1;
	else if (x==0) return 0;
	return 0;
}
