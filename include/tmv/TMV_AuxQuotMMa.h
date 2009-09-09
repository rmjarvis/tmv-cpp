///////////////////////////////////////////////////////////////////////////////
// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:
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


#ifndef X3
#define X3
#endif

#ifndef Y
#define Y
#endif

#ifndef CT
#define CT std::complex<T>
#endif
#ifndef CCT
#define CCT ConjRef<std::complex<T> >
#endif
#ifndef VCT
#define VCT VarConjRef<std::complex<T> >
#endif

#ifndef GETM1
#define GETM1 .GetM1()
#endif

#ifndef GETM2
#define GETM2 .GetM2()
#endif

// x*(x*m/m)
template <class T, class T1, class T2 Y> 
inline QUOTMM<T,T1,T2 X3> operator*(T x, const QUOTMM<T,T1,T2 X3>& qmm)
{ return QUOTMM<T,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(CT x, const QUOTMM<CT,T,T X3>& qmm)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(CCT x, const QUOTMM<CT,T,T X3>& qmm)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(VCT x, const QUOTMM<CT,T,T X3>& qmm)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(T x, const QUOTMM<CT,T1,T2 X3>& qmm)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(CCT x, const QUOTMM<CT,T1,T2 X3>& qmm)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(VCT x, const QUOTMM<CT,T1,T2 X3>& qmm)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

// (x*m/m)*x
template <class T, class T1, class T2 Y> 
inline QUOTMM<T,T1,T2 X3> operator*(const QUOTMM<T,T1,T2 X3>& qmm, T x)
{ return QUOTMM<T,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(const QUOTMM<CT,T,T X3>& qmm, CT x)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(const QUOTMM<CT,T,T X3>& qmm, CCT x)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(const QUOTMM<CT,T,T X3>& qmm, VCT x)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(const QUOTMM<CT,T1,T2 X3>& qmm, T x)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(const QUOTMM<CT,T1,T2 X3>& qmm, CCT x)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(const QUOTMM<CT,T1,T2 X3>& qmm, VCT x)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

// (x*m/m)/x
template <class T, class T1, class T2 Y> 
inline QUOTMM<T,T1,T2 X3> operator/(const QUOTMM<T,T1,T2 X3>& qmm, T x)
{ return QUOTMM<T,T1,T2 X3>(qmm.GetX()/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator/(const QUOTMM<CT,T,T X3>& qmm, CT x)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator/(const QUOTMM<CT,T,T X3>& qmm, CCT x)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator/(const QUOTMM<CT,T,T X3>& qmm, VCT x)
{ return QUOTMM<CT,T,T X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator/(const QUOTMM<CT,T1,T2 X3>& qmm, T x)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()/x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator/(const QUOTMM<CT,T1,T2 X3>& qmm, CCT x)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator/(const QUOTMM<CT,T1,T2 X3>& qmm, VCT x)
{ return QUOTMM<CT,T1,T2 X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

// x*(x*m%m)
template <class T, class T1, class T2 Y> 
inline RQUOTMM<T,T1,T2 X3> operator*(T x, const RQUOTMM<T,T1,T2 X3>& qmm)
{ return RQUOTMM<T,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(CT x, const RQUOTMM<CT,T,T X3>& qmm)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(CCT x, const RQUOTMM<CT,T,T X3>& qmm)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(VCT x, const RQUOTMM<CT,T,T X3>& qmm)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(T x, const RQUOTMM<CT,T1,T2 X3>& qmm)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(CCT x, const RQUOTMM<CT,T1,T2 X3>& qmm)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(VCT x, const RQUOTMM<CT,T1,T2 X3>& qmm)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

// (x*m%m)*x
template <class T, class T1, class T2 Y> 
inline RQUOTMM<T,T1,T2 X3> operator*(const RQUOTMM<T,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<T,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(const RQUOTMM<CT,T,T X3>& qmm, CT x)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(const RQUOTMM<CT,T,T X3>& qmm, CCT x)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(const RQUOTMM<CT,T,T X3>& qmm, VCT x)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(const RQUOTMM<CT,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()*x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(const RQUOTMM<CT,T1,T2 X3>& qmm, CCT x)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(const RQUOTMM<CT,T1,T2 X3>& qmm, VCT x)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()*CT(x),qmm GETM1,qmm GETM2); }

// (x*m%m)/x
template <class T, class T1, class T2 Y> 
inline RQUOTMM<T,T1,T2 X3> operator/(const RQUOTMM<T,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<T,T1,T2 X3>(qmm.GetX()/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator/(const RQUOTMM<CT,T,T X3>& qmm, CT x)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator/(const RQUOTMM<CT,T,T X3>& qmm, CCT x)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator/(const RQUOTMM<CT,T,T X3>& qmm, VCT x)
{ return RQUOTMM<CT,T,T X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator/(const RQUOTMM<CT,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()/x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator/(const RQUOTMM<CT,T1,T2 X3>& qmm, CCT x)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator/(const RQUOTMM<CT,T1,T2 X3>& qmm, VCT x)
{ return RQUOTMM<CT,T1,T2 X3>(qmm.GetX()/CT(x),qmm GETM1,qmm GETM2); }

#ifdef QUOTMM_1

// x*(m/m)
template <class T, class T1, class T2 Y> 
inline QUOTMM<T,T1,T2 X3> operator*(T x, const QUOTMM_1<T,T1,T2 X3>& qmm)
{ return QUOTMM<T,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(CT x, const QUOTMM_1<CT,T,T X3>& qmm)
{ return QUOTMM<CT,T,T X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(CCT x, const QUOTMM_1<CT,T,T X3>& qmm)
{ return QUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(VCT x, const QUOTMM_1<CT,T,T X3>& qmm)
{ return QUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(T x, const QUOTMM_1<CT,T1,T2 X3>& qmm)
{ return QUOTMM<CT,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(CCT x, const QUOTMM_1<CT,T1,T2 X3>& qmm)
{ return QUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(VCT x, const QUOTMM_1<CT,T1,T2 X3>& qmm)
{ return QUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

// (m/m)*x
template <class T, class T1, class T2 Y> 
inline QUOTMM<T,T1,T2 X3> operator*(const QUOTMM_1<T,T1,T2 X3>& qmm, T x)
{ return QUOTMM<T,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(const QUOTMM_1<CT,T,T X3>& qmm, CT x)
{ return QUOTMM<CT,T,T X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(const QUOTMM_1<CT,T,T X3>& qmm, CCT x)
{ return QUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator*(const QUOTMM_1<CT,T,T X3>& qmm, VCT x)
{ return QUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(const QUOTMM_1<CT,T1,T2 X3>& qmm, T x)
{ return QUOTMM<CT,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(const QUOTMM_1<CT,T1,T2 X3>& qmm, CCT x)
{ return QUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator*(const QUOTMM_1<CT,T1,T2 X3>& qmm, VCT x)
{ return QUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

// (m/m)/x
template <class T, class T1, class T2 Y> 
inline QUOTMM<T,T1,T2 X3> operator/(const QUOTMM_1<T,T1,T2 X3>& qmm, T x)
{ return QUOTMM<T,T1,T2 X3>(RealType(T)(1)/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator/(const QUOTMM_1<CT,T,T X3>& qmm, CT x)
{ return QUOTMM<CT,T,T X3>(T(1)/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator/(const QUOTMM_1<CT,T,T X3>& qmm, CCT x)
{ return QUOTMM<CT,T,T X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline QUOTMM<CT,T,T X3> operator/(const QUOTMM_1<CT,T,T X3>& qmm, VCT x)
{ return QUOTMM<CT,T,T X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator/(const QUOTMM_1<CT,T1,T2 X3>& qmm, T x)
{ return QUOTMM<CT,T1,T2 X3>(T(1)/x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator/(const QUOTMM_1<CT,T1,T2 X3>& qmm, CCT x)
{ return QUOTMM<CT,T1,T2 X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline QUOTMM<CT,T1,T2 X3> operator/(const QUOTMM_1<CT,T1,T2 X3>& qmm, VCT x)
{ return QUOTMM<CT,T1,T2 X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

// x*(m%m)
template <class T, class T1, class T2 Y> 
inline RQUOTMM<T,T1,T2 X3> operator*(T x, const RQUOTMM_1<T,T1,T2 X3>& qmm)
{ return RQUOTMM<T,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(CT x, const RQUOTMM_1<CT,T,T X3>& qmm)
{ return RQUOTMM<CT,T,T X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(CCT x, const RQUOTMM_1<CT,T,T X3>& qmm)
{ return RQUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(VCT x, const RQUOTMM_1<CT,T,T X3>& qmm)
{ return RQUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(T x, const RQUOTMM_1<CT,T1,T2 X3>& qmm)
{ return RQUOTMM<CT,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(CCT x, const RQUOTMM_1<CT,T1,T2 X3>& qmm)
{ return RQUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(VCT x, const RQUOTMM_1<CT,T1,T2 X3>& qmm)
{ return RQUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

// (m%m)*x
template <class T, class T1, class T2 Y> 
inline RQUOTMM<T,T1,T2 X3> operator*(const RQUOTMM_1<T,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<T,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(const RQUOTMM_1<CT,T,T X3>& qmm, CT x)
{ return RQUOTMM<CT,T,T X3>(x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(const RQUOTMM_1<CT,T,T X3>& qmm, CCT x)
{ return RQUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator*(const RQUOTMM_1<CT,T,T X3>& qmm, VCT x)
{ return RQUOTMM<CT,T,T X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(const RQUOTMM_1<CT,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<CT,T1,T2 X3>(x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(const RQUOTMM_1<CT,T1,T2 X3>& qmm, CCT x)
{ return RQUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator*(const RQUOTMM_1<CT,T1,T2 X3>& qmm, VCT x)
{ return RQUOTMM<CT,T1,T2 X3>(CT(x),qmm GETM1,qmm GETM2); }

// (m%m)/x
template <class T, class T1, class T2 Y> 
inline RQUOTMM<T,T1,T2 X3> operator/(const RQUOTMM_1<T,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<T,T1,T2 X3>(RealType(T)(1)/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator/(const RQUOTMM_1<CT,T,T X3>& qmm, CT x)
{ return RQUOTMM<CT,T,T X3>(T(1)/x,qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator/(const RQUOTMM_1<CT,T,T X3>& qmm, CCT x)
{ return RQUOTMM<CT,T,T X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

template <class T Y> 
inline RQUOTMM<CT,T,T X3> operator/(const RQUOTMM_1<CT,T,T X3>& qmm, VCT x)
{ return RQUOTMM<CT,T,T X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator/(const RQUOTMM_1<CT,T1,T2 X3>& qmm, T x)
{ return RQUOTMM<CT,T1,T2 X3>(T(1)/x,qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator/(const RQUOTMM_1<CT,T1,T2 X3>& qmm, CCT x)
{ return RQUOTMM<CT,T1,T2 X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

template <class T, class T1, class T2 Y> 
inline RQUOTMM<CT,T1,T2 X3> operator/(const RQUOTMM_1<CT,T1,T2 X3>& qmm, VCT x)
{ return RQUOTMM<CT,T1,T2 X3>(T(1)/CT(x),qmm GETM1,qmm GETM2); }

#undef QUOTMM_1
#endif

#undef X3
#undef Y
#undef GETM1
#undef GETM2


