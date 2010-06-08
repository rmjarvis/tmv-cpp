///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// The Template Matrix/Vector Library for C++ was created by Mike Jarvis     //
// Copyright (C) 1998 - 2009                                                 //
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


//#include <iostream>

#include "TMV_Blas.h" // Sets BLAS if appropriate
#include "TMV_MultMM.h"
#include "tmv/TMV_Matrix.h"
#include "tmv/TMV_MatrixArith.h"

namespace tmv {

    template <bool ca, class T, class Ta, class Tb> 
    static void ColMult(int M, const Tb b00, const Ta* A, T* C)
    {
        for(int i=M/8;i;--i) {
            C[0] += (ca ? TMV_CONJ(A[0]) : A[0]) * b00;
            C[1] += (ca ? TMV_CONJ(A[1]) : A[1]) * b00;
            C[2] += (ca ? TMV_CONJ(A[2]) : A[2]) * b00;
            C[3] += (ca ? TMV_CONJ(A[3]) : A[3]) * b00;
            C[4] += (ca ? TMV_CONJ(A[4]) : A[4]) * b00;
            C[5] += (ca ? TMV_CONJ(A[5]) : A[5]) * b00;
            C[6] += (ca ? TMV_CONJ(A[6]) : A[6]) * b00;
            C[7] += (ca ? TMV_CONJ(A[7]) : A[7]) * b00;
            A += 8; C += 8;
        }
        M %= 8;
        if (M) {
            if (M >= 4) {
                C[0] += (ca ? TMV_CONJ(A[0]) : A[0]) * b00;
                C[1] += (ca ? TMV_CONJ(A[1]) : A[1]) * b00;
                C[2] += (ca ? TMV_CONJ(A[2]) : A[2]) * b00;
                C[3] += (ca ? TMV_CONJ(A[3]) : A[3]) * b00;
                M -= 4; A += 4; C += 4;
            }
            if (M >= 2) {
                C[0] += (ca ? TMV_CONJ(A[0]) : A[0]) * b00;
                C[1] += (ca ? TMV_CONJ(A[1]) : A[1]) * b00;
                M -= 2; A += 2; C += 2;
            }
            if (M) {
                C[0] += (ca ? TMV_CONJ(A[0]) : A[0]) * b00;
            }
        }
    }

    template <bool ca, bool cb, class T, class Ta, class Tb> 
    static void RecursiveCRCMultMM(
        const int M, const int N, const int K,
        const Ta* A, const Tb* B, T* C,
        const int Ask, const int Bsk, const int Csj)
    {
        if (K > N) {
            int K1 = K/2;
            RecursiveCRCMultMM<ca,cb>(M,N,K1,A,B,C,Ask,Bsk,Csj);
            RecursiveCRCMultMM<ca,cb>(M,N,K-K1,A+K1*Ask,B+K1*Bsk,C,Ask,Bsk,Csj);
        } else if (N > 1) {
            int N1 = N/2;
            RecursiveCRCMultMM<ca,cb>(M,N1,K,A,B,C,Ask,Bsk,Csj);
            RecursiveCRCMultMM<ca,cb>(M,N-N1,K,A,B+N1,C+N1*Csj,Ask,Bsk,Csj);
        } else {
            ColMult<ca>(M,(cb ? TMV_CONJ(B[0]) : B[0]),A,C);
        }
    }

    template <bool add, bool ca, bool cb, class T, class Ta, class Tb> 
    static void DoCRCMultMM(
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
        TMVAssert(A.iscm());
        TMVAssert(B.isrm());
        TMVAssert(C.iscm());

        const int M = C.colsize();
        const int N = C.rowsize();
        const int K = A.rowsize();
        const int Ask = A.stepj();
        const int Bsk = B.stepi();

        const Ta* Ap = A.cptr();
        const Tb* Bp = B.cptr();

        Matrix<T,ColMajor> Ctemp(M,N,T(0));
        T* Ct = Ctemp.ptr();
        const int Ctsj = Ctemp.stepj();
        RecursiveCRCMultMM<ca,cb>(M,N,K,Ap,Bp,Ct, Ask,Bsk,Ctsj);
        if (alpha != T(1)) Ctemp *= alpha;
        if (add) C += Ctemp;
        else C = Ctemp;
    }

    template <bool add, class T, class Ta, class Tb> void CRCMultMM(
        const T alpha, const GenMatrix<Ta>& A, const GenMatrix<Tb>& B,
        const MatrixView<T>& C)
    {
        if (A.isconj())
            if (B.isconj())
                DoCRCMultMM<add,true,true>(alpha,A,B,C);
            else
                DoCRCMultMM<add,true,false>(alpha,A,B,C);
        else
            if (B.isconj())
                DoCRCMultMM<add,false,true>(alpha,A,B,C);
            else
                DoCRCMultMM<add,false,false>(alpha,A,B,C);
    }

#ifdef BLAS
#define INST_SKIP_BLAS
#endif

#define InstFile "TMV_MultMM_CRC.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace tmv


