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


#ifndef TMV_SmallMatrixArithFunc_H
#define TMV_SmallMatrixArithFunc_H

#include "TMV_SmallVectorArithFunc.h"

#define CT std::complex<T>

namespace tmv {

    template <class T1, class T2, class T3, class T4>
    struct OKTypeHelper3 { enum { ok=true }; };
    template <class T>
    struct OKTypeHelper3<T,T,CT,T> { enum { ok=false }; };
    template <class T>
    struct OKTypeHelper3<T,CT,T,T> { enum { ok=false }; };
    template <class T>
    struct OKTypeHelper3<T,CT,CT,T> { enum { ok=false }; };
    template <class T>
    struct OKTypeHelper3<CT,T,T,T> { enum { ok=false }; };
    template <class T>
    struct OKTypeHelper3<CT,T,CT,T> { enum { ok=false }; };
    template <class T>
    struct OKTypeHelper3<CT,CT,T,T> { enum { ok=false }; };
    template <class T>
    struct OKTypeHelper3<CT,CT,CT,T> { enum { ok=false }; };

#define OKTypes(T1,T2) (OKTypeHelper<T1,T2>::ok)
#define OKTypes2(T1,T2,T3) (OKTypeHelper2<T1,T2,T3>::ok)
#define OKTypes3(T1,T2,T3,T4) (OKTypeHelper3<T1,T2,T3,T4>::ok)
#define ProductType(T1,T2) typename ProdType<T1,T2>::Tprod

    // y = alpha * A * x
    template <int M, int N, StorageType S, class Ta, class Tx, class Ty, bool ok>
    struct MultMV_1Helper
    { MultMV_1Helper(const Ta*, const Tx*, Ty*) {} };
    template <int M, int N, class Ta, class Tx, class Ty>
    struct MultMV_1Helper<M,N,ColMajor,Ta,Tx,Ty,true>
    {
        MultMV_1Helper(const Ta* A, const Tx* x, Ty* y)
        { // S == ColMajor
            for(int i=0;i<M;++i) y[i] = Ty(0);
            for(int j=0;j<N;++j) {
                AddVVHelper<M,Tx,Ta,Ty,true>(x[j],A+j*M,y);
            }
        }
    };
    template <int M, int N, class Ta, class Tx, class Ty>
    struct MultMV_1Helper<M,N,RowMajor,Ta,Tx,Ty,true>
    {
        MultMV_1Helper(const Ta* A, const Tx* x, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                y[i] = Ty(0);
                MultVVHelper<N,Ty,Ta,Tx>(y[i],A+i*N,x);
            }
        }
    };
    template <int M, int N, StorageType S, class Ta, class Tx, class Ty>
    inline void MultMV_1(const Ta* A, const Tx* x, Ty* y)
    { MultMV_1Helper<M,N,S,Ta,Tx,Ty,OKTypes2(Ta,Tx,Ty)>(A,x,y); }

    template <int M, int N, StorageType S, class T, class Ta, class Tx, class Ty, bool ok>
    struct MultMVHelper
    { MultMVHelper(const T, const Ta*, const Tx*, Ty*) {} };
    template <int M, int N, class T, class Ta, class Tx, class Ty>
    struct MultMVHelper<M,N,ColMajor,T,Ta,Tx,Ty,true>
    {
        MultMVHelper(const T alpha, const Ta* A, const Tx* x, Ty* y)
        { // S == ColMajor
            for(int i=0;i<M;++i) y[i] = Ty(0);
            for(int j=0;j<N;++j) {
                typedef ProductType(T,Tx) PT;
                PT xj = alpha*x[j];
                AddVVHelper<M,PT,Ta,Ty,true>(xj,A+j*M,y);
            }
        }
    };
    template <int M, int N, class T, class Ta, class Tx, class Ty>
    struct MultMVHelper<M,N,RowMajor,T,Ta,Tx,Ty,true>
    {
        MultMVHelper(const T alpha, const Ta* A, const Tx* x, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                y[i] = Ty(0);
                MultVVHelper<N,Ty,Ta,Tx>(y[i],A+i*N,x);
                y[i] *= alpha;
            }
        }
    };
    template <int M, int N, StorageType S, class T, class Ta, class Tx, class Ty>
    inline void MultMV(const T alpha, const Ta* A, const Tx* x, Ty* y)
    { MultMVHelper<M,N,S,T,Ta,Tx,Ty,OKTypes3(T,Ta,Tx,Ty)>(alpha,A,x,y); }

    // y += alpha * A * x
    template <int M, int N, StorageType S, class Ta, class Tx, class Ty, bool ok>
    struct AddMultMV_1Helper
    { AddMultMV_1Helper(const Ta*, const Tx*, Ty*) {} };
    template <int M, int N, class Ta, class Tx, class Ty>
    struct AddMultMV_1Helper<M,N,ColMajor,Ta,Tx,Ty,true>
    {
        AddMultMV_1Helper(const Ta* A, const Tx* x, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                AddVVHelper<M,Tx,Ta,Ty,true>(x[j],A+j*M,y);
            }
        }
    };
    template <int M, int N, class Ta, class Tx, class Ty>
    struct AddMultMV_1Helper<M,N,RowMajor,Ta,Tx,Ty,true>
    {
        AddMultMV_1Helper(const Ta* A, const Tx* x, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                MultVVHelper<N,Ty,Ta,Tx>(y[i],A+i*N,x);
            }
        }
    };
    template <int M, int N, StorageType S, class Ta, class Tx, class Ty>
    inline void AddMultMV_1(const Ta* A, const Tx* x, Ty* y)
    { AddMultMV_1Helper<M,N,S,Ta,Tx,Ty,OKTypes2(Ta,Tx,Ty)>(A,x,y); }

    template <int M, int N, StorageType S, class Ta, class Tx, class Ty, bool ok>
    struct AddMultMV_m1Helper 
    { AddMultMV_m1Helper(const Ta*, const Tx*, Ty*) {} };
    template <int M, int N, class Ta, class Tx, class Ty>
    struct AddMultMV_m1Helper<M,N,ColMajor,Ta,Tx,Ty,true>
    {
        AddMultMV_m1Helper(const Ta* A, const Tx* x, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                AddVVHelper<M,Tx,Ta,Ty,true>(-x[j],A+j*M,y);
            }
        }
    };
    template <int M, int N, class Ta, class Tx, class Ty>
    struct AddMultMV_m1Helper<M,N,RowMajor,Ta,Tx,Ty,true>
    {
        AddMultMV_m1Helper(const Ta* A, const Tx* x, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                for(int j=0;j<N;++j) y[i] -= A[i*N+j] * x[j];
            }
        }
    };
    template <int M, int N, StorageType S, class Ta, class Tx, class Ty>
    inline void AddMultMV_m1(const Ta* A, const Tx* x, Ty* y)
    { AddMultMV_m1Helper<M,N,S,Ta,Tx,Ty,OKTypes2(Ta,Tx,Ty)>(A,x,y); }

    template <int M, int N, StorageType S, class T, class Ta, class Tx, class Ty, bool ok>
    struct AddMultMVHelper 
    { AddMultMVHelper(const T, const Ta*, const Tx*, Ty*) {} };
    template <int M, int N, class T, class Ta, class Tx, class Ty>
    struct AddMultMVHelper<M,N,ColMajor,T,Ta,Tx,Ty,true>
    {
        AddMultMVHelper(const T alpha, const Ta* A, const Tx* x, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                typedef ProductType(T,Tx) PT;
                PT xj = alpha*x[j];
                AddVVHelper<M,PT,Ta,Ty,true>(xj,A+j*M,y);
            }
        }
    };
    template <int M, int N, class T, class Ta, class Tx, class Ty>
    struct AddMultMVHelper<M,N,RowMajor,T,Ta,Tx,Ty,true>
    {
        AddMultMVHelper(const T alpha, const Ta* A, const Tx* x, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                Ty yi(0);
                MultVVHelper<N,Ty,Ta,Tx>(yi,A+i*N,x);
                y[i] += yi * alpha;
            }
        }
    };
    template <int M, int N, StorageType S, class T, class Ta, class Tx, class Ty>
    inline void AddMultMV(const T alpha, const Ta* A, const Tx* x, Ty* y)
    { AddMultMVHelper<M,N,S,T,Ta,Tx,Ty,OKTypes3(T,Ta,Tx,Ty)>(alpha,A,x,y); }

    // y = alpha * x * A
    template <int M, int N, StorageType S, class Tx, class Ta, class Ty, bool ok>
    struct MultVM_1Helper
    { MultVM_1Helper(const Tx*, const Ta*, Ty*) {} };
    template <int M, int N, class Tx, class Ta, class Ty>
    struct MultVM_1Helper<M,N,ColMajor,Tx,Ta,Ty,true>
    {
        MultVM_1Helper(const Tx* x, const Ta* A, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                y[j] = Ty(0);
                MultVVHelper<M,Ty,Ta,Tx>(y[j],A+j*M,x);
            }
        }
    };
    template <int M, int N, class Tx, class Ta, class Ty>
    struct MultVM_1Helper<M,N,RowMajor,Tx,Ta,Ty,true>
    {
        MultVM_1Helper(const Tx* x, const Ta* A, Ty* y)
        { // S == RowMajor
            for(int j=0;j<N;++j) y[j] = Ty(0);
            for(int i=0;i<M;++i) {
                AddVVHelper<N,Tx,Ta,Ty,true>(x[i],A+i*N,y);
            }
        }
    };
    template <int M, int N, StorageType S, class Tx, class Ta, class Ty>
    inline void MultVM_1(const Tx* x, const Ta* A, Ty* y)
    { MultVM_1Helper<M,N,S,Tx,Ta,Ty,OKTypes2(Ta,Tx,Ty)>(x,A,y); }

    template <int M, int N, StorageType S, class T, class Tx, class Ta, class Ty, bool ok>
    struct MultVMHelper
    { MultVMHelper(const T, const Tx*, const Ta*, Ty*) {} };
    template <int M, int N, class T, class Tx, class Ta, class Ty>
    struct MultVMHelper<M,N,ColMajor,T,Tx,Ta,Ty,true>
    {
        MultVMHelper(const T alpha, const Tx* x, const Ta* A, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                y[j] = Ty(0);
                MultVVHelper<M,Ty,Ta,Tx>(y[j],A+j*M,x);
                y[j] *= alpha;
            }
        }
    };
    template <int M, int N, class T, class Tx, class Ta, class Ty>
    struct MultVMHelper<M,N,RowMajor,T,Tx,Ta,Ty,true>
    {
        MultVMHelper(const T alpha, const Tx* x, const Ta* A, Ty* y)
        { // S == RowMajor
            for(int j=0;j<N;++j) y[j] = Ty(0);
            for(int i=0;i<M;++i) {
                typedef ProductType(T,Tx) PT;
                PT xi = alpha*x[i];
                AddVVHelper<N,PT,Ta,Ty,true>(xi,A+i*N,y);
            }
        }
    };
    template <int M, int N, StorageType S, class T, class Tx, class Ta, class Ty>
    inline void MultVM(const T alpha, const Tx* x, const Ta* A, Ty* y)
    { MultVMHelper<M,N,S,T,Tx,Ta,Ty,OKTypes3(T,Ta,Tx,Ty)>(alpha,x,A,y); }

    // y += alpha * A * x
    template <int M, int N, StorageType S, class Tx, class Ta, class Ty, bool ok>
    struct AddMultVM_1Helper
    { AddMultVM_1Helper(const Tx*, const Ta*, Ty*) {} };
    template <int M, int N, class Tx, class Ta, class Ty>
    struct AddMultVM_1Helper<M,N,ColMajor,Tx,Ta,Ty,true>
    {
        AddMultVM_1Helper(const Tx* x, const Ta* A, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                MultVVHelper<M,Ty,Ta,Tx>(y[j],A+j*M,x);
            }
        }
    };
    template <int M, int N, class Tx, class Ta, class Ty>
    struct AddMultVM_1Helper<M,N,RowMajor,Tx,Ta,Ty,true>
    {
        AddMultVM_1Helper(const Tx* x, const Ta* A, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                AddVVHelper<N,Tx,Ta,Ty,true>(x[i],A+i*N,y);
            }
        }
    };
    template <int M, int N, StorageType S, class Tx, class Ta, class Ty>
    inline void AddMultVM_1(const Tx* x, const Ta* A, Ty* y)
    { AddMultVM_1Helper<M,N,S,Tx,Ta,Ty,OKTypes2(Ta,Tx,Ty)>(x,A,y); }

    template <int M, int N, StorageType S, class Tx, class Ta, class Ty, bool ok>
    struct AddMultVM_m1Helper
    { AddMultVM_m1Helper(const Tx*, const Ta*, Ty*) {} };
    template <int M, int N, class Tx, class Ta, class Ty>
    struct AddMultVM_m1Helper<M,N,ColMajor,Tx,Ta,Ty,true>
    {
        AddMultVM_m1Helper(const Tx* x, const Ta* A, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                for(int i=0;i<M;++i) y[j] -= A[j*M+i] * x[i];
            }
        }
    };
    template <int M, int N, class Tx, class Ta, class Ty>
    struct AddMultVM_m1Helper<M,N,RowMajor,Tx,Ta,Ty,true>
    {
        AddMultVM_m1Helper(const Tx* x, const Ta* A, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                AddVVHelper<N,Tx,Ta,Ty,true>(-x[i],A+i*N,y);
            }
        }
    };
    template <int M, int N, StorageType S, class Tx, class Ta, class Ty>
    inline void AddMultVM_m1(const Tx* x, const Ta* A, Ty* y)
    { AddMultVM_m1Helper<M,N,S,Tx,Ta,Ty,OKTypes2(Ta,Tx,Ty)>(x,A,y); }

    template <int M, int N, StorageType S, class T, class Tx, class Ta, class Ty, bool ok>
    struct AddMultVMHelper
    { AddMultVMHelper(const T, const Tx*, const Ta*, Ty*) {} };
    template <int M, int N, class T, class Tx, class Ta, class Ty>
    struct AddMultVMHelper<M,N,ColMajor,T,Tx,Ta,Ty,true>
    {
        AddMultVMHelper(const T alpha, const Tx* x, const Ta* A, Ty* y)
        { // S == ColMajor
            for(int j=0;j<N;++j) {
                Ty yj(0);
                MultVVHelper<M,Ty,Ta,Tx>(yj,A+j*M,x);
                y[j] += yj * alpha;
            }
        }
    };
    template <int M, int N, class T, class Tx, class Ta, class Ty>
    struct AddMultVMHelper<M,N,RowMajor,T,Tx,Ta,Ty,true>
    {
        AddMultVMHelper(const T alpha, const Tx* x, const Ta* A, Ty* y)
        { // S == RowMajor
            for(int i=0;i<M;++i) {
                typedef ProductType(T,Tx) PT;
                PT xi = alpha*x[i];
                AddVVHelper<N,PT,Ta,Ty,true>(xi,A+i*N,y);
            }
        }
    };
    template <int M, int N, StorageType S, class T, class Tx, class Ta, class Ty>
    inline void AddMultVM(const T alpha, const Tx* x, const Ta* A, Ty* y)
    { AddMultVMHelper<M,N,S,T,Tx,Ta,Ty,OKTypes3(T,Ta,Tx,Ty)>(alpha,x,A,y); }

    // B += alpha * A
    template <int M, int N, StorageType Sa, StorageType Sb, class Ta, class Tb, bool ok>
    struct AddMM_1Helper
    { AddMM_1Helper(const Ta*, Tb*) {} };
    template <int M, int N, StorageType S, class Ta, class Tb>
    struct AddMM_1Helper<M,N,S,S,Ta,Tb,true>
    {
        AddMM_1Helper(const Ta* A, Tb* B)
        { // Sa == Sb
            AddVV_1Helper<M*N,Ta,Tb,true>(A,B);
        }
    };
    template <int M, int N, class Ta, class Tb>
    struct AddMM_1Helper<M,N,ColMajor,RowMajor,Ta,Tb,true>
    {
        AddMM_1Helper(const Ta* A, Tb* B)
        { // ColMajor, RowMajor
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) B[i*N+j] += A[j*M+i];
        }
    };
    template <int M, int N, class Ta, class Tb>
    struct AddMM_1Helper<M,N,RowMajor,ColMajor,Ta,Tb,true>
    {
        AddMM_1Helper(const Ta* A, Tb* B)
        { // RowMajor, ColMajor
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) B[j*M+i] += A[i*N+j];
        }
    };
    template <int M, int N, StorageType Sa, StorageType Sb, class Ta, class Tb> 
    inline void AddMM_1(const Ta* A, Tb* B)
    { AddMM_1Helper<M,N,Sa,Sb,Ta,Tb,OKTypes(Ta,Tb)>(A,B); }

    template <int M, int N, StorageType Sa, StorageType Sb, class Ta, class Tb, bool ok>
    struct AddMM_m1Helper
    { AddMM_m1Helper(const Ta*, Tb*) {} };
    template <int M, int N, StorageType S, class Ta, class Tb>
    struct AddMM_m1Helper<M,N,S,S,Ta,Tb,true>
    {
        AddMM_m1Helper(const Ta* A, Tb* B) 
        { // Sa == Sb
            AddVV_m1Helper<M*N,Ta,Tb,true>(A,B);
        }
    };
    template <int M, int N, class Ta, class Tb>
    struct AddMM_m1Helper<M,N,ColMajor,RowMajor,Ta,Tb,true>
    {
        AddMM_m1Helper(const Ta* A, Tb* B) 
        { // ColMajor, RowMajor
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) B[i*N+j] -= A[j*M+i];
        }
    };
    template <int M, int N, class Ta, class Tb>
    struct AddMM_m1Helper<M,N,RowMajor,ColMajor,Ta,Tb,true>
    {
        AddMM_m1Helper(const Ta* A, Tb* B) 
        { // RowMajor, ColMajor
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) B[j*M+i] -= A[i*N+j];
        }
    };
    template <int M, int N, StorageType Sa, StorageType Sb, class Ta, class Tb> 
    inline void AddMM_m1(const Ta* A, Tb* B)
    { AddMM_m1Helper<M,N,Sa,Sb,Ta,Tb,OKTypes(Ta,Tb)>(A,B); }

    template <int M, int N, StorageType Sa, StorageType Sb, class T, class Ta, class Tb, bool ok>
    struct AddMMHelper
    { AddMMHelper(const T, const Ta*, Tb*) {} };
    template <int M, int N, StorageType S, class T, class Ta, class Tb>
    struct AddMMHelper<M,N,S,S,T,Ta,Tb,true>
    {
        inline AddMMHelper(const T alpha, const Ta* A, Tb* B)
        { // Sa == Sb
            AddVVHelper<M*N,T,Ta,Tb,true>(alpha,A,B);
        }
    };
    template <int M, int N, class T, class Ta, class Tb>
    struct AddMMHelper<M,N,ColMajor,RowMajor,T,Ta,Tb,true>
    {
        inline AddMMHelper(const T alpha, const Ta* A, Tb* B)
        { // ColMajor, RowMajor
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) B[i*N+j] += alpha*A[j*M+i];
        }
    };
    template <int M, int N, class T, class Ta, class Tb>
    struct AddMMHelper<M,N,RowMajor,ColMajor,T,Ta,Tb,true>
    {
        inline AddMMHelper(const T alpha, const Ta* A, Tb* B)
        { // RowMajor, ColMajor
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) B[j*M+i] += alpha*A[i*N+j];
        }
    };
    template <int M, int N, StorageType Sa, StorageType Sb, class T, class Ta, class Tb> 
    inline void AddMM(const T alpha, const Ta* A, Tb* B)
    { AddMMHelper<M,N,Sa,Sb,T,Ta,Tb,OKTypes2(T,Ta,Tb)>(alpha,A,B); }

    // C = alpha * A * B
#define Aik (Sa == RowMajor ? A[i*K+k] : A[k*M+i])
#define Bkj (Sb == RowMajor ? B[k*N+j] : B[j*K+k])
#define Cij (Sc == RowMajor ? C[i*N+j] : C[j*M+i])

    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc, bool ok>
    struct MultMM_1Helper
    { MultMM_1Helper(const Ta*, const Tb*, Tc*) {} };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc>
    struct MultMM_1Helper<M,N,K,Sa,Sb,Sc,Ta,Tb,Tc,true>
    {
        inline MultMM_1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Default: Inner product method
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) {
                Tc cij(0);
                for(int k=0;k<K;++k) cij += Aik * Bkj;
                Cij = cij;
            }
        }
    };
    template <int M, int N, int K, StorageType Sb, class Ta, class Tb, class Tc>
    struct MultMM_1Helper<M,N,K,ColMajor,Sb,ColMajor,Ta,Tb,Tc,true>
    {
        inline MultMM_1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Sa == ColMajor && Sc == ColMajor
            for(int j=0;j<M*N;++j) C[j]=Tc(0);
            for(int j=0;j<N;++j) for(int k=0;k<K;++k) {
                AddVVHelper<M,Tb,Ta,Tc,true>(Bkj,A+k*M,C+j*M);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, class Ta, class Tb, class Tc>
    struct MultMM_1Helper<M,N,K,Sa,RowMajor,RowMajor,Ta,Tb,Tc,true>
    {
        inline MultMM_1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Sb == RowMajor && Sc == RowMajor
            for(int j=0;j<M*N;++j) C[j]=Tc(0);
            for(int i=0;i<M;++i) for(int k=0;k<K;++k) {
                AddVVHelper<N,Ta,Tb,Tc,true>(Aik,B+k*N,C+i*N);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc>
    inline void MultMM_1(const Ta* A, const Tb* B, Tc* C)
    { MultMM_1Helper<M,N,K,Sa,Sb,Sc,Ta,Tb,Tc,OKTypes2(Ta,Tb,Tc)>(A,B,C); }

    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class T, class Ta, class Tb, class Tc, bool ok>
    struct MultMMHelper
    { MultMMHelper(const T, const Ta*, const Tb*, Tc*) {} };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class T, class Ta, class Tb, class Tc>
    struct MultMMHelper<M,N,K,Sa,Sb,Sc,T,Ta,Tb,Tc,true>
    {
        inline MultMMHelper(const T alpha, const Ta* A, const Tb* B, Tc* C)
        { // Default: Inner product method
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) {
                Tc cij(0);
                for(int k=0;k<K;++k) cij += Aik * Bkj;
                Cij = alpha * cij;
            }
        }
    };
    template <int M, int N, int K, StorageType Sb, class T, class Ta, class Tb, class Tc>
    struct MultMMHelper<M,N,K,ColMajor,Sb,ColMajor,T,Ta,Tb,Tc,true>
    {
        inline MultMMHelper(const T alpha, const Ta* A, const Tb* B, Tc* C)
        { // Sa == ColMajor && Sc == ColMajor
            for(int j=0;j<M*N;++j) C[j]=Tc(0);
            for(int j=0;j<N;++j) for(int k=0;k<K;++k) {
                typedef ProductType(T,Tb) PT;
                PT bkj = alpha * Bkj;
                AddVVHelper<M,PT,Ta,Tc,true>(bkj,A+k*M,C+j*M);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, class T, class Ta, class Tb, class Tc>
    struct MultMMHelper<M,N,K,Sa,RowMajor,RowMajor,T,Ta,Tb,Tc,true>
    {
        inline MultMMHelper(const T alpha, const Ta* A, const Tb* B, Tc* C)
        { // Sb == RowMajor && Sc == RowMajor
            for(int j=0;j<M*N;++j) C[j]=Tc(0);
            for(int i=0;i<M;++i) for(int k=0;k<K;++k) {
                typedef ProductType(T,Ta) PT;
                PT aik = alpha * Aik;
                AddVVHelper<N,PT,Tb,Tc,true>(aik,B+k*N,C+i*N);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class T, class Ta, class Tb, class Tc>
    inline void MultMM(const T alpha, const Ta* A, const Tb* B, Tc* C)
    { MultMMHelper<M,N,K,Sa,Sb,Sc,T,Ta,Tb,Tc,OKTypes3(T,Ta,Tb,Tc)>(alpha,A,B,C); }

    // C += alpha * A * B
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc, bool ok>
    struct AddMultMM_1Helper
    { AddMultMM_1Helper(const Ta*, const Tb*, Tc*) {} };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc>
    struct AddMultMM_1Helper<M,N,K,Sa,Sb,Sc,Ta,Tb,Tc,true>
    {
        inline AddMultMM_1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Default: Inner product method
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) {
                Tc cij(0);
                for(int k=0;k<K;++k) cij += Aik * Bkj;
                Cij += cij;
            }
        }
    };
    template <int M, int N, int K, StorageType Sb, class Ta, class Tb, class Tc>
    struct AddMultMM_1Helper<M,N,K,ColMajor,Sb,ColMajor,Ta,Tb,Tc,true>
    {
        inline AddMultMM_1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Sa == ColMajor && Sc == ColMajor
            for(int j=0;j<N;++j) for(int k=0;k<K;++k) {
                Tb bkj = Bkj;
                AddVVHelper<M,Tb,Ta,Tc,true>(bkj,A+k*M,C+j*M);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, class Ta, class Tb, class Tc>
    struct AddMultMM_1Helper<M,N,K,Sa,RowMajor,RowMajor,Ta,Tb,Tc,true>
    {
        inline AddMultMM_1Helper(const Ta* A, const Tb* B, Tc* C)
        {  // Sb == RowMajor && Sc == RowMajor
            for(int i=0;i<M;++i) for(int k=0;k<K;++k) {
                Ta aik = Aik;
                AddVVHelper<N,Ta,Tb,Tc,true>(aik,B+k*N,C+i*N);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc>
    inline void AddMultMM_1(const Ta* A, const Tb* B, Tc* C)
    { AddMultMM_1Helper<M,N,K,Sa,Sb,Sc,Ta,Tb,Tc,OKTypes2(Ta,Tb,Tc)>(A,B,C); }

    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc, bool ok>
    struct AddMultMM_m1Helper
    { AddMultMM_m1Helper(const Ta*, const Tb*, Tc*) {} };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc>
    struct AddMultMM_m1Helper<M,N,K,Sa,Sb,Sc,Ta,Tb,Tc,true>
    {
        inline AddMultMM_m1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Default: Inner product method
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) {
                Tc cij(0);
                for(int k=0;k<K;++k) cij += Aik * Bkj;
                Cij -= cij;
            }
        }
    };
    template <int M, int N, int K, StorageType Sb, class Ta, class Tb, class Tc>
    struct AddMultMM_m1Helper<M,N,K,ColMajor,Sb,ColMajor,Ta,Tb,Tc,true>
    {
        inline AddMultMM_m1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Sa == ColMajor && Sc == ColMajor
            for(int j=0;j<N;++j) for(int k=0;k<K;++k) {
                Tb bkj = Bkj;
                AddVVHelper<M,Tb,Ta,Tc,true>(-bkj,A+k*M,C+j*M);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, class Ta, class Tb, class Tc>
    struct AddMultMM_m1Helper<M,N,K,Sa,RowMajor,RowMajor,Ta,Tb,Tc,true>
    {
        inline AddMultMM_m1Helper(const Ta* A, const Tb* B, Tc* C)
        { // Sb == RowMajor && Sc == RowMajor
            for(int i=0;i<M;++i) for(int k=0;k<K;++k) {
                Ta aik = Aik;
                AddVVHelper<N,Ta,Tb,Tc,true>(-aik,B+k*N,C+i*N);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class Ta, class Tb, class Tc>
    inline void AddMultMM_m1(const Ta* A, const Tb* B, Tc* C)
    { AddMultMM_m1Helper<M,N,K,Sa,Sb,Sc,Ta,Tb,Tc,OKTypes2(Ta,Tb,Tc)>(A,B,C); }

    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class T, class Ta, class Tb, class Tc, bool ok>
    struct AddMultMMHelper
    { AddMultMMHelper(const T, const Ta*, const Tb*, Tc*) {} };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class T, class Ta, class Tb, class Tc>
    struct AddMultMMHelper<M,N,K,Sa,Sb,Sc,T,Ta,Tb,Tc,true>
    {
        inline AddMultMMHelper(const T alpha, const Ta* A, const Tb* B, Tc* C)
        { // Default: Inner product method
            for(int i=0;i<M;++i) for(int j=0;j<N;++j) {
                Tc cij(0);
                for(int k=0;k<K;++k) cij += Aik * Bkj;
                Cij += alpha * cij;
            }
        }
    };
    template <int M, int N, int K, StorageType Sb, class T, class Ta, class Tb, class Tc>
    struct AddMultMMHelper<M,N,K,ColMajor,Sb,ColMajor,T,Ta,Tb,Tc,true>
    {
        inline AddMultMMHelper(const T alpha, const Ta* A, const Tb* B, Tc* C)
        { // Sa == ColMajor && Sc == ColMajor
            for(int j=0;j<N;++j) 
                for(int k=0;k<K;++k) {
                    typedef ProductType(T,Tb) PT;
                    PT bkj = alpha * Bkj;
                    AddVVHelper<M,PT,Ta,Tc,true>(bkj,A+k*M,C+j*M);
                }
        }
    };
    template <int M, int N, int K, StorageType Sa, class T, class Ta, class Tb, class Tc>
    struct AddMultMMHelper<M,N,K,Sa,RowMajor,RowMajor,T,Ta,Tb,Tc,true>
    {
        inline AddMultMMHelper(const T alpha, const Ta* A, const Tb* B, Tc* C)
        { // Sb == RowMajor && Sc == RowMajor
            for(int i=0;i<M;++i) for(int k=0;k<K;++k) {
                typedef ProductType(T,Ta) PT;
                PT aik = alpha * Aik;
                AddVVHelper<N,PT,Tb,Tc,true>(aik,B+k*N,C+i*N);
            }
        }
    };
    template <int M, int N, int K, StorageType Sa, StorageType Sb, StorageType Sc, class T, class Ta, class Tb, class Tc>
    inline void AddMultMM(const T alpha, const Ta* A, const Tb* B, Tc* C)
    { AddMultMMHelper<M,N,K,Sa,Sb,Sc,T,Ta,Tb,Tc,OKTypes3(T,Ta,Tb,Tc)>(alpha,A,B,C); }
#undef Aik
#undef Bkj
#undef Cij

    // A = alpha * x * yT
    template <int M, int N, StorageType S, class Tx, class Ty, class Ta, bool ok>
    struct Rank1Update_1Helper
    { Rank1Update_1Helper(const Tx*, const Ty*, Ta*) {} };
    template <int M, int N, class Tx, class Ty, class Ta>
    struct Rank1Update_1Helper<M,N,ColMajor,Tx,Ty,Ta,true>
    {
        Rank1Update_1Helper(const Tx* x, const Ty* y, Ta* A)
        { // S == ColMajor
            for(int j=0;j<N;++j) 
                MultXVHelper2<M,Ty,Tx,Ta,true>(y[j],x,A+j*M);
        }
    };
    template <int M, int N, class Tx, class Ty, class Ta>
    struct Rank1Update_1Helper<M,N,RowMajor,Tx,Ty,Ta,true>
    {
        Rank1Update_1Helper(const Tx* x, const Ty* y, Ta* A)
        { // S == RowMajor
            for(int i=0;i<M;++i) 
                MultXVHelper2<N,Tx,Ty,Ta,true>(x[i],y,A+i*N);
        }
    };
    template <int M, int N, StorageType S, class Tx, class Ty, class Ta>
    inline void Rank1Update_1(const Tx* x, const Ty* y, Ta* A)
    { Rank1Update_1Helper<M,N,S,Tx,Ty,Ta,OKTypes2(Tx,Ty,Ta)>(x,y,A); }

    template <int M, int N, StorageType S, class T, class Tx, class Ty, class Ta, bool ok>
    struct Rank1UpdateHelper
    { Rank1UpdateHelper(const T, const Tx*, const Ty*, Ta*) {} };
    template <int M, int N, class T, class Tx, class Ty, class Ta>
    struct Rank1UpdateHelper<M,N,ColMajor,T,Tx,Ty,Ta,true>
    {
        Rank1UpdateHelper(const T alpha, const Tx* x, const Ty* y, Ta* A)
        { // S == ColMajor
            typedef ProductType(T,Ty) PT;
            for(int j=0;j<N;++j) 
                MultXVHelper2<M,PT,Tx,Ta,true>(alpha*y[j],x,A+j*M);
        }
    };
    template <int M, int N, class T, class Tx, class Ty, class Ta>
    struct Rank1UpdateHelper<M,N,RowMajor,T,Tx,Ty,Ta,true>
    {
        Rank1UpdateHelper(const T alpha, const Tx* x, const Ty* y, Ta* A)
        { // S == RowMajor
            typedef ProductType(T,Tx) PT;
            for(int i=0;i<M;++i) 
                MultXVHelper2<N,PT,Ty,Ta,true>(alpha*x[i],y,A+i*N);
        }
    };
    template <int M, int N, StorageType S, class T, class Tx, class Ty, class Ta>
    inline void Rank1Update(const T alpha, const Tx* x, const Ty* y, Ta* A)
    { Rank1UpdateHelper<M,N,S,T,Tx,Ty,Ta,OKTypes3(T,Tx,Ty,Ta)>(alpha,x,y,A); }

    // A += alpha * x * yT
    template <int M, int N, StorageType S, class Tx, class Ty, class Ta, bool ok>
    struct AddRank1Update_1Helper
    { AddRank1Update_1Helper(const Tx*, const Ty*, Ta*) {} };
    template <int M, int N, class Tx, class Ty, class Ta>
    struct AddRank1Update_1Helper<M,N,ColMajor,Tx,Ty,Ta,true>
    {
        AddRank1Update_1Helper(const Tx* x, const Ty* y, Ta* A)
        { // S == ColMajor
            for(int j=0;j<N;++j) 
                AddVVHelper<M,Ty,Tx,Ta,true>(y[j],x,A+j*M);
        }
    };
    template <int M, int N, class Tx, class Ty, class Ta>
    struct AddRank1Update_1Helper<M,N,RowMajor,Tx,Ty,Ta,true>
    {
        AddRank1Update_1Helper(const Tx* x, const Ty* y, Ta* A)
        { // S == RowMajor
            for(int i=0;i<M;++i) 
                AddVVHelper<N,Tx,Ty,Ta,true>(x[i],y,A+i*N);
        }
    };
    template <int M, int N, StorageType S, class Tx, class Ty, class Ta>
    inline void AddRank1Update_1(const Tx* x, const Ty* y, Ta* A)
    { AddRank1Update_1Helper<M,N,S,Tx,Ty,Ta,OKTypes2(Tx,Ty,Ta)>(x,y,A); }

    template <int M, int N, StorageType S, class Tx, class Ty, class Ta, bool ok>
    struct AddRank1Update_m1Helper
    { AddRank1Update_m1Helper(const Tx*, const Ty*, Ta*) {} };
    template <int M, int N, class Tx, class Ty, class Ta>
    struct AddRank1Update_m1Helper<M,N,ColMajor,Tx,Ty,Ta,true>
    {
        AddRank1Update_m1Helper(const Tx* x, const Ty* y, Ta* A)
        { // S == ColMajor
            for(int j=0;j<N;++j) 
                AddVVHelper<M,Ty,Tx,Ta,true>(-y[j],x,A+j*M);
        }
    };
    template <int M, int N, class Tx, class Ty, class Ta>
    struct AddRank1Update_m1Helper<M,N,RowMajor,Tx,Ty,Ta,true>
    {
        AddRank1Update_m1Helper(const Tx* x, const Ty* y, Ta* A)
        { // S == RowMajor
            for(int i=0;i<M;++i)
                AddVVHelper<N,Tx,Ty,Ta,true>(-x[i],y,A+i*N);
        }
    };
    template <int M, int N, StorageType S, class Tx, class Ty, class Ta>
    inline void AddRank1Update_m1(const Tx* x, const Ty* y, Ta* A)
    { AddRank1Update_m1Helper<M,N,S,Tx,Ty,Ta,OKTypes2(Tx,Ty,Ta)>(x,y,A); }

    template <int M, int N, StorageType S, class T, class Tx, class Ty, class Ta, bool ok>
    struct AddRank1UpdateHelper
    { AddRank1UpdateHelper(const T, const Tx*, const Ty*, Ta*) {} };
    template <int M, int N, class T, class Tx, class Ty, class Ta>
    struct AddRank1UpdateHelper<M,N,ColMajor,T,Tx,Ty,Ta,true>
    {
        AddRank1UpdateHelper(const T alpha, const Tx* x, const Ty* y, Ta* A)
        { // S == ColMajor
            typedef ProductType(T,Ty) PT;
            for(int j=0;j<N;++j) 
                AddVVHelper<M,PT,Tx,Ta,true>(alpha*y[j],x,A+j*M);
        }
    };
    template <int M, int N, class T, class Tx, class Ty, class Ta>
    struct AddRank1UpdateHelper<M,N,RowMajor,T,Tx,Ty,Ta,true>
    {
        AddRank1UpdateHelper(const T alpha, const Tx* x, const Ty* y, Ta* A)
        { // S == RowMajor
            typedef ProductType(T,Tx) PT;
            for(int i=0;i<M;++i) 
                AddVVHelper<N,PT,Ty,Ta,true>(alpha*x[i],y,A+i*N);
        }
    };
    template <int M, int N, StorageType S, class T, class Tx, class Ty, class Ta>
    inline void AddRank1Update(const T alpha, const Tx* x, const Ty* y, Ta* A)
    { AddRank1UpdateHelper<M,N,S,T,Tx,Ty,Ta,OKTypes3(T,Tx,Ty,Ta)>(alpha,x,y,A); }

    template <class T, class Ta, class Tb, int M, int N, StorageType Sa, StorageType Sb> 
    inline void ElementProd(
        const T alpha, const SmallMatrix<Ta,M,N,Sa>& A, 
        SmallMatrix<Tb,M,N,Sb>& B)
    {
        if (Sa == Sb) {
            const Ta* Ap = A.cptr();
            Tb* Bp = B.ptr();
            for(int i=0;i<M*N;++i) Bp[i] *= alpha * Ap[i];
        } else {
            for(int i=0;i<M;++i) for(int j=0;j<N;j++) 
                B.ref(i,j) *= alpha * A.cref(i,j);
        }
    }

    template <class T, class Ta, class Tb, class Tc, int M, int N, StorageType Sa, StorageType Sb, StorageType Sc> 
    inline void AddElementProd(
        const T alpha, const SmallMatrix<Ta,M,N,Sa>& A,
        const SmallMatrix<Tb,M,N,Sb>& B, SmallMatrix<Tc,M,N,Sc>& C)
    {
        if (Sa == Sb && Sa == Sc) {
            const Ta* Ap = A.cptr();
            const Tb* Bp = B.cptr();
            Tc* Cp = C.ptr();
            for(int i=0;i<M*N;++i) Cp[i] += alpha * Ap[i] * Bp[i];
        } else {
            for(int i=0;i<M;++i) for(int j=0;j<N;j++) 
                C.ref(i,j) += alpha * A.cref(i,j) * B.cref(i,j);
        }
    }
    template <class T, class Ta, int M, int N, StorageType Sa, StorageType Sb> 
    inline void ElementProd(
        const CT , const SmallMatrix<Ta,M,N,Sa>& , SmallMatrix<T,M,N,Sb>& )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb, int M, int N, StorageType Sa, StorageType Sb, StorageType Sc> 
    inline void AddElementProd(
        const CT , const SmallMatrix<Ta,M,N,Sa>& ,
        const SmallMatrix<Tb,M,N,Sb>& , SmallMatrix<T,M,N,Sc>& )
    { TMVAssert(TMV_FALSE); }

#undef OKTypes
#undef OKTypes2
#undef OKTypes3
#undef ProductType

} // namespace tmv

#undef CT

#endif
