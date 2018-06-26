#include "SurveySim.h"

//  using the formula:
//      [a b c]
//  R = [d e f]
//      [g h i]
// 
//      [h-f]
//  u = [c-g]
//      [d-b]


//  compute vector corss-product
int GSL_CrossProduct( const gsl_vector* V,
                      const gsl_vector* W,
                      gsl_vector* P ){
    double v1 = gsl_vector_get(V,0);
    double v2 = gsl_vector_get(V,1);
    double v3 = gsl_vector_get(V,2);
    double w1 = gsl_vector_get(W,0);
    double w2 = gsl_vector_get(W,1);
    double w3 = gsl_vector_get(W,2);

    gsl_vector_set(P, 0, v2*w3-v3*w2);
    gsl_vector_set(P, 1,-v1*w3+v3*w1);
    gsl_vector_set(P, 2, v1*w2-v2*w1);

    return 0;
}

int Get_RotationAxis( double phi_old, double theta_old,
                      double phi_new, double theta_new,
                      double* axis ){
    int i,info = 0;
    double sin_phi_old = sin(phi_old);
    double cos_phi_old = cos(phi_old);
    double sin_theta_old = sin(theta_old);
    double cos_theta_old = cos(theta_old);

    double sin_phi_new = sin(phi_new);
    double cos_phi_new = cos(phi_new);
    double sin_theta_new = sin(theta_new);
    double cos_theta_new = cos(theta_new);

//  inverse rotation about z-axis
    double R3_[] = {cos_theta_old, sin_theta_old, 0.0,
                   -sin_theta_old, cos_theta_old, 0.0,
                    0.0,           0.0,           1.0};

//  inverse rotation about y-axis
    double R2_[] = {cos_phi_old, 0.0,-sin_phi_old,
                    0.0,         1.0, 0.0,
                    sin_phi_old, 0.0, cos_phi_old };

//  rotation about y-axis
    double R2[] = { cos_phi_new, 0.0, sin_phi_new,
                    0.0,         1.0, 0.0,
                   -sin_phi_new, 0.0, cos_phi_new };

//  rotation about z-axis
    double R3[] = { cos_theta_new,-sin_theta_new, 0.0,
                    sin_theta_new, cos_theta_new, 0.0,
                    0.0,           0.0,           1.0 };

    gsl_matrix_view MR3_ = gsl_matrix_view_array(R3_,3,3);
    gsl_matrix_view MR2_ = gsl_matrix_view_array(R2_,3,3);
    gsl_matrix_view MR2  = gsl_matrix_view_array(R2,3,3);
    gsl_matrix_view MR3  = gsl_matrix_view_array(R3,3,3);

    gsl_matrix* R = gsl_matrix_alloc(3,3);  // R stores the final rotation matrix
    gsl_matrix* R_temp1 = gsl_matrix_alloc(3,3);
    gsl_matrix* R_temp2 = gsl_matrix_alloc(3,3);

//  compute the rotation matrix
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, &MR3.matrix, &MR2.matrix, 0.0, R_temp1 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp1, &MR2_.matrix, 0.0, R_temp2 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp2, &MR3_.matrix, 0.0, R );

//  compute the rotation axis
    axis[0] = gsl_matrix_get(R,2,1) - gsl_matrix_get(R,1,2);
    axis[1] = gsl_matrix_get(R,0,2) - gsl_matrix_get(R,2,0);
    axis[2] = gsl_matrix_get(R,1,0) - gsl_matrix_get(R,0,1);

//  normalize the rotation axis
    double axis_norm = sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
    for( i=0; i<3; i++ )
        axis[i] /= axis_norm;

//  free allocated memory
    gsl_matrix_free(R_temp1);
    gsl_matrix_free(R_temp2);
    gsl_matrix_free(R);

    return info;
}

int Get_RotationAngle( double phi_old, double theta_old,
                       double phi_new, double theta_new,
                       double* angle_rot ){
    int info = 0;
    double sin_phi_old = sin(phi_old);
    double cos_phi_old = cos(phi_old);
    double sin_theta_old = sin(theta_old);
    double cos_theta_old = cos(theta_old);

    double sin_phi_new = sin(phi_new);
    double cos_phi_new = cos(phi_new);
    double sin_theta_new = sin(theta_new);
    double cos_theta_new = cos(theta_new);

//  inverse rotation about z-axis
    double R3_[] = {cos_theta_old, sin_theta_old, 0.0,
                   -sin_theta_old, cos_theta_old, 0.0,
                    0.0,           0.0,           1.0};

//  inverse rotation about y-axis
    double R2_[] = {cos_phi_old, 0.0,-sin_phi_old,
                    0.0,         1.0, 0.0,
                    sin_phi_old, 0.0, cos_phi_old };

//  rotation about y-axis
    double R2[] = { cos_phi_new, 0.0, sin_phi_new,
                    0.0,         1.0, 0.0,
                   -sin_phi_new, 0.0, cos_phi_new };

//  rotation about z-axis
    double R3[] = { cos_theta_new,-sin_theta_new, 0.0,
                    sin_theta_new, cos_theta_new, 0.0,
                    0.0,           0.0,           1.0 };

    gsl_matrix_view MR3_ = gsl_matrix_view_array(R3_,3,3);
    gsl_matrix_view MR2_ = gsl_matrix_view_array(R2_,3,3);
    gsl_matrix_view MR2  = gsl_matrix_view_array(R2,3,3);
    gsl_matrix_view MR3  = gsl_matrix_view_array(R3,3,3);

    gsl_matrix* R = gsl_matrix_alloc(3,3);  // R stores the final rotation matrix
    gsl_matrix* R_temp1 = gsl_matrix_alloc(3,3);
    gsl_matrix* R_temp2 = gsl_matrix_alloc(3,3);

//  compute the rotation matrix
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, &MR3.matrix, &MR2.matrix,  0.0, R_temp1 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp1,     &MR2_.matrix, 0.0, R_temp2 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp2,     &MR3_.matrix, 0.0, R );

//  compute the rotation angle
    double Trace_of_R = gsl_matrix_get(R,0,0) + gsl_matrix_get(R,1,1) + gsl_matrix_get(R,2,2);
    double cosval     = 0.5*(Trace_of_R-1.0);

    cosval = cosval >  1.0 ?  1.0 : cosval;
    cosval = cosval < -1.0 ? -1.0 : cosval;

    *angle_rot = acos(cosval);

//  free allocated memory
    gsl_matrix_free(R_temp1);
    gsl_matrix_free(R_temp2);
    gsl_matrix_free(R);

    return info;
}

int Get_RotationAngle_faster( double phi_old, double theta_old,
                              double phi_new, double theta_new,
                              double* angle_rot ){
    int info = 0;
    double sin_phi_old = sin(phi_old);
    double cos_phi_old = cos(phi_old);
    double sin_theta_old = sin(theta_old);
    double cos_theta_old = cos(theta_old);

    double sin_phi_new = sin(phi_new);
    double cos_phi_new = cos(phi_new);
    double sin_theta_new = sin(theta_new);
    double cos_theta_new = cos(theta_new);

//  compute the rotation angle
    double Trace_of_R = cos_theta_old*cos_phi_old*cos_phi_new*cos_theta_new
                      + sin_theta_old*sin_theta_new
                      + cos_theta_old*sin_phi_old*sin_phi_new*cos_theta_new
                      + sin_theta_old*cos_phi_old*cos_phi_new*sin_theta_new
                      + cos_theta_new*cos_theta_old
                      + sin_theta_old*sin_phi_old*sin_phi_new*sin_theta_new
                      + sin_phi_old*sin_phi_new
                      + cos_phi_old*cos_phi_new;

    double cosval     = 0.5*(Trace_of_R-1.0);

    cosval = cosval >  1.0 ?  1.0 : cosval;
    cosval = cosval < -1.0 ? -1.0 : cosval;

    *angle_rot = acos(cosval);

    return info;
}


int Get_RotationAngle_Axis( double phi_old, double theta_old,
                            double phi_new, double theta_new,
                            double* angle_rot,
                            double* axis ){
    int info = 0;
    double sin_phi_old = sin(phi_old);
    double cos_phi_old = cos(phi_old);
    double sin_theta_old = sin(theta_old);
    double cos_theta_old = cos(theta_old);

    double sin_phi_new = sin(phi_new);
    double cos_phi_new = cos(phi_new);
    double sin_theta_new = sin(theta_new);
    double cos_theta_new = cos(theta_new);

//  inverse rotation about z-axis
    double R3_[] = {cos_theta_old, sin_theta_old, 0.0,
                   -sin_theta_old, cos_theta_old, 0.0,
                    0.0,           0.0,           1.0};

//  inverse rotation about y-axis
    double R2_[] = {cos_phi_old, 0.0,-sin_phi_old,
                    0.0,         1.0, 0.0,
                    sin_phi_old, 0.0, cos_phi_old };

//  rotation about y-axis
    double R2[] = { cos_phi_new, 0.0, sin_phi_new,
                    0.0,         1.0, 0.0,
                   -sin_phi_new, 0.0, cos_phi_new };

//  rotation about z-axis
    double R3[] = { cos_theta_new,-sin_theta_new, 0.0,
                    sin_theta_new, cos_theta_new, 0.0,
                    0.0,           0.0,           1.0 };

    gsl_matrix_view MR3_ = gsl_matrix_view_array(R3_,3,3);
    gsl_matrix_view MR2_ = gsl_matrix_view_array(R2_,3,3);
    gsl_matrix_view MR2  = gsl_matrix_view_array(R2,3,3);
    gsl_matrix_view MR3  = gsl_matrix_view_array(R3,3,3);

    gsl_matrix* R = gsl_matrix_alloc(3,3);  // R stores the final rotation matrix
    gsl_matrix* R_temp1 = gsl_matrix_alloc(3,3);
    gsl_matrix* R_temp2 = gsl_matrix_alloc(3,3);

//  compute the rotation matrix
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, &MR3.matrix, &MR2.matrix,  0.0, R_temp1 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp1,     &MR2_.matrix, 0.0, R_temp2 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp2,     &MR3_.matrix, 0.0, R );

//  compute the rotation angle
    double Trace_of_R = gsl_matrix_get(R,0,0) + gsl_matrix_get(R,1,1) + gsl_matrix_get(R,2,2);
    double cosval     = 0.5*(Trace_of_R-1.0);

    cosval = cosval >  1.0 ?  1.0 : cosval;
    cosval = cosval < -1.0 ? -1.0 : cosval;

    *angle_rot = acos(cosval);

//  compute the rotation axis
    axis[0] = gsl_matrix_get(R,2,1) - gsl_matrix_get(R,1,2);
    axis[1] = gsl_matrix_get(R,0,2) - gsl_matrix_get(R,2,0);
    axis[2] = gsl_matrix_get(R,1,0) - gsl_matrix_get(R,0,1);

//  normalize the rotation axis
    int i;
    double axis_norm = sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
    for( i=0; i<3; i++ )
        axis[i] /= axis_norm;

//  free allocated memory
    gsl_matrix_free(R_temp1);
    gsl_matrix_free(R_temp2);
    gsl_matrix_free(R);

    return info;
}

//  =====================================================================
//  check the rotation matrix calculation
// int main(){

//     const gsl_rng_type* T = gsl_rng_default;
//     gsl_rng* r_seed = gsl_rng_alloc(T);
    
//     srand(time(NULL));
//     gsl_rng_set(r_seed,rand());

//     double phi1, theta1;
//     double phi2, theta2;
//     double angle;
//     double *axis = (double*)malloc(sizeof(double));
    
//     int cnt = 0;
//     int nrun = 1;
//     while ( cnt < nrun ){
//         phi1 = gsl_ran_flat(r_seed, 0.0, M_PI);
//         phi2 = gsl_ran_flat(r_seed, 0.0, M_PI);
//         theta1 = gsl_ran_flat(r_seed, 0.0, 2*M_PI);
//         theta2 = gsl_ran_flat(r_seed, 0.0, 2*M_PI);

//         printf("old direction: (%20.16f, %20.16f)\n", phi1*radian2degree, theta1*radian2degree);
//         printf("new direction: (%20.16f, %20.16f)\n", phi2*radian2degree, theta2*radian2degree);

//         Get_RotationAngle(phi1, theta1, phi2, theta2, &angle, axis);
//         printf("rotation angle: %20.16f\n", angle*radian2degree);

//         cnt++;
//     }

//     gsl_rng_free(r_seed);

//     free(axis);

//     return 0;
// }


