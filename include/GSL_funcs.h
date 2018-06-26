//  This header file is added by XYH @ 2018-05-09

#ifndef _GSL_FUNCS_H_
#define _GSL_FUNCS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//	GSL header files
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <gsl/gsl_integration.h>


#define _newline_ {printf("\n");}


//  some mathematical constants
#define E 2.718281828459045
//#define PI 3.14159365358979323846
#define PI  M_PI  // M_PI is taken from GSL
#define PI_180 (M_PI/180)

#define radian2degree (180*M_1_PI)
#define degree2radian (M_PI/180)

//  compute vector corss-product
int GSL_CrossProduct( const gsl_vector* V,
                      const gsl_vector* W,
                      gsl_vector* P );

int Get_RotationAxis( double phi_old, double theta_old,
                      double phi_new, double theta_new,
                      double* axis );

//  The obtained angle is in unit of radian, NOT degree!!!
int Get_RotationAngle( double phi_old, double theta_old,
                       double phi_new, double theta_new,
                       double* angle_rot );

int Get_RotationAngle_faster( double phi_old, double theta_old,
                              double phi_new, double theta_new,
                              double* angle_rot );

int Get_RotationAngle_Axis( double phi_old, double theta_old,
                            double phi_new, double theta_new,
                            double* angle_rot, double* axis );


int init_gsl_interp_accel();
int free_gsl_interp_accel();

int Test_OrbitFile( double t, int idx );
int Find_OrbitFile_Index( double t, int* idx );


double locateSat_GSL( double    t,
                      double    *result );
#endif
