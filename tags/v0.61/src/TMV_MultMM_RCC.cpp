///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// The Template Matrix/Vector Library for C++ was created by Mike Jarvis     //
// Copyright (C) 2008                                                        //
//                                                                           //
// The project is hosted at http://sourceforge.net/projects/tmv-cpp/         //
// where you can find the current version and current documention.           //
//                                                                           //
// For concerns or problems with the software, Mike may be contacted at      //
// mike_jarvis@users.sourceforge.net                                         //
//                                                                           //
// This program is free software; you can redistribute it and/or             //
// modify it under the terms of the GNU General Public License               //
// as published by the Free Software Foundation; either version 2            //
// of the License, or (at your option) any later version.                    //
//                                                                           //
// This program is distributed in the hope that it will be useful,           //
// but WITHOUT ANY WARRANTY; without even the implied warranty of            //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
// GNU General Public License for more details.                              //
//                                                                           //
// You should have received a copy of the GNU General Public License         //
// along with this program in the file LICENSE.                              //
//                                                                           //
// If not, write to:                                                         //
// The Free Software Foundation, Inc.                                        //
// 51 Franklin Street, Fifth Floor,                                          //
// Boston, MA  02110-1301, USA.                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////



#include "TMV_Blas.h" // Sets BLAS if appropriate
#include "TMV_MultMM.h"
#include "TMV_Matrix.h"
#include "TMV_MatrixArith.h"

namespace tmv {

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM11(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int , const int , const int PARAMFIRSTLAST(T))
     {
       T c00(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 c00 += a0k * bk0;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C < _last);
#endif
       C[0] += c00;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM21(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int , const int PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       T c00(0);
       T c10(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM31(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int , const int PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       T c00(0);
       T c10(0);
       T c20(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c20 += a2k * bk0;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+2 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
       C[2] += c20;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM41(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int , const int PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Ta*const A3 = A2 + Asi;
       T c00(0);
       T c10(0);
       T c20(0);
       T c30(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Ta a3k = ca ? CONJ(A3[k]) : A3[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c20 += a2k * bk0;
	 c30 += a3k * bk0;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+3 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
       C[2] += c20;
       C[3] += c30;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM12(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int , const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Tb*const B1 = B + Bsj;
       T*const C1 = C + Csj;
       T c00(0);
       T c01(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c01 += a0k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1 < _last);
#endif
       C[0] += c00;
       C1[0] += c01;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM22(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Tb*const B1 = B + Bsj;
       T*const C1 = C + Csj;
       T c00(0);
       T c10(0);
       T c01(0);
       T c11(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C+1 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
       C1[0] += c01;
       C1[1] += c11;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM32(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Tb*const B1 = B + Bsj;
       T*const C1 = C + Csj;
       T c00(0);
       T c10(0);
       T c20(0);
       T c01(0);
       T c11(0);
       T c21(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c20 += a2k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
	 c21 += a2k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+2 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+2 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
       C[2] += c20;
       C1[0] += c01;
       C1[1] += c11;
       C1[2] += c21;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM42(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Ta*const A3 = A2 + Asi;
       const Tb*const B1 = B + Bsj;
       T*const C1 = C + Csj;
       T c00(0);
       T c10(0);
       T c01(0);
       T c11(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+1 < _last);
#endif
       C[0] += c00; c00 = T(0);
       C[1] += c10; c10 = T(0);
       C1[0] += c01; c01 = T(0);
       C1[1] += c11; c11 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Ta a3k = ca ? CONJ(A3[k]) : A3[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a2k * bk0;
	 c10 += a3k * bk0;
	 c01 += a2k * bk1;
	 c11 += a3k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C+2 >= _first);
       TMVAssert(C+3 < _last);
       TMVAssert(C1+2 >= _first);
       TMVAssert(C1+3 < _last);
#endif
       C[2] += c00;
       C[3] += c10;
       C1[2] += c01;
       C1[3] += c11;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM13(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int , const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T c00(0);
       T c01(0);
       T c02(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 c00 += a0k * bk0;
	 c01 += a0k * bk1;
	 c02 += a0k * bk2;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1 < _last);
       TMVAssert(C2 >= _first);
       TMVAssert(C2 < _last);
#endif
       C[0] += c00;
       C1[0] += c01;
       C2[0] += c02;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM23(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T c00(0);
       T c10(0);
       T c01(0);
       T c11(0);
       T c02(0);
       T c12(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
	 c02 += a0k * bk2;
	 c12 += a1k * bk2;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+1 < _last);
       TMVAssert(C2 >= _first);
       TMVAssert(C2+1 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
       C1[0] += c01;
       C1[1] += c11;
       C2[0] += c02;
       C2[1] += c12;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM33(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T c00(0);
       T c10(0);
       T c20(0);
       T c01(0);
       T c11(0);
       T c21(0);
       T c02(0);
       T c12(0);
       T c22(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c20 += a2k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
	 c21 += a2k * bk1;
	 c02 += a0k * bk2;
	 c12 += a1k * bk2;
	 c22 += a2k * bk2;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+2 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+2 < _last);
       TMVAssert(C2 >= _first);
       TMVAssert(C2+2 < _last);
#endif
       C[0] += c00;
       C[1] += c10;
       C[2] += c20;
       C1[0] += c01;
       C1[1] += c11;
       C1[2] += c21;
       C2[0] += c02;
       C2[1] += c12;
       C2[2] += c22;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM43(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Ta*const A3 = A2 + Asi;
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T c00(0);
       T c10(0);
       T c01(0);
       T c11(0);
       T c02(0);
       T c12(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
	 c02 += a0k * bk2;
	 c12 += a1k * bk2;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+1 < _last);
       TMVAssert(C2 >= _first);
       TMVAssert(C2+1 < _last);
#endif
       C[0] += c00; c00 = T(0);
       C[1] += c10; c10 = T(0);
       C1[0] += c01; c01 = T(0);
       C1[1] += c11; c11 = T(0);
       C2[0] += c02; c02 = T(0);
       C2[1] += c12; c12 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Ta a3k = ca ? CONJ(A3[k]) : A3[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 c00 += a2k * bk0;
	 c10 += a3k * bk0;
	 c01 += a2k * bk1;
	 c11 += a3k * bk1;
	 c02 += a2k * bk2;
	 c12 += a3k * bk2;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C+2 >= _first);
       TMVAssert(C+3 < _last);
       TMVAssert(C1+2 >= _first);
       TMVAssert(C1+3 < _last);
       TMVAssert(C2+2 >= _first);
       TMVAssert(C2+3 < _last);
#endif
       C[2] += c00;
       C[3] += c10;
       C1[2] += c01;
       C1[3] += c11;
       C2[2] += c02;
       C2[3] += c12;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM14(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int , const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       const Tb*const B3 = B2 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T*const C3 = C2 + Csj;
       T c00(0);
       T c01(0);
       T c02(0);
       T c03(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 const Tb bk3 = cb ? CONJ(B3[k]) : B3[k];
	 c00 += a0k * bk0;
	 c01 += a0k * bk1;
	 c02 += a0k * bk2;
	 c03 += a0k * bk3;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1 < _last);
       TMVAssert(C2 >= _first);
       TMVAssert(C2 < _last);
       TMVAssert(C3 >= _first);
       TMVAssert(C3 < _last);
#endif
       C[0] += c00;
       C1[0] += c01;
       C2[0] += c02;
       C3[0] += c03;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM24(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       const Tb*const B3 = B2 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T*const C3 = C2 + Csj;
       T c00(0);
       T c10(0);
       T c01(0);
       T c11(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+1 < _last);
#endif
       C[0] += c00; c00 = T(0);
       C[1] += c10; c10 = T(0);
       C1[0] += c01; c01 = T(0);
       C1[1] += c11; c11 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 const Tb bk3 = cb ? CONJ(B3[k]) : B3[k];
	 c00 += a0k * bk2;
	 c10 += a1k * bk2;
	 c01 += a0k * bk3;
	 c11 += a1k * bk3;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C2 >= _first);
       TMVAssert(C2+1 < _last);
       TMVAssert(C3 >= _first);
       TMVAssert(C3+1 < _last);
#endif
       C2[0] += c00;
       C2[1] += c10;
       C3[0] += c01;
       C3[1] += c11;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM34(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       const Tb*const B3 = B2 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T*const C3 = C2 + Csj;
       T c00(0);
       T c10(0);
       T c20(0);
       T c01(0);
       T c11(0);
       T c21(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c20 += a2k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
	 c21 += a2k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+2 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+2 < _last);
#endif
       C[0] += c00; c00 = T(0);
       C[1] += c10; c10 = T(0);
       C[2] += c20; c20 = T(0);
       C1[0] += c01; c01 = T(0);
       C1[1] += c11; c11 = T(0);
       C1[2] += c21; c21 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 const Tb bk3 = cb ? CONJ(B3[k]) : B3[k];
	 c00 += a0k * bk2;
	 c10 += a1k * bk2;
	 c20 += a2k * bk2;
	 c01 += a0k * bk3;
	 c11 += a1k * bk3;
	 c21 += a2k * bk3;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C2 >= _first);
       TMVAssert(C2+2 < _last);
       TMVAssert(C3 >= _first);
       TMVAssert(C3+2 < _last);
#endif
       C2[0] += c00;
       C2[1] += c10;
       C2[2] += c20;
       C3[0] += c01;
       C3[1] += c11;
       C3[2] += c21;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     static void RCCMultMM44(const int K,
	 const Ta*const A, const Tb* B, T*const C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       const Ta*const A1 = A + Asi;
       const Ta*const A2 = A1 + Asi;
       const Ta*const A3 = A2 + Asi;
       const Tb*const B1 = B + Bsj;
       const Tb*const B2 = B1 + Bsj;
       const Tb*const B3 = B2 + Bsj;
       T*const C1 = C + Csj;
       T*const C2 = C1 + Csj;
       T*const C3 = C2 + Csj;
       T c00(0);
       T c10(0);
       T c01(0);
       T c11(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a0k * bk0;
	 c10 += a1k * bk0;
	 c01 += a0k * bk1;
	 c11 += a1k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C >= _first);
       TMVAssert(C+1 < _last);
       TMVAssert(C1 >= _first);
       TMVAssert(C1+1 < _last);
#endif
       C[0] += c00; c00 = T(0);
       C[1] += c10; c10 = T(0);
       C1[0] += c01; c01 = T(0);
       C1[1] += c11; c11 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Ta a3k = ca ? CONJ(A3[k]) : A3[k];
	 const Tb bk0 = cb ? CONJ(B[k]) : B[k];
	 const Tb bk1 = cb ? CONJ(B1[k]) : B1[k];
	 c00 += a2k * bk0;
	 c10 += a3k * bk0;
	 c01 += a2k * bk1;
	 c11 += a3k * bk1;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C+2 >= _first);
       TMVAssert(C+3 < _last);
       TMVAssert(C1+2 >= _first);
       TMVAssert(C1+3 < _last);
#endif
       C[2] += c00; c00 = T(0);
       C[3] += c10; c10 = T(0);
       C1[2] += c01; c01 = T(0);
       C1[3] += c11; c11 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a2k = ca ? CONJ(A2[k]) : A2[k];
	 const Ta a3k = ca ? CONJ(A3[k]) : A3[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 const Tb bk3 = cb ? CONJ(B3[k]) : B3[k];
	 c00 += a2k * bk2;
	 c10 += a3k * bk2;
	 c01 += a2k * bk3;
	 c11 += a3k * bk3;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C2+2 >= _first);
       TMVAssert(C2+3 < _last);
       TMVAssert(C3+2 >= _first);
       TMVAssert(C3+3 < _last);
#endif
       C2[2] += c00; c00 = T(0);
       C2[3] += c10; c10 = T(0);
       C3[2] += c01; c01 = T(0);
       C3[3] += c11; c11 = T(0);
       for(int k=K-1;k>=0;--k) {
	 const Ta a0k = ca ? CONJ(A[k]) : A[k];
	 const Ta a1k = ca ? CONJ(A1[k]) : A1[k];
	 const Tb bk2 = cb ? CONJ(B2[k]) : B2[k];
	 const Tb bk3 = cb ? CONJ(B3[k]) : B3[k];
	 c00 += a0k * bk2;
	 c10 += a1k * bk2;
	 c01 += a0k * bk3;
	 c11 += a1k * bk3;
       }
#ifdef TMVFLDEBUG
       TMVAssert(C2 >= _first);
       TMVAssert(C2+1 < _last);
       TMVAssert(C3 >= _first);
       TMVAssert(C3+1 < _last);
#endif
       C2[0] += c00;
       C2[1] += c10;
       C3[0] += c01;
       C3[1] += c11;
     }

   template <bool ca, bool cb, class T, class Ta, class Tb>
     extern void RecursiveRCCMultMM(
	 const int M, const int N, const int K,
	 const Ta* A, const Tb* B, T* C,
	 const int Asi, const int Bsj, const int Csj PARAMFIRSTLAST(T))
     {
       if (K > MMRCC_BLOCKSIZE_K) {
	 int K1 = K/2;
	 RecursiveRCCMultMM<ca,cb>(M,N,K1,A,B,C,Asi,Bsj,Csj 
	     FIRSTLAST1(_first,_last));
	 RecursiveRCCMultMM<ca,cb>(M,N,K-K1,A+K1,B+K1,C,Asi,Bsj,Csj 
	     FIRSTLAST1(_first,_last));
       } else if (M > N && M > 4) {
	 int M1 = M/2;
	 RecursiveRCCMultMM<ca,cb>(M1,N,K,A,B,C,Asi,Bsj,Csj 
	     FIRSTLAST1(_first,_last));
	 RecursiveRCCMultMM<ca,cb>(M-M1,N,K,A+M1*Asi,B,C+M1,Asi,Bsj,Csj 
	     FIRSTLAST1(_first,_last));
       } else if (N > 4) {
	 int N1 = N/2;
	 RecursiveRCCMultMM<ca,cb>(M,N1,K,A,B,C,Asi,Bsj,Csj 
	     FIRSTLAST1(_first,_last));
	 RecursiveRCCMultMM<ca,cb>(M,N-N1,K,A,B+N1*Bsj,C+N1*Csj,Asi,Bsj,Csj 
	     FIRSTLAST1(_first,_last));
       } else {
	 if (N == 4)
	   if (M == 4)
	     RCCMultMM44<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 3)
	     RCCMultMM34<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 2)
	     RCCMultMM24<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else
	     RCCMultMM14<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	 else if (N == 3)
	   if (M == 4)
	     RCCMultMM43<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 3)
	     RCCMultMM33<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 2)
	     RCCMultMM23<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else
	     RCCMultMM13<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	 else if (N == 2)
	   if (M == 4)
	     RCCMultMM42<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 3)
	     RCCMultMM32<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 2)
	     RCCMultMM22<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else
	     RCCMultMM12<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	 else
	   if (M == 4)
	     RCCMultMM41<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 3)
	     RCCMultMM31<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else if (M == 2)
	     RCCMultMM21<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
	   else
	     RCCMultMM11<ca,cb>(K,A,B,C,Asi,Bsj,Csj FIRSTLAST1(_first,_last));
       }
     }

  static void MakeTaskList(int M,int N, int i, int j, 
      std::vector<int>& ilist, std::vector<int>& jlist, int& index)
  {
    if (M > N) {
      int M1 = M/2;
      MakeTaskList(M1,N,i,j,ilist,jlist,index);
      MakeTaskList(M-M1,N,i+M1,j,ilist,jlist,index);
    } else if (N > 1) {
      int N1 = N/2;
      MakeTaskList(M,N1,i,j,ilist,jlist,index);
      MakeTaskList(M,N-N1,i,j+N1,ilist,jlist,index);
    } else {
      ilist[index] = i;
      jlist[index] = j;
      index++;
    }
  }

  template <bool add, bool ca, bool cb, class T, class Ta, class Tb> 
    extern void DoRCCMultMM(
	const T alpha, const GenMatrix<Ta>& A, const GenMatrix<Tb>& B,
	const MatrixView<T>& C)
    {
      TMVAssert(A.colsize() == C.colsize());
      TMVAssert(A.rowsize() == B.colsize());
      TMVAssert(B.rowsize() == C.rowsize());
      TMVAssert(C.colsize() > 0);
      TMVAssert(C.rowsize() > 0);
      TMVAssert(A.rowsize() > 0);
      TMVAssert(alpha != T(0));
      TMVAssert(ca == A.isconj());
      TMVAssert(cb == B.isconj());
      TMVAssert(C.ct()==NonConj);
      TMVAssert(A.isrm());
      TMVAssert(B.iscm());
      TMVAssert(C.iscm());

      const int M = C.colsize();
      const int N = C.rowsize();
      const int K = A.rowsize();
      const int Asi = A.stepi();
      const int Bsj = B.stepj();

      const Ta* Ap = A.cptr();
      const Tb* Bp = B.cptr();

      const int MB = MMRCC_BLOCKSIZE_MN;
      const int NB = MMRCC_BLOCKSIZE_MN*sizeof(double)/sizeof(T);
      const int Mb = M/MB;
      const int Nb = N/NB;
      if (Mb || Nb) {
	const int M2 = M-Mb*MB;
	const int N2 = N-Nb*NB;
	const int M1 = M-M2;
	const int N1 = N-N2;

	// This is best done recursively to avoid cache misses as much 
	// as possible, but openmp doesn't do recursion very well (yet), 
	// so I just figure out the recursive order first and do a 
	// simple loop that openmp can divvy up appropriately.
	const int MbNb = Mb*Nb;
	std::vector<int> ilist(MbNb);
	std::vector<int> jlist(MbNb);
	if (MbNb) {
	  int listindex=0;
	  MakeTaskList(Mb,Nb,0,0,ilist,jlist,listindex);
	}
#ifdef _OPENMP
#pragma omp parallel
#endif
	{
	  tmv::Matrix<T,tmv::ColMajor> Ctemp(MIN(M,MB),MIN(N,NB));
	  T* Ct = Ctemp.ptr();
	  const int Ctsj = Ctemp.stepj();

	  // Full MBxNB blocks in C matrix
	  if (Mb && Nb) {
#ifdef _OPENMP
#pragma omp for nowait
#endif
	    for(int ij=0;ij<int(MbNb);ij++) {
	      const int i=ilist[ij];
	      const int j=jlist[ij];
	      const int ii = i*MB;
	      const int jj = j*NB;

	      Ctemp.Zero();
	      RecursiveRCCMultMM<ca,cb>(MB,NB,K,Ap+ii*Asi,Bp+jj*Bsj,Ct,
		  Asi,Bsj,Ctsj FIRSTLAST1(Ctemp.first,Ctemp.last));
	      if (alpha != T(1)) Ctemp *= alpha;
	      if (add) C.SubMatrix(ii,ii+MB,jj,jj+NB) += Ctemp;
	      else C.SubMatrix(ii,ii+MB,jj,jj+NB) = Ctemp;
	    }
	  } // Mb && Nb

	  // MB x N2 partial blocks:
	  if (Mb && N2) {
#ifdef _OPENMP
#pragma omp for nowait
#endif
	    for(int i=0;i<int(Mb);i++) {
	      const int ii = i*MB;

	      Ctemp.Zero();
	      RecursiveRCCMultMM<ca,cb>(MB,N2,K,Ap+ii*Asi,Bp+N1*Bsj,Ct,
		  Asi,Bsj,Ctsj FIRSTLAST1(Ctemp.first,Ctemp.last));
	      if (alpha != T(1)) Ctemp.Cols(0,N2) *= alpha;
	      if (add) C.SubMatrix(ii,ii+MB,N1,N) += Ctemp.Cols(0,N2);
	      else C.SubMatrix(ii,ii+MB,N1,N) = Ctemp.Cols(0,N2);
	    }
	  } // Mb && N2

	    // M2 x NB partial blocks:
	  if (M2 && Nb) {
#ifdef _OPENMP
#pragma omp for nowait
#endif
	    for(int j=0;j<int(Nb);j++) {
	      const int jj = j*NB;

	      Ctemp.Zero();
	      RecursiveRCCMultMM<ca,cb>(M2,NB,K,Ap+M1*Asi,Bp+jj*Bsj,Ct,
		  Asi,Bsj,Ctsj FIRSTLAST1(Ctemp.first,Ctemp.last));
	      if (alpha != T(1)) Ctemp.Rows(0,M2) *= alpha;
	      if (add) C.SubMatrix(M1,M,jj,jj+NB) += Ctemp.Rows(0,M2);
	      else C.SubMatrix(M1,M,jj,jj+NB) = Ctemp.Rows(0,M2);
	    }
	  } // M2 && Nb

	  // Final M2 x N2 partial block
	  if (M2 && N2) {
#ifdef _OPENMP
#pragma omp single
#endif
	    {
	      Ctemp.Zero();
	      RecursiveRCCMultMM<ca,cb>(M2,N2,K,Ap+M1*Asi,Bp+N1*Bsj,Ct,
		  Asi,Bsj,Ctsj FIRSTLAST1(Ctemp.first,Ctemp.last));
	      if (alpha != T(1)) Ctemp.SubMatrix(0,M2,0,N2) *= alpha;
	      if (add) C.SubMatrix(M1,M,N1,N) += Ctemp.SubMatrix(0,M2,0,N2);
	      else C.SubMatrix(M1,M,N1,N) = Ctemp.SubMatrix(0,M2,0,N2);
	    } 
	  } // M2 && N2
	} // end parallel
      } // Mb || Nb
      else {
	tmv::Matrix<T,tmv::ColMajor> Ctemp(M,N,T(0));
	T* Ct = Ctemp.ptr();
	int Ctsj = Ctemp.stepj();
	RecursiveRCCMultMM<ca,cb>(M,N,K,Ap,Bp,Ct,Asi,Bsj,Ctsj 
	    FIRSTLAST1(Ctemp.first,Ctemp.last));
	if (alpha != T(1)) Ctemp *= alpha;
	if (add) C += Ctemp;
	else C = Ctemp;
      }
    }

  template <bool add, class T, class Ta, class Tb> void RCCMultMM(
      const T alpha, const GenMatrix<Ta>& A, const GenMatrix<Tb>& B,
      const MatrixView<T>& C)
  {
    if (A.isconj())
      if (B.isconj())
	DoRCCMultMM<add,true,true>(alpha,A,B,C);
      else
	DoRCCMultMM<add,true,false>(alpha,A,B,C);
    else
      if (B.isconj())
	DoRCCMultMM<add,false,true>(alpha,A,B,C);
      else
	DoRCCMultMM<add,false,false>(alpha,A,B,C);
  }

#ifdef BLAS
#define INST_SKIP_BLAS
#endif

#define InstFile "TMV_MultMM_RCC.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace mv


