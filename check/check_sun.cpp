
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include <armadillo>

#define idx_sat_x 3
#define idx_sat_y 4
#define idx_sat_z 5

#define idx_sun_x 6
#define idx_sun_y 7
#define idx_sun_z 8

#define idx_zhang 18

#define R_Earth	6378.1366	


// using namespace std;

int check_thj( double Sat[3], double Sun[3], double& delta_angle ){
	double OS, OE, OS_OE;
	double theta0, theta1, theta2, theta3;
    
	OS_OE = Sat[0]*Sun[0] + Sat[1]*Sun[1] + Sat[2]*Sun[2];
	OS = sqrt(Sun[0]*Sun[0] + Sun[1]*Sun[1] + Sun[2]*Sun[2]);
	OE = sqrt(Sat[0]*Sat[0] + Sat[1]*Sat[1] + Sat[2]*Sat[2]);

	theta0 = acos(OS_OE / OS / OE);
	theta1 = acos(R_Earth / OS);
	theta2 = acos(R_Earth / OE);
	theta3 = (16.0 + 35.4*2.0)/60.;

    delta_angle = theta0 - (theta1+theta2+theta3);

	return int(theta0 < (theta1+theta2+theta3));
}


int check_zx( double Sat[3], double Sun[3], double& delta_angle ){

	double OS, OE, OS_OE;
    
	OS_OE = Sat[0]*Sun[0] + Sat[1]*Sun[1] + Sat[2]*Sun[2];
	OS = sqrt(Sun[0]*Sun[0] + Sun[1]*Sun[1] + Sun[2]*Sun[2]);
	OE = sqrt(Sat[0]*Sat[0] + Sat[1]*Sat[1] + Sat[2]*Sat[2]);
    
	double cosval = OS_OE/OS/OE;

	return int(cosval >= -0.3385737);
}


int main( int argc, char* argv[] ){

	if( argc < 2 ){
		std::cout << "usage: " << argv[0] << " simulation_result.txt\n";
		exit(0);
	}

//	simulation result filename
	std::string result_fname = std::string(argv[1]);

//	load the result
	arma::mat result;
	result.load(result_fname.c_str(), arma::raw_ascii);

	int counter = 0;
	
	std::ofstream outfile("delta_angle.txt");

	for( int i=0; i<result.n_rows; ++i ){
		double Sat[3] = {result(i,idx_sat_x), result(i,idx_sat_y), result(i,idx_sat_z)};
		double Sun[3] = {result(i,idx_sun_x), result(i,idx_sun_y), result(i,idx_sun_z)};
		
		double delta_angle;
		int tmp = check_thj(Sat, Sun, delta_angle);

		if( tmp != result(i,idx_zhang) ){
			++counter;
            outfile << delta_angle << std::endl;
		}
	}
	
	outfile.close();

	std::cout << " found total " << counter << " lines of bad results ....\n";
}
