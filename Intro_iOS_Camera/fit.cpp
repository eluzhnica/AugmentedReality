////
////  fit.cpp
////  Intro_iOS_Camera
////
////  Created by Enxhell Luzhnica on 12/12/15.
////  Copyright © 2015 CMU_16432. All rights reserved.
////
//
//#include "fit.hpp"
//
////-----------------------------------------------------------------
//// Function to return the affine warp between 3D points on a plane
////
//// <in>
//// X = concatenated matrix of 2D projected points in the image (2xN)
//// W = concatenated matrix of 3D points on the plane (3XN)
////
//// <out>
//// H = 3x3 homography matrix
//fmat getPerspectiveTransform(fmat &X, fmat &W) {
//    fmat H;
//    
//    for (int i=0; i<W.n_cols; i++) {
//        fmat result;
//        
//        fmat x = X.col(i);
//        fmat w = W.col(i);
//        
//        result << 0 << 0 << 0 << -w(0,0) << -w(1,0) << -1
//        << w(0,0)*x(1,0) << w(1,0)*x(1,0) << x(1,0) << endr
//        << w(0,0) << w(1,0) << 1 << 0 << 0 << 0
//        << -x(0,0)*w(0,0) << -x(0,0)*w(1,0) << -x(0,0) << endr;
//        H = join_vert(H, result);
//    }
//    
//    fmat U;
//    fvec s;
//    fmat V;
//    
//    svd(U,s,V,H);
//    
//    fmat result = V.col(V.n_cols-1);
//    result.reshape(3, 3);
//    
//    result = result.t();
//    return result;
//}
//
////-----------------------------------------------------------------
//// Function to project points using the affine transform
////
//// <in>
//// W = concatenated matrix of 3D points on the plane (3XN)
//// H = 3x3 homography matrix
////
//// <out>
//// X = concatenated matrix of 2D projected points in the image (2xN)
//fmat warpPerspective(fmat &W, fmat &H) {
//    fmat X;
//    
//    fmat w = W;
//    w = join_vert(w, ones<fmat>(1,w.n_cols));
//    w.shed_row(2);
//    
//    for (int i=0; i<W.n_cols; i++) {
//        fmat sol;
//        sol = H * w.col(i);
//        sol(0) /= sol(2);
//        sol(1) /= sol(2);
//        sol.shed_row(2);
//        X = join_horiz(X, sol);
//    }
//    
//    return X;
//}