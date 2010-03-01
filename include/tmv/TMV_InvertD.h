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


#ifndef TMV_InvertD_H
#define TMV_InvertD_H

#include "TMV_BaseMatrix_Diag.h"
#include "TMV_Scaling.h"


namespace tmv {

    // Defined below:
    template <class V>
    inline void ElemInvert(BaseVector_Mutable<V>& v);
    template <class V>
    inline void InlineElemInvert(BaseVector_Mutable<V>& v);

    // Defined in TMV_InvertD.cpp
    template <class T>
    void InstElemInvert(VectorView<T> v);

    //
    // DiagMatrix = DiagMatrix^-1
    //

    template <int algo, int size, class V>
    struct ElemInvert_Helper;

    // algo 0: size == 0, nothing to do
    template <class V>
    struct ElemInvert_Helper<0,0,V> 
    { 
        typedef typename V::iterator IT;
        static inline void call(V& ) {}
        static inline void call2(int, IT ) {}
    };

    // algo 11: simple for loop
    template <int size, class V>
    struct ElemInvert_Helper<11,size,V>
    {
        typedef typename V::iterator IT;
        static inline void call(V& v)
        {
            const int n = size == UNKNOWN ? int(v.size()) : size;
            call2(n,v.begin());
        }
        static void call2(int n, IT A)
        {
            typedef typename V::real_type RT;
            if (n) do {
                *A = ZProd<false,false>::quot(RT(1) , *A); ++A;
            } while (--n);
        }
    };

#ifdef __SSE__
    // algo 21: single precision SSE: v real
    // SSE also has a _mm_rcp_ps command that takes a reciprocal.
    // However, it is only 12 bit accurate, which is not acceptable,
    // so unfortunately we can't use it.
    // Instead, we do the reciprocal as 1 / x with _mm_div_ps.
    template <int size, class V>
    struct ElemInvert_Helper<21,size,V>
    {
        typedef typename V::iterator IT;
        static inline void call(V& v)
        {
            const int n = size == UNKNOWN ? int(v.size()) : size;
            call2(n,v.begin());
        }
        static void call2(int n, IT A)
        {
            const bool unit = V::vstep == 1;
            const typename V::real_type one(1);

            if (unit) {
                while (n && (((unsigned int)(A.getP()) & 0xf) != 0) ) {
                    *A = one / *A;
                    ++A; --n;
                }
            }

            int n_4 = (n>>2);
            int nb = n-(n_4<<2);
            
            if (n_4) {
                IT A1 = A+1;
                IT A2 = A+2;
                IT A3 = A+3;

                __m128 xone = _mm_set1_ps(1.F);
                __m128 xA,xB;
                do {
                    Maybe<unit>::sse_load(
                        xA,A.getP(),A1.getP(),A2.getP(),A3.getP());
                    xB = _mm_div_ps(xone,xA);
                    Maybe<unit>::sse_store(
                        A.getP(),A1.getP(),A2.getP(),A3.getP(),xB);
                    A+=4; A1+=4; A2+=4; A3+=4;
                } while (--n_4);
            }

            if (nb) do { *A = one / *A; ++A; } while (--nb);
        }
    };

    // algo 23: single precision SSE: v complex
    template <int size, class V>
    struct ElemInvert_Helper<23,size,V>
    {
        typedef typename V::iterator IT;
        static inline void call(V& v)
        {
            const int n = size == UNKNOWN ? int(v.size()) : size;
            call2(n,v.begin());
        }
        static void call2(int n, IT A)
        {
            const bool unit = V::vstep == 1;
            const typename V::real_type one(1);

            if (unit) {
                while (n && (((unsigned int)(A.getP()) & 0xf) != 0) ) {
                    *A = ZProd<false,false>::quot(one , *A);
                    ++A; --n;
                }
            }

            int n_2 = (n>>1);
            int nb = n-(n_2<<1);
            
            if (n_2) {
                IT A1 = A+1;

                // These look backwards, but order is from hi to lo values.
                __m128 xmone = _mm_set_ps(-1, 1, -1, 1);
                __m128 xA, xB;
                __m128 xAc, xnorm, x1, x2; // temp values
                do {
                    Maybe<unit>::sse_load(xA,A.getP(),A1.getP());
                    xAc = _mm_mul_ps(xmone,xA); // conj(xA)
                    x1 = _mm_mul_ps(xA,xA);
                    x2 = _mm_shuffle_ps(x1,x1,_MM_SHUFFLE(2,3,0,1));
                    xnorm = _mm_add_ps(x1,x2); // = norm(xA)
                    xB = _mm_div_ps(xAc,xnorm);  // = 1/xA
                    Maybe<unit>::sse_store(A.getP(),A1.getP(),xB);
                    A+=2; A1+=2;
                } while (--n_2);
            }

            if (nb) *A = ZProd<false,false>::quot(one , *A);
        }
    };
#endif

#ifdef __SSE2__
    // algo 31: double precision SSE2: v real
    template <int size, class V>
    struct ElemInvert_Helper<31,size,V>
    {
        typedef typename V::iterator IT;
        static inline void call(V& v)
        {
            const int n = size == UNKNOWN ? int(v.size()) : size;
            call2(n,v.begin());
        }
        static void call2(int n, IT A)
        {
            const bool unit = V::vstep == 1;
            const typename V::real_type one(1);

            if (unit) {
                while (n && (((unsigned int)(A.getP()) & 0xf) != 0) ) {
                    *A = one / *A;
                    ++A; --n;
                }
            }

            int n_2 = (n>>1);
            int nb = n-(n_2<<1);
            
            if (n_2) {
                IT A1 = A+1;

                __m128d xone = _mm_set1_pd(1.);
                __m128d xA,xB;
                do {
                    Maybe<unit>::sse_load(xA,A.getP(),A1.getP());
                    xB = _mm_div_pd(xone,xA);
                    Maybe<unit>::sse_store(A.getP(),A1.getP(),xB);
                    A+=2; A1+=2;
                } while (--n_2);
            }

            if (nb) *A = one / *A;
        }
    };

    // algo 33: double precision SSE2: v complex
    template <int size, class V>
    struct ElemInvert_Helper<33,size,V>
    {
        typedef typename V::iterator IT;
        static inline void call(V& v)
        {
            const int n = size == UNKNOWN ? int(v.size()) : size;
            call2(n,v.begin());
        }
        static void call2(int n, IT A)
        {
            if (n) {
                // These look backwards, but order is from hi to lo values.
                __m128d xmone = _mm_set_pd(-1 , 1);
                __m128d xA,xB;
                __m128d xAc, xnorm, x1, x2; // temp values
                if (((unsigned int)(A.getP()) & 0xf) == 0) {
                    do {
                        Maybe<true>::sse_load(xA,A.getP());
                        xAc = _mm_mul_pd(xmone,xA); // conj(xA)
                        x1 = _mm_mul_pd(xA,xA);
                        x2 = _mm_shuffle_pd(x1,x1,_MM_SHUFFLE2(0,1));
                        xnorm = _mm_add_pd(x1,x2); // = norm(xA)
                        xB = _mm_div_pd(xAc,xnorm);  // = 1/xA
                        Maybe<true>::sse_store(A.getP(),xB); ++A;
                    } while (--n);
                } else {
                    do {
                        Maybe<true>::sse_loadu(xA,A.getP());
                        xAc = _mm_mul_pd(xmone,xA); // conj(xA)
                        x1 = _mm_mul_pd(xA,xA);
                        x2 = _mm_shuffle_pd(x1,x1,_MM_SHUFFLE2(0,1));
                        xnorm = _mm_add_pd(x1,x2); // = norm(xA)
                        xB = _mm_div_pd(xAc,xnorm);  // = 1/xA
                        Maybe<true>::sse_storeu(A.getP(),xB); ++A;
                    } while (--n);
                }
            }
        }
    };
#endif

    // algo -4: No branches or copies
    template <int size, class V>
    struct ElemInvert_Helper<-4,size,V>
    {
        typedef typename V::iterator IT;
        typedef typename V::real_type RT;
        enum { vfloat = Traits2<RT,float>::sametype };
        enum { vdouble = Traits2<RT,double>::sametype };
        enum { vreal = V::visreal };
        enum { vcomplex = V::viscomplex };
        enum { unit = V::vstep == 1 };

        enum { algo = (
                size == 0 ? 0 : 
#if TMV_OPT >= 1
#ifdef __SSE__
                ( vfloat && vreal && unit ) ? 21 :
                ( vfloat && vcomplex ) ? 23 :
#endif
#ifdef __SSE2__
                ( vdouble && vreal && unit ) ? 31 :
                ( vdouble && vcomplex ) ? 33 :
#endif
#endif
                11 ) };
        static inline void call(V& v)
        {
            TMVStaticAssert(!V::vconj);
#ifdef PRINTALGO_InvD
            std::cout<<"InlineElemInvert:  \n";
            std::cout<<"v = "<<TMV_Text(v)<<std::endl;
            std::cout<<"size = "<<size<<" = "<<v.size()<<std::endl;
            std::cout<<"algo = "<<algo<<std::endl;
            std::cout<<"v = "<<v<<std::endl;
#endif
            ElemInvert_Helper<algo,size,V>::call(v); 
#ifdef PRINTALGO_InvD
            std::cout<<"v => "<<v<<std::endl;
#endif
        }
        static inline void call2(int n, IT A)
        {
            TMVStaticAssert(!V::vconj);
            ElemInvert_Helper<algo,size,V>::call2(n,A); 
        }
    };

    // algo -3: Determine which algorithm to use
    template <int size, class V>
    struct ElemInvert_Helper<-3,size,V> 
    {
        static inline void call(V& v)
        { ElemInvert_Helper<-4,size,V>::call(v); }
    };

    // algo 97: Conjugate
    template <int size, class V>
    struct ElemInvert_Helper<97,size,V>
    {
        static inline void call(V& v)
        { 
            typedef typename V::conjugate_type Vc;
            Vc vc = v.conjugate();
            ElemInvert_Helper<-2,size,Vc>::call(vc);
        }
    };

    // algo 98: Call inst
    template <int size, class V>
    struct ElemInvert_Helper<98,size,V>
    {
        static inline void call(V& v)
        { InstElemInvert(v.xView()); }
    };

    // algo -2: Check for inst
    template <int size, class V>
    struct ElemInvert_Helper<-2,size,V>
    {
        static inline void call(V& v)
        {
            typedef typename V::value_type T;
            const bool inst = 
                V::vsize == UNKNOWN &&
                Traits<T>::isinst;
            const bool conj = V::vconj;
            const int algo = 
                size == 0 ? 0 : 
                conj ? 97 :
                inst ? 98 :
                -4;
            ElemInvert_Helper<algo,size,V>::call(v);
        }
    };

    // algo -1: Check for aliases? No.
    template <int size, class V>
    struct ElemInvert_Helper<-1,size,V>
    {
        static inline void call(V& v)
        { ElemInvert_Helper<-2,size,V>::call(v); }
    };

    //
    // Element-wise v(i) = 1/v(i)
    //

    template <int algo, class V>
    inline void DoElemInvert(BaseVector_Mutable<V>& v)
    {
        typedef typename V::cview_type Vv;
        Vv vv = v.cView();
        ElemInvert_Helper<algo,V::vsize,Vv>::call(vv);
    }

    template <class V>
    inline void ElemInvert(BaseVector_Mutable<V>& v)
    { DoElemInvert<-2>(v); }

    template <class V>
    inline void InlineElemInvert(BaseVector_Mutable<V>& v)
    { DoElemInvert<-3>(v); }

} // namespace tmv

#endif 
