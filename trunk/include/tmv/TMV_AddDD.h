

#ifndef TMV_AddDD_H
#define TMV_AddDD_H

#include "TMV_BaseMatrix_Diag.h"
#include "TMV_Scaling.h"
#include "TMV_MultXM_Funcs.h"
#include "TMV_MultXV_Funcs.h"

namespace tmv {

    //
    // D = x * D + x * D
    //

    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static TMV_INLINE void AddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Diag_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M2::const_diag_type m2d = m2.diag();
        typename M3::diag_type m3d = m3.diag();
        AddVV(x1,m1d,x2,m2d,m3d); 
    }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static TMV_INLINE void NoAliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Diag_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M2::const_diag_type m2d = m2.diag();
        typename M3::diag_type m3d = m3.diag();
        NoAliasAddVV(x1,m1d,x2,m2d,m3d); 
    }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static TMV_INLINE void AliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Diag_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M2::const_diag_type m2d = m2.diag();
        typename M3::diag_type m3d = m3.diag();
        AliasAddVV(x1,m1d,x2,m2d,m3d); 
    }

    //
    // M = x * D + x * D
    //

    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static inline void AddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M2::const_diag_type m2d = m2.diag();
        typename M3::diag_type m3d = m3.diag();
        AddVV(x1,m1d,x2,m2d,m3d); 
        if (m1.size() > 1) {
            m3.upperTri().offDiag().setZero();
            m3.lowerTri().offDiag().setZero();
        }
    }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static inline void NoAliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M2::const_diag_type m2d = m2.diag();
        typename M3::diag_type m3d = m3.diag();
        m3.setZero();
        NoAliasAddVV(x1,m1d,x2,m2d,m3d); 
    }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static inline void AliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M2::const_diag_type m2d = m2.diag();
        typename M3::diag_type m3d = m3.diag();
        AliasAddVV(x1,m1d,x2,m2d,m3d); 
        if (m1.size() > 1) {
            m3.upperTri().offDiag().setZero();
            m3.lowerTri().offDiag().setZero();
        }
    }

    //
    // M = x * D + x * M
    //

    // TODO: This function is supposed to only check for aliasing if 
    // the sizes are UNKNOWN.
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static inline void AddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Calc<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    {
        if (SameStorage(m1,m3.mat())) {
            typename M1::copy_type m1c = m1;
            typename M1::copy_type::const_diag_type m1d = m1c.diag();
            typename M3::diag_type m3d = m3.mat().diag();
            MultXM<false>(x2,m2.mat(),m3.mat());
            NoAliasMultXV<true>(x1,m1d,m3d);
        } else {
            typename M1::const_diag_type m1d = m1.diag();
            typename M3::diag_type m3d = m3.mat().diag();
            MultXM<false>(x2,m2.mat(),m3.mat());
            NoAliasMultXV<true>(x1,m1d,m3d);
        } 
    }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static inline void NoAliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Calc<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    {
        typename M1::const_diag_type m1d = m1.diag();
        typename M3::diag_type m3d = m3.mat().diag();
        NoAliasMultXM<false>(x2,m2.mat(),m3.mat());
        NoAliasMultXV<true>(x1,m1d,m3d);
    }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static inline void AliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Diag<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Calc<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    {
        if (SameStorage(m1,m3.mat())) {
            typename M1::copy_type m1c = m1;
            typename M1::copy_type::const_diag_type m1d = m1c.diag();
            typename M3::diag_type m3d = m3.mat().diag();
            MultXM<false>(x2,m2.mat(),m3.mat());
            NoAliasMultXV<true>(x1,m1d,m3d);
        } else {
            typename M1::const_diag_type m1d = m1.diag();
            typename M3::diag_type m3d = m3.mat().diag();
            MultXM<false>(x2,m2.mat(),m3.mat());
            NoAliasMultXV<true>(x1,m1d,m3d);
        } 
    }

    //
    // M = x * M + x * D
    //

    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static TMV_INLINE void AddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Calc<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    { AddMM(x2,m2,x1,m1,m3); }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static TMV_INLINE void NoAliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Calc<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    { NoAliasAddMM(x2,m2,x1,m1,m3); }
    template <int ix1, class T1, class M1, int ix2, class T2, class M2, class M3>
    static TMV_INLINE void AliasAddMM(
        const Scaling<ix1,T1>& x1, const BaseMatrix_Calc<M1>& m1,
        const Scaling<ix2,T2>& x2, const BaseMatrix_Diag<M2>& m2,
        BaseMatrix_Mutable<M3>& m3)
    { AliasAddMM(x2,m2,x1,m1,m3); }

} // namespace tmv

#endif
