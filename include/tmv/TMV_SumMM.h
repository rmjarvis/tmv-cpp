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


#ifndef TMV_SumMM_H
#define TMV_SumMM_H

#include "TMV_ProdXM.h"
#include "TMV_SumVV.h"

namespace tmv {

    //
    // Matrix + Matrix
    //

#if 0
    // Default unless overridden by specialized version
    template <int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    inline void InlineAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Calc<M1>& m1, 
        const Scaling<ix2,T2>& x2, const BaseMatrix_Calc<M2>& m2, 
        BaseMatrix_Mutable<M3>& m3)
    { (m3 = x1*m1) += (x2*m2); }

    template <bool checkalias, bool conj, bool rm, bool inst, 
              int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    struct CallAddMM2;
    template <bool conj, bool rm, bool inst, int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    struct CallAddMM2<true,conj,rm,inst,ix1,T1,M1,ix2,T2,M2,M3> // checkalias
    {
        static inline void call(
            const Scaling<ix1,T1>& x1, const M1& m1,
            const Scaling<ix2,T2>& x2, const M2& m2, M3& m3)
        {
            if (!SameStorage(m1,m3) && !SameStorage(m2,m3)) {
                CallAddMM2<false,conj,rm,inst,ix1,T1,M1,ix2,T2,M2,M3>::call(
                    x1,m1,x2,m2,m3);
            } else if (!SameStorage(m2,m3)) { // alias with m1 only
                AliasMultXM<false>(x1,m1,m3);
                NoAliasMultXM<true>(x2,m2,m3);
            } else if (!SameStorage(m1,m3)) { // alias with m2 only
                AliasMultXM<false>(x2,m2,m3);
                NoAliasMultXM<true>(x1,m1,m3);
            } else { // alias with both
                typename M1::copy_type m1c = m1;
                AliasMultXM<false>(x2,m2,m3);
                NoAliasMultXM<true>(x1,m1c,m3);
            }
        }
    };
    template <bool rm, bool inst, int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    struct CallAddMM2<false,true,rm,inst,ix1,T1,M1,ix2,T2,M2,M3> // conj = true
    {
        static inline void call(
            const Scaling<ix1,T1>& x1, const M1& m1,
            const Scaling<ix2,T2>& x2, const M2& m2, M3& m3)
        { 
            typedef typename M1::const_conjugate_type M1c;
            typedef typename M2::const_conjugate_type M2c;
            typedef typename M3::conjugate_type M3c;
            M1c m1c = m1.conjugate();
            M2c m2c = m2.conjugate();
            M3c m3c = m3.conjugate();
            CallAddMM2<false,false,rm,inst,ix1,T1,M1c,ix2,T2,M2c,M3c>::call(
                TMV_CONJ(x1),m1c,TMV_CONJ(x2),m2c,m3c);
        }
    };
    template <bool inst, int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    struct CallAddMM2<false,false,true,inst,ix1,T1,M1,ix2,T2,M2,M3> 
    // rm = true
    {
        static inline void call(
            const Scaling<ix1,T1>& x1, const M1& m1,
            const Scaling<ix2,T2>& x2, const M2& m2, M3& m3)
        { 
            typedef typename M1::const_transpose_type M1t;
            typedef typename M2::const_transpose_type M2t;
            typedef typename M3::transpose_type M3t;
            M1t m1t = m1.transpose();
            M2t m2t = m2.transpose();
            M3t m3t = m3.transpose();
            CallAddMM2<false,false,false,inst,ix1,T1,M1t,ix2,T2,M2t,M3t>::call(
                x1,m1t,x2,m2t,m3t);
        }
    };
    template <int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    struct CallAddMM2<false,false,false,true,ix1,T1,M1,ix2,T2,M2,M3> 
    // inst = true
    {
        static inline void call(
            const Scaling<ix1,T1>& x1, const M1& m1,
            const Scaling<ix2,T2>& x2, const M2& m2, M3& m3)
        {
            typename M3::value_type xx1(x1);
            typename M3::value_type xx2(x2);
            // Rather than have an Inst version for this,
            // just do it in two steps.
            InstMultXM(xx1,m1.xView(),m3.xView());
            InstAddMultXM(xx2,m2.xView(),m3.xView());
        }
    };
    template <int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    struct CallAddMM2<false,false,false,false,ix1,T1,M1,ix2,T2,M2,M3> 
    // inst = false
    {
        static inline void call(
            const Scaling<ix1,T1>& x1, const M1& m1,
            const Scaling<ix2,T2>& x2, const M2& m2, M3& m3)
        { InlineAddMM(x1,m1,x2,m2,m3); }
    };

    template <int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    inline void AddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Calc<M1>& m1, 
        const Scaling<ix2,T2>& x2, const BaseMatrix_Calc<M2>& m2, 
        BaseMatrix_Mutable<M3>& m3)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same));
        TMVStaticAssert((Sizes<M1::mcolsize,M3::mcolsize>::same));
        TMVStaticAssert((Sizes<M2::mcolsize,M3::mcolsize>::same));
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same));
        TMVStaticAssert((Sizes<M1::mrowsize,M3::mrowsize>::same));
        TMVStaticAssert((Sizes<M2::mrowsize,M3::mrowsize>::same));
        TMVAssert(m1.colsize() == m3.colsize());
        TMVAssert(m2.colsize() == m3.colsize());
        TMVAssert(m1.rowsize() == m3.rowsize());
        TMVAssert(m2.rowsize() == m3.rowsize());

        typedef typename M1::value_type TM1;
        typedef typename M2::value_type TM2;
        typedef typename M3::value_type TM3;
        const bool conj = M3::mconj;
        const bool rm = M3::mrowmajor && !M3::mcolmajor;
        const bool checkalias = 
                M1::mcolsize == UNKNOWN && M1::mrowsize == UNKNOWN &&
                M2::mcolsize == UNKNOWN && M2::mrowsize == UNKNOWN &&
                M3::mcolsize == UNKNOWN && M3::mrowsize == UNKNOWN;
        const bool inst = 
                checkalias &&
#ifdef TMV_INST_MIX
                Traits2<TM1,TM3>::samebase &&
                Traits2<TM2,TM3>::samebase &&
#else
                Traits2<TM1,TM3>::sametype &&
                Traits2<TM2,TM3>::sametype &&
#endif
                Traits<TM3>::isinst;
        CallAddMM2<checkalias,conj,rm,inst,ix1,T1,M1,ix2,T2,M2,M3>::call(
            x1,m1.mat(),x2,m2.mat(),m3.mat());
    }

    template <int ix1, class T1, class M1, 
              int ix2, class T2, class M2, class M3>
    inline void NoAliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Calc<M1>& m1, 
        const Scaling<ix2,T2>& x2, const BaseMatrix_Calc<M2>& m2, 
        BaseMatrix_Mutable<M3>& m3)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same));
        TMVStaticAssert((Sizes<M1::mcolsize,M3::mcolsize>::same));
        TMVStaticAssert((Sizes<M2::mcolsize,M3::mcolsize>::same));
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same));
        TMVStaticAssert((Sizes<M1::mrowsize,M3::mrowsize>::same));
        TMVStaticAssert((Sizes<M2::mrowsize,M3::mrowsize>::same));
        TMVAssert(m1.colsize() == m3.colsize());
        TMVAssert(m2.colsize() == m3.colsize());
        TMVAssert(m1.rowsize() == m3.rowsize());
        TMVAssert(m2.rowsize() == m3.rowsize());

        typedef typename M1::value_type TM1;
        typedef typename M2::value_type TM2;
        typedef typename M3::value_type TM3;
        const bool conj = M3::mconj;
        const bool rm = M3::mrowmajor && !M3::mcolmajor;
        const bool inst = 
                M1::mcolsize == UNKNOWN && M1::mrowsize == UNKNOWN &&
                M2::mcolsize == UNKNOWN && M2::mrowsize == UNKNOWN &&
                M3::mcolsize == UNKNOWN && M3::mrowsize == UNKNOWN &&
#ifdef TMV_INST_MIX
                Traits2<TM1,TM3>::samebase &&
                Traits2<TM2,TM3>::samebase &&
#else
                Traits2<TM1,TM3>::sametype &&
                Traits2<TM2,TM3>::sametype &&
#endif
                Traits<TM3>::isinst;
        CallAddMM2<false,conj,rm,inst,ix1,T1,M1,ix2,T2,M2,M3>::call(
            x1,m1.mat(),x2,m2.mat(),m3.mat());
    }
#endif

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    class SumMM;

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    struct Traits<SumMM<ix1,T1,M1,ix2,T2,M2> >
    {
        typedef typename ProdXM<ix1,T1,M1>::value_type mtype1;
        typedef typename ProdXM<ix2,T2,M2>::value_type mtype2;
        typedef typename Traits2<mtype1,mtype2>::type value_type;

        enum { mcolsize = Sizes<M1::mcolsize,M2::mcolsize>::size };
        enum { mrowsize = Sizes<M1::mrowsize,M2::mrowsize>::size };
        enum { mshape = ShapeTraits2<M1::mshape,M2::mshape>::sum };
        enum { mfort = M1::mfort && M2::mfort };
        enum { mcalc = false };
        enum { rm1 = Traits<typename M1::calc_type>::mrowmajor };
        enum { rm2 = Traits<typename M2::calc_type>::mrowmajor };
        enum { cm1 = Traits<typename M1::calc_type>::mcolmajor };
        enum { mrowmajor = ( rm1 || (rm2 && !cm1) ) };

        typedef SumMM<ix1,T1,M1,ix2,T2,M2> type;
        typedef typename MCopyHelper<value_type,mshape,mcolsize,mrowsize,
                mrowmajor,mfort>::type copy_type;
        typedef const copy_type calc_type;
        typedef typename TypeSelect<
            (M1::mcalc && M2::mcalc),const type,calc_type>::type eval_type;
        typedef InvalidType inverse_type;
    };

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    class SumMM : public BaseMatrix<SumMM<ix1,T1,M1,ix2,T2,M2> >
    {
    public:

        typedef SumMM<ix1,T1,M1,ix2,T2,M2> type;
        typedef typename Traits<type>::value_type value_type;

        typedef typename Traits<value_type>::real_type real_type;
        typedef typename Traits<value_type>::complex_type complex_type;

        inline SumMM(
            const T1& _x1, const BaseMatrix<M1>& _m1, 
            const T2& _x2, const BaseMatrix<M2>& _m2) :
            x1(_x1), m1(_m1.mat()), x2(_x2), m2(_m2.mat())
        {
            TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
            TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
            TMVAssert(m1.colsize() == m2.colsize());
            TMVAssert(m1.rowsize() == m2.rowsize());
        }

        inline const Scaling<ix1,T1>& getX1() const { return x1; }
        inline const M1& getM1() const { return m1; }
        inline const Scaling<ix2,T2>& getX2() const { return x2; }
        inline const M2& getM2() const { return m2; }

        inline size_t colsize() const { return m1.colsize(); }
        inline size_t rowsize() const { return m1.rowsize(); }

        inline value_type cref(int i, int j) const
        { return x1 * m1.cref(i,j) + x2 * m2.cref(i,j); }

        template <class M3>
        inline void assignTo(BaseMatrix_Mutable<M3>& m3) const
        {
            TMVStaticAssert((
                    ShapeTraits2<type::mshape,M3::mshape>::assignable)); 
            TMVStaticAssert((type::misreal || M3::miscomplex));
            TMVStaticAssert((Sizes<type::mcolsize,M3::mcolsize>::same)); 
            TMVStaticAssert((Sizes<type::mrowsize,M3::mrowsize>::same)); 
            TMVAssert(colsize() == m3.colsize());
            TMVAssert(rowsize() == m3.rowsize());
            AddMM(x1,m1.calc(),x2,m2.calc(),m3.mat());
        }

        template <class M3>
        inline void newAssignTo(BaseMatrix_Mutable<M3>& m3) const
        {
            TMVStaticAssert((
                    ShapeTraits2<type::mshape,M3::mshape>::assignable)); 
            TMVStaticAssert((type::misreal || M3::miscomplex));
            TMVStaticAssert((Sizes<type::mcolsize,M3::mcolsize>::same)); 
            TMVStaticAssert((Sizes<type::mrowsize,M3::mrowsize>::same)); 
            TMVAssert(colsize() == m3.colsize());
            TMVAssert(rowsize() == m3.rowsize());
            NoAliasAddMM(x1,m1.calc(),x2,m2.calc(),m3.mat());
        }

    private:
        const Scaling<ix1,T1> x1;
        const M1& m1;
        const Scaling<ix2,T2> x2;
        const M2& m2;
    };

#define RT typename M2::real_type
    // m += m
    template <class M1, class M2>
    inline void AddEq(
        BaseMatrix_Mutable<M1>& m1, const BaseMatrix<M2>& m2) 
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        TMVStaticAssert(M1::miscomplex || M2::misreal);
        MultXM<true>(m2.calc(),m1.mat());
    }

    // m += xm
    template <class M1, int ix2, class T2, class M2>
    inline void AddEq(
        BaseMatrix_Mutable<M1>& m1, const ProdXM<ix2,T2,M2>& m2) 
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        TMVStaticAssert(M1::miscomplex || M2::misreal);
        MultXM<true>(m2.getX(),m2.getM().calc(),m1.mat());
    }

    // m -= m
    template <class M1, class M2>
    inline void SubtractEq(
        BaseMatrix_Mutable<M1>& m1, const BaseMatrix<M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        TMVStaticAssert(M1::miscomplex || M2::misreal);
        MultXM<true>(Scaling<-1,RT>(),m2.calc(),m1.mat());
    }

    // m -= xm
    template <class M1, int ix2, class T2, class M2>
    inline void SubtractEq(
        BaseMatrix_Mutable<M1>& m1, const ProdXM<ix2,T2,M2>& m2) 
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        TMVStaticAssert(M1::miscomplex || M2::misreal);
        MultXM<true>(-m2.getX(),m2.getM().calc(),m1.mat());
    }

    // m + m
    template <class M1, class M2>
    inline SumMM<1,RT,M1,1,RT,M2> operator+(
        const BaseMatrix<M1>& m1, const BaseMatrix<M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<1,RT,M1,1,RT,M2>(RT(1),m1,RT(1),m2); 
    }

    // xm + m
    template <int ix1, class T1, class M1, class M2>
    inline SumMM<ix1,T1,M1,1,RT,M2> operator+(
        const ProdXM<ix1,T1,M1>& m1, const BaseMatrix<M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<ix1,T1,M1,1,RT,M2>(T1(m1.getX()),m1.getM(),RT(1),m2); 
    }

    // m + xm
    template <class M1, int ix2, class T2, class M2>
    inline SumMM<1,RT,M1,ix2,T2,M2> operator+(
        const BaseMatrix<M1>& m1, const ProdXM<ix2,T2,M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<1,RT,M1,ix2,T2,M2>(RT(1),m1,T2(m2.getX()),m2.getM()); 
    }

    // xm + xm
    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<ix1,T1,M1,ix2,T2,M2> operator+(
        const ProdXM<ix1,T1,M1>& m1, const ProdXM<ix2,T2,M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<ix1,T1,M1,ix2,T2,M2>(
            T1(m1.getX()),m1.getM(),T2(m2.getX()),m2.getM()); 
    }

    // m - m
    template <class M1, class M2>
    inline SumMM<1,RT,M1,-1,RT,M2> operator-(
        const BaseMatrix<M1>& m1, const BaseMatrix<M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<1,RT,M1,-1,RT,M2>(RT(1),m1,RT(-1),m2); 
    }

    // xm - m
    template <int ix1, class T1, class M1, class M2>
    inline SumMM<ix1,T1,M1,-1,RT,M2> operator-(
        const ProdXM<ix1,T1,M1>& m1, const BaseMatrix<M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<ix1,T1,M1,-1,RT,M2>(T1(m1.getX()),m1.getM(),RT(-1),m2); 
    }

    // m - xm
    template <class M1, int ix2, class T2, class M2>
    inline SumMM<1,RT,M1,-ix2,T2,M2> operator-(
        const BaseMatrix<M1>& m1, const ProdXM<ix2,T2,M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<1,RT,M1,-ix2,T2,M2>(RT(1),m1,-T2(m2.getX()),m2.getM()); 
    }

    // xm - xm
    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<ix1,T1,M1,-ix2,T2,M2> operator-(
        const ProdXM<ix1,T1,M1>& m1, const ProdXM<ix2,T2,M2>& m2)
    {
        TMVStaticAssert((Sizes<M1::mcolsize,M2::mcolsize>::same)); 
        TMVStaticAssert((Sizes<M1::mrowsize,M2::mrowsize>::same)); 
        TMVAssert(m1.colsize() == m2.colsize());
        TMVAssert(m1.rowsize() == m2.rowsize());
        return SumMM<ix1,T1,M1,-ix2,T2,M2>(
            T1(m1.getX()),m1.getM(),-T2(m2.getX()),m2.getM()); 
    }
#undef RT


    // Consolidate x*(xm+xm) type constructs:

#define RT typename SumMM<ix1,T1,M1,ix2,T2,M2>::real_type
#define CT typename SumMM<ix1,T1,M1,ix2,T2,M2>::complex_type
#define CCT ConjRef<CT>
#define TX1 typename Traits2<T,T1>::type
#define TX2 typename Traits2<T,T2>::type

    // -(xm+xm)
    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<-ix1,T1,M1,-ix2,T2,M2> operator-(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        return SumMM<-ix1,T1,M1,-ix2,T2,M2>(
            -T1(smm.getX1()),smm.getM1(),-T2(smm.getX2()),smm.getM2());
    }

    // x * (xm+xm)
    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,T1,M1,0,T2,M2> operator*(
        const int x, const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        return SumMM<0,T1,M1,0,T2,M2>(
            RT(x)*smm.getX1(),smm.getM1(), RT(x)*smm.getX2(),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,T1,M1,0,T2,M2> operator*(
        const RT x, const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        return SumMM<0,T1,M1,0,T2,M2>(
            x*smm.getX1(),smm.getM1(), x*smm.getX2(),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,CT,M1,0,CT,M2> operator*(
        const CT x, const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        return SumMM<0,CT,M1,0,CT,M2>(
            x*smm.getX1(),smm.getM1(), x*smm.getX2(),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,CT,M1,0,CT,M2> operator*(
        const CCT x, const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        return SumMM<0,CT,M1,0,CT,M2>(
            CT(x)*smm.getX1(),smm.getM1(), CT(x)*smm.getX2(),smm.getM2());
    }
    template <int ix, class T, int ix1, class T1, class M1, 
              int ix2, class T2, class M2>
    inline SumMM<ix1*ix,TX1,M1,ix2*ix,TX2,M2> operator*(
        const Scaling<ix,T>& x, const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        return SumMM<ix1*ix,TX1,M1,ix2*ix,TX2,M2>(
            T(x)*smm.getX1(),smm.getM1(),T(x)*smm.getX2(),smm.getM2());
    }

    // (xm+xm)*x
    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,T1,M1,0,T2,M2> operator*(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const int x)
    {
        return SumMM<0,T1,M1,0,T2,M2>(
            RT(x)*smm.getX1(),smm.getM1(), RT(x)*smm.getX2(),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,T1,M1,0,T2,M2> operator*(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const RT x)
    {
        return SumMM<0,T1,M1,0,T2,M2>(
            x*smm.getX1(),smm.getM1(), x*smm.getX2(),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,CT,M1,0,CT,M2> operator*(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const CT x)
    {
        return SumMM<0,CT,M1,0,CT,M2>(
            x*smm.getX1(),smm.getM1(), x*smm.getX2(),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,CT,M1,0,CT,M2> operator*(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const CCT x)
    {
        return SumMM<0,CT,M1,0,CT,M2>(
            CT(x)*smm.getX1(),smm.getM1(), CT(x)*smm.getX2(),smm.getM2());
    }

    template <int ix, class T, int ix1, class T1, class M1, 
              int ix2, class T2, class M2>
    inline SumMM<ix1*ix,TX1,M1,ix2*ix,TX2,M2> operator*(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const Scaling<ix,T>& x)
    {
        return SumMM<ix1*ix,TX1,M1,ix2*ix,TX2,M2>(
            T(x)*smm.getX1(),smm.getM1(),T(x)*smm.getX2(),smm.getM2());
    }

    // (xm+xm)/x
    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,T1,M1,0,T2,M2> operator/(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const int x)
    {
        return SumMM<0,T1,M1,0,T2,M2>(
            smm.getX1()/RT(x),smm.getM1(), smm.getX2()/RT(x),smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,T1,M1,0,T2,M2> operator/(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const RT x)
    {
        return SumMM<0,T1,M1,0,T2,M2>(
            smm.getX1()/x,smm.getM1(), smm.getX2()/x,smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,CT,M1,0,CT,M2> operator/(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const CT x)
    {
        return SumMM<0,CT,M1,0,CT,M2>(
            smm.getX1()/x,smm.getM1(), smm.getX2()/x,smm.getM2());
    }

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline SumMM<0,CT,M1,0,CT,M2> operator/(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const CCT x)
    {
        return SumMM<0,CT,M1,0,CT,M2>(
            smm.getX1()/CT(x),smm.getM1(), smm.getX2()/CT(x),smm.getM2());
    }

    template <int ix, class T, int ix1, class T1, class M1, 
              int ix2, class T2, class M2>
    inline SumMM<ix1*ix,TX1,M1,ix2*ix,TX2,M2> operator/(
        const SumMM<ix1,T1,M1,ix2,T2,M2>& smm, const Scaling<ix,T>& x)
    {
        return SumMM<ix1*ix,TX1,M1,ix2*ix,TX2,M2>(
            smm.getX1()/T(x),smm.getM1(),smm.getX2()/T(x),smm.getM2());
    }

#undef RT
#undef CT
#undef CCT
#undef TX1
#undef TX2



    // TMV_Text

    template <int ix1, class T1, class M1, int ix2, class T2, class M2>
    inline std::string TMV_Text(const SumMM<ix1,T1,M1,ix2,T2,M2>& smm)
    {
        std::ostringstream s;
        s << "SumMM< "<< ix1<<","<<TMV_Text(T1())
            <<" , "<<TMV_Text(smm.getM1())
            <<" , "<<ix2<<","<<TMV_Text(T2())
            <<" , "<<TMV_Text(smm.getM2())<<" >";
        return s.str();
    }



} // namespace tmv

#endif 
