
#include "tmv/TMV_MultXM.h"
#include "tmv/TMV_Matrix.h"
#include "tmv/TMV_ProdXM.h"
#include "tmv/TMV_TransposeM.h"
#include "tmv/TMV_Vector.h"
#include "tmv/TMV_ScaleM.h"

namespace tmv {

    template <bool add, class T, class M1, class M2>
    static void DoMultXM(const T x, const M1& m1, M2& m2)
    {
        if (x == T(1))
            InlineMultXM<add>(Scaling<1,T>(x),m1,m2); 
        else if (x == T(-1))
            InlineMultXM<add>(Scaling<-1,T>(x),m1,m2); 
        else if (x == T(0))
            Maybe<!add>::zero(m2);
        else
            InlineMultXM<add>(Scaling<0,T>(x),m1,m2); 
    }

    template <bool add, class T, class M1, class M2>
    static void DoMultXM(const std::complex<T> x, const M1& m1, M2& m2)
    { 
        if (imag(x) == T(0)) {
            if (real(x) == T(1))
                InlineMultXM<add>(Scaling<1,T>(real(x)),m1,m2); 
            else if (real(x) == T(-1))
                InlineMultXM<add>(Scaling<-1,T>(real(x)),m1,m2); 
            else if (real(x) == T(0))
                Maybe<!add>::zero(m2);
            else
                InlineMultXM<add>(Scaling<0,T>(real(x)),m1,m2); 
        } else 
            InlineMultXM<add>(Scaling<0,std::complex<T> >(x),m1,m2); 
    }

    template <class T1, int C1, class T2>
    void InstMultXM(
        const T2 x, const ConstMatrixView<T1,C1>& m1, MatrixView<T2> m2)
    {
        if (m1.iscm() && m2.iscm()) {
            MatrixView<T2,ColMajor> m2cm = m2.cmView();
            DoMultXM<false>(x,m1.cmView(),m2cm);
        } else if (m1.isrm() && m2.isrm()) {
            MatrixView<T2,ColMajor> m2t = m2.transpose().cmView();
            DoMultXM<false>(x,m1.transpose().cmView(),m2t);
        } else {
            InstCopy(m1,m2);
            InstScale(x,m2);
        }
    }

    template <class T1, int C1, class T2>
    void InstAddMultXM(
        const T2 x, const ConstMatrixView<T1,C1>& m1, MatrixView<T2> m2)
    {
        if (m1.iscm() && m2.iscm()) {
            MatrixView<T2,ColMajor> m2cm = m2.cmView();
            DoMultXM<true>(x,m1.cmView(),m2cm);
        } else if (m1.isrm() && m2.isrm()) {
            MatrixView<T2,ColMajor> m2t = m2.transpose().cmView();
            DoMultXM<true>(x,m1.transpose().cmView(),m2t);
        } else {
            DoMultXM<true>(x,m1,m2);
        }
    }

    template <class T1, int C1, class T2>
    void InstAliasMultXM(
        const T2 x, const ConstMatrixView<T1,C1>& m1, MatrixView<T2> m2)
    { InlineAliasMultXM<false>(Scaling<0,T2>(x),m1,m2); }
    template <class T1, int C1, class T2>
    void InstAliasAddMultXM(
        const T2 x, const ConstMatrixView<T1,C1>& m1, MatrixView<T2> m2)
    { InlineAliasMultXM<true>(Scaling<0,T2>(x),m1,m2); }

#define InstFile "TMV_MultXM.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace tmv


