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



#include "TMV_Blas.h"
#include "TMV_VectorArithFunc.h"
#include "TMV_Vector.h"

//#define XDEBUG

#ifdef XDEBUG
#include "TMV_VIt.h"
#include <iostream>
using std::cerr;
using std::endl;
#endif

namespace tmv {

#ifdef TMV_BLOCKSIZE
#define TMV_MULTVV_RECURSE_SIZE TMV_BLOCKSIZE
#else
#define TMV_MULTVV_RECURSE_SIZE 64
#endif

  //
  // VectorComposite
  //

  template <class T> const T* VectorComposite<T>::cptr() const
  {
    if (!itsv.get()) {
      size_t len = this->size();
      itsv.reset(new T[len]);
      AssignToV(VectorView<T>(itsv.get(),len,1,NonConj
	  FIRSTLAST1(itsv.get(),itsv.get()+len) ) );
    }
    return itsv.get();
  }

  //
  // MultVV
  //
  template <bool unit, bool c2, class T, class T2> static T NonBlasMultVV(
      const GenVector<T>& v1, const GenVector<T2>& v2) 
  {
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    TMVAssert(v1.ct() == NonConj);
    TMVAssert(v2.step() != -1);
    TMVAssert(v1.step() != -1 || v2.step() == 1);
    TMVAssert(v2.step() >= 0 || v1.step() == 1);
    TMVAssert(c2 == v2.isconj());

    const T* v1ptr = v1.cptr();
    const T2* v2ptr = v2.cptr();

    const int N = v1.size();
    if (N > TMV_MULTVV_RECURSE_SIZE) {
      // This isn't for speed reasons - it's for increased accuracy.
      // For large vectors, the incremental additions can be much smaller
      // than the running sum, so the relative errors can be huge.
      // With the recursive algorithm, the relative error is generally
      // closer to the expected few * epsilon.
      const int N1 = N/2;
      return NonBlasMultVV<unit,c2>(v1.SubVector(0,N1),v2.SubVector(0,N1)) +
	NonBlasMultVV<unit,c2>(v1.SubVector(N1,N),v2.SubVector(N1,N));
    } else {
      T res(0);

      if (unit) {
	const int N1 = N/4;
	const int N2 = N-4*N1;
	if (N1) for(int i=N1;i>0;--i,v1ptr+=4,v2ptr+=4) {
	  res += (*v1ptr) * (c2 ? CONJ(*v2ptr) : (*v2ptr));
	  res += v1ptr[1] * (c2 ? CONJ(v2ptr[1]) : v2ptr[1]);
	  res += v1ptr[2] * (c2 ? CONJ(v2ptr[2]) : v2ptr[2]);
	  res += v1ptr[3] * (c2 ? CONJ(v2ptr[3]) : v2ptr[3]);
	}
	if (N2) for(int i=N2;i>0;--i,++v1ptr,++v2ptr) 
	  res += (*v1ptr) * (c2 ? CONJ(*v2ptr) : (*v2ptr));
      } else {
	const int s1 = v1.step();
	const int s2 = v2.step();

	for(int i=N;i>0;--i,v1ptr+=s1,v2ptr+=s2) 
	  res += (*v1ptr) * (c2 ? CONJ(*v2ptr) : (*v2ptr));
      }
      return res;
    }
  }
  template <class T, class T2> static inline T DoMultVV(
      const GenVector<T>& v1, const GenVector<T2>& v2) 
  { 
    if (v1.step() == 1 && v2.step() == 1)
      if (v2.isconj()) return NonBlasMultVV<true,true>(v1,v2); 
      else return NonBlasMultVV<true,false>(v1,v2); 
    else
      if (v2.isconj()) return NonBlasMultVV<false,true>(v1,v2); 
      else return NonBlasMultVV<false,false>(v1,v2); 
  }
#ifdef BLAS
#ifdef INST_DOUBLE
  template <> double DoMultVV(
      const GenVector<double>& v1, const GenVector<double>& v2) 
  { 
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    int n=v2.size();
    int s1=v1.step();
    int s2=v2.step();
    const double* v1p = v1.cptr();
    if (s1 < 0) v1p += (n-1)*s1;
    const double* v2p = v2.cptr();
    if (s2 < 0) v2p += (n-1)*s2;
    return BLASNAME(ddot) (BLASV(n),BLASP(v1p),BLASV(s1),
	BLASP(v2p),BLASV(s2));
  }
  template <> std::complex<double> DoMultVV(
      const GenVector<std::complex<double> >& v1, 
      const GenVector<std::complex<double> >& v2) 
  { 
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    TMVAssert(v1.ct() == NonConj);
    int n=v2.size();
    int s1=v1.step();
    int s2=v2.step();
    const std::complex<double>* v1p = v1.cptr();
    if (s1 < 0) v1p += (n-1)*s1;
    const std::complex<double>* v2p = v2.cptr();
    if (s2 < 0) v2p += (n-1)*s2;
    std::complex<double> res;
    if (v2.isconj())
      BLASZDOTSET( res, BLASZDOTNAME(zdotc) (
	    BLASZDOT1(BLASP(&res))
	    BLASV(n),BLASP(v2p),BLASV(s2),
	    BLASP(v1p),BLASV(s1)
	    BLASZDOT2(BLASP(&res)) ));
    else
      BLASZDOTSET( res, BLASZDOTNAME(zdotu) (
	    BLASZDOT1(BLASP(&res))
	    BLASV(n),BLASP(v2p),BLASV(s2),
	    BLASP(v1p),BLASV(s1)
	    BLASZDOT2(BLASP(&res)) ));
    return res;
  }
  template <> std::complex<double> DoMultVV(
      const GenVector<std::complex<double> >& v1, 
      const GenVector<double>& v2) 
  { 
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    TMVAssert(v1.ct() == NonConj);
    int n=v2.size();
    int s1=2*v1.step();
    int s2=v2.step();
    const std::complex<double>* v1p = v1.cptr();
    if (s1 < 0) v1p += (n-1)*v1.step();
    const double* v2p = v2.cptr();
    if (s2 < 0) v2p += (n-1)*s2;
    double resr = 
      BLASNAME(ddot) (BLASV(n),BLASP((double*)v1p),BLASV(s1),
	BLASP(v2p),BLASV(s2));
    double resi = 
      BLASNAME(ddot) (BLASV(n),BLASP((double*)v1p+1),BLASV(s1),
	BLASP(v2p),BLASV(s2));
    return std::complex<double>(resr,resi);
  }
#endif
#ifdef INST_FLOAT
  template <> float DoMultVV(
      const GenVector<float>& v1, const GenVector<float>& v2) 
  { 
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    int n=v2.size();
    int s1=v1.step();
    int s2=v2.step();
    const float* v1p = v1.cptr();
    if (s1 < 0) v1p += (n-1)*s1;
    const float* v2p = v2.cptr();
    if (s2 < 0) v2p += (n-1)*s2;
    return BLASNAME(sdot) (BLASV(n),BLASP(v1p),BLASV(s1),
	BLASP(v2p),BLASV(s2));
  }
  template <> std::complex<float> DoMultVV(
      const GenVector<std::complex<float> >& v1, 
      const GenVector<std::complex<float> >& v2) 
  { 
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    TMVAssert(v1.ct() == NonConj);
    int n=v2.size();
    int s1=v1.step();
    int s2=v2.step();
    const std::complex<float>* v1p = v1.cptr();
    if (s1 < 0) v1p += (n-1)*s1;
    const std::complex<float>* v2p = v2.cptr();
    if (s2 < 0) v2p += (n-1)*s2;
    std::complex<float> res;
    if (v2.isconj())
      BLASZDOTSET( res, BLASZDOTNAME(cdotc) (
	    BLASZDOT1(BLASP(&res))
	    BLASV(n),BLASP(v2p),BLASV(s2),
	    BLASP(v1p),BLASV(s1)
	    BLASZDOT2(BLASP(&res)) ));
    else
      BLASZDOTSET( res, BLASZDOTNAME(cdotu) (
	    BLASZDOT1(BLASP(&res))
	    BLASV(n),BLASP(v2p),BLASV(s2),
	    BLASP(v1p),BLASV(s1)
	    BLASZDOT2(BLASP(&res)) ));
    return res;
  }
  template <> std::complex<float> DoMultVV(
      const GenVector<std::complex<float> >& v1, 
      const GenVector<float>& v2) 
  { 
    TMVAssert(v1.size()==v2.size());
    TMVAssert(v1.size()>0);
    TMVAssert(v1.ct() == NonConj);
    int n=v2.size();
    int s1=2*v1.step();
    int s2=v2.step();
    const std::complex<float>* v1p = v1.cptr();
    if (s1 < 0) v1p += (n-1)*v1.step();
    const float* v2p = v2.cptr();
    if (s2 < 0) v2p += (n-1)*s2;
    float resr = 
      BLASNAME(sdot) (BLASV(n),BLASP((float*)v1p),BLASV(s1),
	BLASP(v2p),BLASV(s2));
    float resi = 
      BLASNAME(sdot) (BLASV(n),BLASP((float*)v1p)+1,BLASV(s1),
	BLASP(v2p),BLASV(s2));
    return std::complex<float>(resr,resi);
  }
#endif
#endif // BLAS

  template <class T, class T2> T MultVV(
      const GenVector<T>& v1, const GenVector<T2>& v2) 
  { 
    TMVAssert(v1.size() == v2.size()); 

#ifdef XDEBUG
    T resx(0);
    for(int i=0;i<int(v1.size());i++) {
      resx += v1(i)*v2(i);
    }
#endif

    T res(0);
    if (v1.size() > 0) {
      if (v2.SameAs(v1.Conjugate())) return v1.NormSq();
      else if (ShouldReverse(v1.step(),v2.step())) 
	if (v1.isconj()) 
	  res = CONJ(DoMultVV(v1.Reverse().Conjugate(),
		v2.Reverse().Conjugate()));
	else 
	  res = DoMultVV(v1.Reverse(),v2.Reverse());
      else 
	if (v1.isconj()) 
	  res = CONJ(DoMultVV(v1.Conjugate(),v2.Conjugate()));
	else 
	  res = DoMultVV(v1,v2);
    }

#ifdef XDEBUG
    if (ABS(resx-res) > 0.001*MAX(RealType(T)(1),Norm(v1)*Norm(v2))) {
      cerr<<"MultVV: \n";
      cerr<<"v1 = "<<Type(v1)<<"  step "<<v1.step()<<"  "<<v1<<endl;
      cerr<<"v2 = "<<Type(v2)<<"  step "<<v2.step()<<"  "<<v2<<endl;
      cerr<<"v1*v2 = "<<resx<<endl;
      cerr<<"res = "<<res<<endl;
      cerr<<"abs(resx-res) = "<<ABS(resx-res)<<endl;
      abort();
    }
#endif

    return res;
  }

#define InstFile "TMV_MultVV.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace mv


