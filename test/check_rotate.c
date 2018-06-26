#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#ifndef _error_
#define _error_ 1
#endif

#ifndef _pi_
#define _pi_ 3.1415926535
#endif

//  ========================================================================

//  compute vector corss-product
int CrossProduct( const gsl_vector* V,
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

//  generate a random unit vector
int GenRandomVector( gsl_rng*       r,
                     gsl_vector*    V,
                     double*        alpha,
                     double*        gamma ){
    *alpha = gsl_ran_flat(r, 0.0, _pi_);
    *gamma = gsl_ran_flat(r, 0.0, 2.0*_pi_);
    gsl_vector_set(V, 0, sin(*alpha)*cos(*gamma));
    gsl_vector_set(V, 1, sin(*alpha)*sin(*gamma));
    gsl_vector_set(V, 2, cos(*alpha));

    return 0;
}

//  rotate the random vector V by the random rotation matrix R
int RotateV( gsl_matrix* R,
             gsl_vector* V,
             gsl_vector* V_rot ){
    return gsl_blas_dgemv( CblasNoTrans, 1.0, R, V, 0.0, V_rot );
}

int FindRotationMatrix( double alpha_old, double gamma_old,
                        double alpha_new, double gamma_new,
                        gsl_matrix* R ){
    int info=0;

//  inverse rotation about z-axis
    double R3_[] = {cos(-gamma_old),-sin(-gamma_old), 0.0,
                    sin(-gamma_old), cos(-gamma_old), 0.0,
                    0.0,            0.0,            1.0 };

//  inverse rotation about y-axis
    double R2_[] = {cos(-alpha_old), 0.0, sin(-alpha_old),
                    0.0,            1.0, 0.0,
                   -sin(-alpha_old), 0.0, cos(-alpha_old) };

//  rotation about y-axis
    double R2[] = { cos(alpha_new), 0.0, sin(alpha_new),
                    0.0,            1.0, 0.0,
                   -sin(alpha_new), 0.0, cos(alpha_new) };

//  rotation about z-axis
    double R3[] = { cos(gamma_new),-sin(gamma_new), 0.0,
                    sin(gamma_new), cos(gamma_new), 0.0,
                    0.0,            0.0,            1.0 };

    gsl_matrix_view MR3_ = gsl_matrix_view_array(R3_,3,3);
    gsl_matrix_view MR2_ = gsl_matrix_view_array(R2_,3,3);
    gsl_matrix_view MR2  = gsl_matrix_view_array(R2,3,3);
    gsl_matrix_view MR3  = gsl_matrix_view_array(R3,3,3);

    gsl_matrix* R_temp1 = gsl_matrix_alloc(3,3);
    gsl_matrix* R_temp2 = gsl_matrix_alloc(3,3);

    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, &MR3.matrix, &MR2.matrix, 0.0, R_temp1 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp1, &MR2_.matrix, 0.0, R_temp2 );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, R_temp2, &MR3_.matrix, 0.0, R );

    gsl_matrix_free(R_temp1);
    gsl_matrix_free(R_temp2);

    return info;
}

int FindRotationAxis( gsl_matrix* R, gsl_vector* A ){
    int info=1;
    int i,j;

    gsl_matrix* Rtemp = gsl_matrix_alloc(3,3);
    gsl_matrix_memcpy( Rtemp, R );  //  make a copy of R

    gsl_vector_complex* eigen_vals = gsl_vector_complex_alloc(3);
    gsl_matrix_complex* eigen_vecs = gsl_matrix_complex_alloc(3,3);

    //  solve the eigen-value problem
    gsl_eigen_nonsymmv_workspace* w = gsl_eigen_nonsymmv_alloc(3);
    gsl_eigen_nonsymmv_params(1,w);
    gsl_eigen_nonsymmv(Rtemp,eigen_vals,eigen_vecs,w);

    for( i=0; i<3; i++ ){
        gsl_complex x = gsl_vector_complex_get( eigen_vals, i );
        if( fabs(GSL_IMAG(x)) <= 0.0 ){
            for( j=0; j<3; j++ ){
                gsl_complex y = gsl_matrix_complex_get(eigen_vecs, i, j);
                if( fabs(GSL_IMAG(y)) <= 0.0 ){
                    gsl_vector_set(A,j,GSL_REAL(y));
                }
                else{
                    info = 1;
                    break;
                }
            }
            info = 0;
            break;
        }
    }

    // normalize A
    double A_norm = gsl_blas_dnrm2(A);
    for( i=0; i<3; i++ )
        gsl_vector_set(A, i, gsl_vector_get(A,i)/A_norm);

    gsl_matrix_free(Rtemp);
    gsl_vector_complex_free(eigen_vals);
    gsl_matrix_complex_free(eigen_vecs);
    gsl_eigen_nonsymmv_free(w);

    return info;
}

//  V, W and axis are normalized unit vectors
double FindRotationAngle( gsl_vector* axis,
                          gsl_vector* V,
                          gsl_vector* W ){
    gsl_vector* A = gsl_vector_alloc(3);
    gsl_vector* B = gsl_vector_alloc(3);

    CrossProduct(V, axis, A);
    CrossProduct(W, axis, B);

    double cosvalue, angle;
    gsl_blas_ddot(A, B, &cosvalue);

    angle = acos(cosvalue) * 180 / _pi_;

    gsl_vector_free(A);
    gsl_vector_free(B);

    return angle;
}

//  rotate about a given axis by an angle of omega
//  R(n,w) = S(phi,theta) * R(e3,w) * S^{-1}(phi,theta)
/*int RotateAboutdAxis( gsl_vector*   axis,*/
/*                      double        omega,*/
/*                      gsl_matrix*   R ){*/
/*    */
/*}*/

//  be aware of the different convetions of(phi,theta) used in usual mathematics and astronomy
int RotateAboutdAxis( double phi,
                      double theta,
                      double w,
                      gsl_matrix* R ){
    int info;

    double S[] = {  cos(phi)*cos(theta), -sin(phi), cos(phi)*sin(theta),
                    sin(phi)*cos(theta),  cos(phi), sin(phi)*sin(theta),
                    -sin(theta),          0.0,      cos(theta) };
    double IS[]= {  cos(phi)*cos(theta), sin(phi)*cos(theta), -sin(theta),
                   -sin(phi),            cos(phi),             0.0,
                    cos(phi)*sin(theta), sin(phi)*sin(theta),  cos(theta) };

    double R3w[] = { cos(w), -sin(w), 0.0,
                     sin(w),  cos(w), 0.0,
                     0.0,     0.0,    1.0 };

    gsl_matrix_view MS  = gsl_matrix_view_array(S, 3, 3);
    gsl_matrix_view MIS = gsl_matrix_view_array(IS, 3, 3);
    gsl_matrix_view MR3w= gsl_matrix_view_array(R3w,3, 3);

    gsl_matrix* Rtemp = gsl_matrix_alloc(3,3);

    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, &MS.matrix, &MR3w.matrix, 0.0, Rtemp );
    info = gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, Rtemp, &MIS.matrix, 0.0, R );
    gsl_matrix_free(Rtemp);

    return info;
}

//  =====================================================================

int run_a_test( gsl_rng* r ){
    int i, info=1;
    double alpha_old, gamma_old;
    double alpha_new, gamma_new;

    gsl_vector* V_old = gsl_vector_alloc(3);
    gsl_vector* V_new = gsl_vector_alloc(3);
    gsl_vector* V_rot = gsl_vector_alloc(3);

//  generate two random directions
    GenRandomVector(r, V_old, &alpha_old, &gamma_old);
    GenRandomVector(r, V_new, &alpha_new, &gamma_new);

//  find the rotation matrix
    gsl_matrix* R = gsl_matrix_alloc(3,3);
    info = FindRotationMatrix(alpha_old, gamma_old, alpha_new, gamma_new, R);

//  rotate V_old to V_new
    RotateV(R, V_old, V_rot);

    // compare R*V_old and R_new
    for( i=0; i<3; i++ ){
        double diff = gsl_vector_get(V_rot,i) - gsl_vector_get(V_new,i);
        if( fabs(diff) > 1E-10 ){
            printf("diff V(%d) = %10.8f\n", i, diff);
            info = 1;
            break;
        }
    }

//  determine the rotation axis
    gsl_vector* axis_rot = gsl_vector_alloc(3);
    info = FindRotationAxis( R, axis_rot );

//  determine the rotation angle
    double ang_rot = FindRotationAngle( axis_rot, V_old, V_new );

    if( gsl_isnan(ang_rot) ){
        printf("error in getting the rotation angle!\n");
        return 0;
    }

    gsl_vector_free(axis_rot);
    gsl_vector_free(V_old);
    gsl_vector_free(V_new);
    gsl_vector_free(V_rot);
    gsl_matrix_free(R);

    return info;
}

//  ================= MAIN =========================
int main( int argc, char* argv[] ){

    const gsl_rng_type* T = gsl_rng_default;
    gsl_rng* r = gsl_rng_alloc(T);

    srand(time(NULL));
    gsl_rng_set(r, rand());

    int cnt = 0;
    while( cnt < 100000 ){
        if( run_a_test(r) == 0 ){
            printf("successful %6d th test!\n",1+cnt);
        }
        cnt++;
    }

    gsl_rng_free(r);
    return 0;
}
