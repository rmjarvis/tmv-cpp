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
#include "TMV_SymMatrix.h"
#include "TMV_Matrix.h"
#include "TMV_Vector.h"
#include "TMV_SymLDLD.h"
#include "TMV_SymCHD.h"
#include "TMV_SymSVD.h"
#include "TMV_VIt.h"
#include "TMV_SymMatrixArith.h"
#include "TMV_DiagMatrix.h"
#include <iostream>

#include "TMV_SVD.h"
//#define XDEBUG

#ifdef XDEBUG
using std::cerr;
using std::endl;
#endif

namespace tmv {

#define RT RealType(T)

  //
  // Access
  //

  template <class T> T GenSymMatrix<T>::cref(int i, int j) const
  {
    TMVAssert(i < int(size()));
    TMVAssert(j < int(size()));
    const T* mi;
    if ((uplo() == Upper && i<=j) || (uplo() == Lower && i>=j)) {
      if (isrm()) mi = cptr() + int(i)*stepi() + j;
      else if (iscm()) mi = cptr() + i + int(j)*stepj();
      else mi = cptr() + int(i)*stepi() + int(j)*stepj();
      return isconj() ? CONJ(*mi) : *mi;
    } else {
      if (isrm()) mi = cptr() + int(j)*stepi() + i;
      else if (iscm()) mi = cptr() + j + int(i)*stepj();
      else mi = cptr() + int(j)*stepi() + int(i)*stepj();
      return IsReal(T()) ? *mi : 
	(ct()==Conj) != (sym()==Herm) ? CONJ(*mi) : *mi;
    }
  }

  template <class T, IndexStyle I> RefType(T) SymMatrixView<T,I>::ref(
      int i, int j) const
  {
    TMVAssert(i < int(size()));
    TMVAssert(j < int(size()));
    T* mi;
    if ((uplo() == Upper && i<=j) || (uplo() == Lower && i>=j)) {
      if (this->isrm()) mi = ptr() + int(i)*stepi() + j;
      else if (this->iscm()) mi = ptr() + i + int(j)*stepj();
      else mi = ptr() + int(i)*stepi() + int(j)*stepj();
    } else {
      if (this->isrm()) mi = ptr() + int(j)*stepi() + i;
      else if (this->iscm()) mi = ptr() + j + int(i)*stepj();
      else mi = ptr() + int(j)*stepi() + int(i)*stepj();
    }
#ifdef TMVFLDEBUG
    TMVAssert(mi>=first);
    TMVAssert(mi<last);
#endif
    if ((uplo() == Upper && i<=j) || (uplo() == Lower && i>=j)) 
      return REF(mi,ct());
    else
      return IsReal(T()) ? REF(mi,NonConj) : 
	(ct()==Conj) != (sym()==Herm) ? REF(mi,Conj) : REF(mi,NonConj);
  }

  template <class T> auto_ptr<BaseMatrix<T> > GenSymMatrix<T>::NewCopy() const
  {
    auto_ptr<BaseMatrix<T> > a;
    if (issym()) {
      if (uplo() == Upper) {
	if (isrm()) a.reset(new SymMatrix<T,Upper,RowMajor>(*this));
	else a.reset(new SymMatrix<T,Upper,ColMajor>(*this));
      } else {
	if (isrm()) a.reset(new SymMatrix<T,Lower,RowMajor>(*this));
	else a.reset(new SymMatrix<T,Lower,ColMajor>(*this));
      }
    } else {
      if (uplo() == Upper) {
	if (isrm()) a.reset(new HermMatrix<T,Upper,RowMajor>(*this));
	else a.reset(new HermMatrix<T,Upper,ColMajor>(*this));
      } else {
	if (isrm()) a.reset(new HermMatrix<T,Lower,RowMajor>(*this));
	else a.reset(new HermMatrix<T,Lower,ColMajor>(*this));
      }
    }
    return a;
  }

  template <class T> auto_ptr<BaseMatrix<T> > GenSymMatrix<T>::NewView() const
  {
    auto_ptr<BaseMatrix<T> > a(new ConstSymMatrixView<T>(View()));
    return a;
  }

  template <class T> auto_ptr<BaseMatrix<T> > GenSymMatrix<T>::NewTranspose() const
  {
    auto_ptr<BaseMatrix<T> > a(new ConstSymMatrixView<T>(Transpose()));
    return a;
  }

  template <class T> auto_ptr<BaseMatrix<T> > GenSymMatrix<T>::NewConjugate() const
  {
    auto_ptr<BaseMatrix<T> > a(new ConstSymMatrixView<T>(Conjugate()));
    return a;
  }

  template <class T> auto_ptr<BaseMatrix<T> > GenSymMatrix<T>::NewAdjoint() const
  {
    auto_ptr<BaseMatrix<T> > a(new ConstSymMatrixView<T>(Adjoint()));
    return a;
  }

  template <class T> auto_ptr<BaseMatrix<T> > GenSymMatrix<T>::NewInverse() const
  {
    if (issym()) {
      auto_ptr<SymMatrix<T,Upper,ColMajor> > a(
	  new SymMatrix<T,Upper,ColMajor>(size()));
      Inverse(a->View());
      BaseMatrix<T>* ret1 = a.release();
      auto_ptr<BaseMatrix<T> > ret(ret1);
      return ret;
    } else {
      auto_ptr<HermMatrix<T,Upper,ColMajor> > a(
	  new HermMatrix<T,Upper,ColMajor>(size()));
      Inverse(a->View());
      BaseMatrix<T>* ret1 = a.release();
      auto_ptr<BaseMatrix<T> > ret(ret1);
      return ret;
    }
  }

  template <class T> void GenSymMatrix<T>::NewDivider() const
  { 
    switch(this->GetDivType()) {
      case LU : {
		  this->SetDiv(new SymLDLDiv<T>(*this,this->IsDivInPlace())); 
		  break; 
		}
      case SV : {
		  if (IsReal(T()) || sym()==Herm) {
		    this->SetDiv(new HermSVDiv<T>(*this)); 
		  } else {
		    this->SetDiv(new SymSVDiv<T>(*this)); 
		  }
		  break; 
		}
      case CH : {
		  this->SetDiv(new HermCHDiv<T>(*this,this->IsDivInPlace()));
		  break;
		}
      default : TMVAssert(FALSE); 
    }
  }

  template <class T> QuotXS<T,T> GenSymMatrix<T>::QInverse() const
  { return QuotXS<T,T>(T(1),*this); }

  template <class T> template <class T1> void GenSymMatrix<T>::DoInverse(
      const SymMatrixView<T1>& sinv) const
  {
    TMVAssert(issym() == sinv.issym());
    TMVAssert(isherm() == sinv.isherm());

    this->SetDiv();
    const SymDivider<T>* sdiv = dynamic_cast<const SymDivider<T>*>(
	this->GetDiv());
    TMVAssert(sdiv);
    sdiv->Inverse(sinv);
  }

  //
  // OK? (SubMatrix, etc.)
  //

  template <class T> bool GenSymMatrix<T>::OKSubMatrix(
      int i1, int i2, int j1, int j2, int istep, int jstep) const
  { 
    if (i1==i2 || j1==j2) return true; // no elements, so whatever...
    bool ok = true;
    int i2x = i2-istep;
    int j2x = j2-jstep;
    if (istep == 0) {
      ok = false;
      std::cout<<"istep ("<<istep<<") can not be 0\n";
    }
    if (i1 < 0 || i1 >= int(size())) {
      ok = false;
      std::cout<<"first col element ("<<i1<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if (i2x < 0 || i2x >= int(size())) {
      ok = false;
      std::cout<<"last col element ("<<i2x<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if ((i2-i1)%istep != 0) {
      ok = false;
      std::cout<<"col range ("<<i2-i1<<") must be multiple of istep (";
      std::cout<<istep<<")\n";
    }
    if ((i2-i1)/istep < 0) {
      ok = false;
      std::cout<<"n col elements ("<<(i2-i1)/istep<<") must be nonnegative\n";
    }
    if (jstep == 0) {
      ok = false;
      std::cout<<"jstep ("<<jstep<<") can not be 0\n";
    }
    if (j1 < 0 || j1 >= int(size())) {
      ok = false;
      std::cout<<"first row element ("<<j1<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if (j2-jstep < 0 || j2-jstep >= int(size())) {
      ok = false;
      std::cout<<"last row element ("<<j2-jstep<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if ((j2-j1)%jstep != 0) {
      ok = false;
      std::cout<<"row range ("<<j2-j1<<") must be multiple of istep (";
      std::cout<<jstep<<")\n";
    }
    if ((j2-j1)/jstep < 0) {
      ok = false;
      std::cout<<"n row elements ("<<(j2-j1)/jstep<<") must be nonnegative\n";
    }
    if ((i1<j1 && i2x>j2x) || (i1>j1 && i2x<j2x)) {
      ok = false;
      std::cout<<"Upper left ("<<i1<<','<<j1<<") and lower right (";
      std::cout<<i2x<<','<<j2x<<") corners must be in same triangle\n";
    }
    if ((i2x<j1 && i1>j2x) || (i2x>j1 && i1<j2x)) {
      ok = false;
      std::cout<<"Upper right ("<<i1<<','<<j2x<<") and lower left (";
      std::cout<<i2x<<','<<j1<<") corners must be in same triangle\n";
    }
    return ok;
  }

  template <class T> bool GenSymMatrix<T>::OKSubVector(
      int i, int j, int istep, int jstep, int n) const 
  {
    if (n==0) return true;
    bool ok = true;
    if (istep == 0 && jstep == 0) {
      ok = false;
      std::cout<<"istep ("<<istep<<") and jstep ("<<jstep;
      std::cout<<") can not both be 0\n";
    }
    if (i<0 || i >= int(size())) {
      ok = false;
      std::cout<<"i ("<<i<<") must be in 0 -- "<<size()-1<<std::endl;
    }
    if (j<0 || j >= int(size())) {
      ok = false;
      std::cout<<"j ("<<j<<") must be in 0 -- "<<size()-1<<std::endl;
    }
    int i2 = int(i)+istep*int(n-1);
    int j2 = int(j)+jstep*int(n-1);
    if (i2 < 0 || i2 >= int(size())) {
      ok = false;
      std::cout<<"last element's i ("<<i2<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if (j2 < 0 || j2 >= int(size())) {
      ok = false;
      std::cout<<"last element's j ("<<j2<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if ((i<j && i2>j2) || (i>j && i2<j2)) {
      ok = false;
      std::cout<<"First ("<<i<<','<<j<<") and last ("<<i2<<','<<j2;
      std::cout<<") elements must be in same triangle\n";
    }
    return ok;
  }

  template <class T> bool GenSymMatrix<T>::OKSubSymMatrix(
      int i1, int i2, int istep) const 
  {
    if (i1==i2) return true;
    bool ok=true;
    if (istep == 0) {
      ok = false; 
      std::cout<<"istep ("<<istep<<") can not be 0\n";
    }
    if (i1<0 || i1 >= int(size())) {
      ok = false;
      std::cout<<"first diag element ("<<i1<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if (i2-istep<0 || i2-istep >= int(size())) {
      ok = false;
      std::cout<<"last diag element ("<<i2-istep<<") must be in 0 -- ";
      std::cout<<size()-1<<std::endl;
    }
    if ((i2-i1)%istep != 0) {
      ok = false;
      std::cout<<"range ("<<i2-i1<<") must be multiple of istep (";
      std::cout<<istep<<")\n";
    }
    if ((i2-i1)/istep < 0) {
      ok = false;
      std::cout<<"n diag elements ("<<(i2-i1)/istep<<") must be nonnegative\n";
    }
    return ok;
  }

  template <class T> bool ConstSymMatrixView<T,FortranStyle>::OKSubMatrix(
      int i1, int i2, int j1, int j2, int istep, int jstep) const
  { 
    if (i1==i2 || j1==j2) return true; // no elements, so whatever...
    bool ok = true;
    if (istep == 0) {
      ok = false;
      std::cout<<"istep ("<<istep<<") can not be 0\n";
    }
    if (i1 < 1 || i1 > int(this->size())) {
      ok = false;
      std::cout<<"first col element ("<<i1<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if (i2 < 1 || i2 > int(this->size())) {
      ok = false;
      std::cout<<"last col element ("<<i2-istep<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if ((i2-i1)%istep != 0) {
      ok = false;
      std::cout<<"col range ("<<i2-i1<<") must be multiple of istep (";
      std::cout<<istep<<")\n";
    }
    if ((i2-i1)/istep < 0) {
      ok = false;
      std::cout<<"n col elements ("<<(i2-i1)/istep+1<<") must be positive\n";
    }
    if (jstep == 0) {
      ok = false;
      std::cout<<"jstep ("<<jstep<<") can not be 0\n";
    }
    if (j1 < 1 || j1 > int(this->size())) {
      ok = false;
      std::cout<<"first row element ("<<j1<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if (j2 < 1 || j2 > int(this->size())) {
      ok = false;
      std::cout<<"last row element ("<<j2-jstep<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if ((j2-j1)%jstep != 0) {
      ok = false;
      std::cout<<"row range ("<<j2-j1<<") must be multiple of istep (";
      std::cout<<jstep<<")\n";
    }
    if ((j2-j1)/jstep < 0) {
      ok = false;
      std::cout<<"n row elements ("<<(j2-j1)/jstep+1<<") must be positive\n";
    }
    if ((i1<j1 && i2>j2) || (i1>j1 && i2<j2)) {
      ok = false;
      std::cout<<"Upper left ("<<i1<<','<<j1<<") and lower right (";
      std::cout<<i2<<','<<j2<<") corners must be in same triangle\n";
    }
    if ((i2<j1 && i1>j2) || (i2>j1 && i1<j2)) {
      ok = false;
      std::cout<<"Upper right ("<<i1<<','<<j2<<") and lower left (";
      std::cout<<i2<<','<<j1<<") corners must be in same triangle\n";
    }
    return ok;
  }

  template <class T> bool ConstSymMatrixView<T,FortranStyle>::OKSubVector(
      int i, int j, int istep, int jstep, int n) const 
  {
    if (n==0) return true;
    bool ok = true;
    if (istep == 0 && jstep == 0) {
      ok = false;
      std::cout<<"istep ("<<istep<<") and jstep ("<<jstep;
      std::cout<<") can not both be 0\n";
    }
    if (i < 1 || i > int(this->size())) {
      ok = false;
      std::cout<<"i ("<<i<<") must be in 1 -- "<<this->size()<<std::endl;
    }
    if (i < 1 || j > int(this->size())) {
      ok = false;
      std::cout<<"j ("<<j<<") must be in 1 -- "<<this->size()<<std::endl;
    }
    int i2 = int(i)+istep*int(n-1);
    int j2 = int(j)+jstep*int(n-1);
    if (i2 < 1 || i2 > int(this->size())) {
      ok = false;
      std::cout<<"last element's i ("<<i2<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if (j2 < 1 || j2 > int(this->size())) {
      ok = false;
      std::cout<<"last element's j ("<<j2<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if ((i<j && i2>j2) || (i>j && i2<j2)) {
      ok = false;
      std::cout<<"First ("<<i<<','<<j<<") and last ("<<i2<<','<<j2;
      std::cout<<") elements must be in same triangle\n";
    }
    return ok;
  }

  template <class T> bool ConstSymMatrixView<T,FortranStyle>::OKSubSymMatrix(
      int i1, int i2, int istep) const 
  {
    if (i1==i2) return true;
    bool ok=true;
    if (istep == 0) {
      ok = false; 
      std::cout<<"istep ("<<istep<<") can not be 0\n";
    }
    if (i1<1 || i1 > int(this->size())) {
      ok = false;
      std::cout<<"first diag element ("<<i1<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if (i2-istep<1 || i2-istep > int(this->size())) {
      ok = false;
      std::cout<<"last diag element ("<<i2-istep<<") must be in 1 -- ";
      std::cout<<this->size()<<std::endl;
    }
    if ((i2-i1)%istep != 0) {
      ok = false;
      std::cout<<"range ("<<i2-i1<<") must be multiple of istep ("<<istep;
      std::cout<<")\n";
    }
    if ((i2-i1)/istep < 0) {
      ok = false;
      std::cout<<"n diag elements ("<<(i2-i1)/istep+1<<") must be positive\n";
    }
    return ok;
  }

  // 
  // SwapRowsCols
  //

  template <class T, IndexStyle I> 
    const SymMatrixView<T,I>& SymMatrixView<T,I>::SwapRowsCols(
	int i1, int i2) const
    {
      TMVAssert(i1<int(size()));
      TMVAssert(i2<int(size()));
      if (i1 == i2) return *this;
      else {
#ifdef XDEBUG
	Matrix<T> M(*this);
	Matrix<T> M0(*this);
	M.SwapRows(i1,i2);
	M.SwapCols(i1,i2);
#endif
	if (i1 > i2) SWAP(i1,i2);
	// Now i1 < i2
	if (uplo() == Upper) Transpose().SwapRowsCols(i1,i2);
	else {
	  Swap(row(i1,0,i1),row(i2,0,i1));
	  Swap(row(i2,i1+1,i2),col(i1,i1+1,i2));
	  if (!this->issym()) {
	    row(i2,i1,i2).ConjugateSelf(); // Conj m(i2,i1) as well
	    col(i1,i1+1,i2).ConjugateSelf();
	  }
	  Swap(col(i1,i2+1,size()),col(i2,i2+1,size()));
	  diag().Swap(i1,i2);
	}
#ifdef XDEBUG
	if (tmv::Norm(M-*this) > 1.e-5*MAX(RT(1),tmv::Norm(M))) {
	  cerr<<"SwapRowsCols: i1,i2 = "<<i1<<','<<i2<<endl;
	  cerr<<"M0 = "<<tmv::Type(*this)<<"  "<<M0<<endl;
	  cerr<<"M = "<<M<<endl;
	  cerr<<"S = "<<*this<<endl;
	  abort();
	}
#endif
	return *this;
      }
    }

  template <class T, IndexStyle I> 
    const SymMatrixView<T,I>& SymMatrixView<T,I>::PermuteRowsCols(
	const int* p, int i1, int i2) const
    {
      const int* pi = p+i1;
      for(int i=i1;i<i2;++i,++pi) {
	TMVAssert(*pi < int(size()));
	SwapRowsCols(i,*pi);
      }
      return *this;
    }
 
  template <class T, IndexStyle I> 
    const SymMatrixView<T,I>& SymMatrixView<T,I>::ReversePermuteRowsCols(
	const int* p, int i1, int i2) const
    {
      const int* pi = p+i2;
      for(int i=i2;i>i1;) {
	--i; --pi;
	TMVAssert(*pi < int(size()));
	SwapRowsCols(i,*pi);
      }
      return *this;
    }
 

  //
  // Norms
  //

  template <class T> RT GenSymMatrix<T>::NormSq(const RT scale) const
  { 
    RT ans = diag().NormSq(scale);
    if (size() > 1) ans += RT(2) * UpperTri().OffDiag().NormSq(scale);
    return ans;
  }

  template <class T> static RT NonLapNorm1(const GenSymMatrix<T>& m) 
  { 
    RT max(0);
    for(int j=0;j<int(m.size());++j) {
      RT temp = m.col(j,0,j).Norm1();
      temp += m.col(j,j,m.size()).Norm1();
      if (temp > max) max = temp;
    }
    return max;
  } 

  template <class T> static RT NonLapNormF(const GenSymMatrix<T>& m)
  {
    static const RT eps = Epsilon<T>();
    static const RT inveps = RT(1)/eps;

    RT mmax = m.MaxAbsElement();
    if (mmax == RT(0)) return RT(0);
    else if (mmax * mmax * eps == RT(0)) {
      // Then we need to rescale, since underflow has caused rounding errors
      // Epsilon is a pure power of 2, so no rounding errors from rescaling.
      RT scale = inveps;
      mmax *= scale;
      while (mmax < eps) { scale *= inveps; mmax *= inveps; }
      return SQRT(m.NormSq(scale))/scale;
    } else if (RT(1) / (mmax*mmax) == RT(0)) {
      // Then we have overflow, so we need to rescale:
      RT scale = eps;
      mmax *= scale;
      while (mmax > RT(1)) { scale *= eps; mmax *= eps; }
      return SQRT(m.NormSq(scale))/scale;
    } 
    return SQRT(m.NormSq());
  }

  template <class T> static inline RT NonLapMaxAbsElement(
      const GenSymMatrix<T>& m)
  { return m.UpperTri().MaxAbsElement(); }

#ifdef XLAP
  template <class T> static RT LapNorm(const char c, 
      const GenSymMatrix<T>& m)
  {
    switch(c) {
      case 'M' : return NonLapMaxAbsElement(m);
      case '1' : return NonLapNorm1(m);
      case 'F' : return NonLapNormF(m);
      default : TMVAssert(FALSE);
    }
    return RT(0);
  }
#ifdef INST_DOUBLE
  template <> double LapNorm(const char c, const GenSymMatrix<double>& m)
  {
    TMVAssert(m.iscm() || m.isrm());
    char cc = c;
    int N = m.size();
    int lda = m.iscm() ? m.stepj() : m.stepi();
#ifndef LAPNOWORK
    auto_array<double> work(cc == '1' ? new double[N] : 0);
#endif
    double norm = LAPNAMEX(dlansy) (LAPCM LAPV(cc),
	(m.iscm() == (m.uplo()==Upper) ? LAPCH_UP : LAPCH_LO),
	LAPV(N),LAPP(m.cptr()),LAPV(lda) LAPWK(work.get()) LAP1 LAP1);
    return norm;
  }
  template <> double LapNorm(const char c, 
      const GenSymMatrix<std::complex<double> >& m)
  {
    TMVAssert(m.iscm() || m.isrm());
    char cc = c;
    int N = m.size();
    int lda = m.iscm() ? m.stepj() : m.stepi();
#ifndef LAPNOWORK
    auto_array<double> work(cc == '1' ? new double[N] : 0);
#endif
    double norm = m.isherm() ?
      LAPNAMEX(zlanhe) (LAPCM LAPV(cc),
	  (m.iscm() == (m.uplo()==Upper) ? LAPCH_UP : LAPCH_LO),
	  LAPV(N),LAPP(m.cptr()),LAPV(lda) LAPWK(work.get()) LAP1 LAP1) :
      LAPNAMEX(zlansy) (LAPCM LAPV(cc),
	  (m.iscm() == (m.uplo()==Upper) ? LAPCH_UP : LAPCH_LO),
	  LAPV(N),LAPP(m.cptr()),LAPV(lda) LAPWK(work.get()) LAP1 LAP1);
    return norm;
  }
#endif
#ifdef INST_FLOAT
  template <> float LapNorm(const char c, const GenSymMatrix<float>& m)
  {
    TMVAssert(m.iscm() || m.isrm());
    char cc = c;
    int N = m.size();
    int lda = m.iscm() ? m.stepj() : m.stepi();
#ifndef LAPNOWORK
    auto_array<float> work(cc == '1' ? new float[N] : 0);
#endif
    float norm = LAPNAMEX(slansy) (LAPCM LAPV(cc),
	(m.iscm() == (m.uplo()==Upper) ? LAPCH_UP : LAPCH_LO),
	LAPV(N),LAPP(m.cptr()),LAPV(lda) LAPWK(work.get()) LAP1 LAP1);
    return norm;
  }
  template <> float LapNorm(const char c, 
      const GenSymMatrix<std::complex<float> >& m)
  {
    TMVAssert(m.iscm() || m.isrm());
    char cc = c;
    int N = m.size();
    int lda = m.iscm() ? m.stepj() : m.stepi();
#ifndef LAPNOWORK
    auto_array<float> work(cc == '1' ? new float[N] : 0);
#endif
    float norm = m.isherm() ?
      LAPNAMEX(clanhe) (LAPCM LAPV(cc),
	  (m.iscm() == (m.uplo()==Upper) ? LAPCH_UP : LAPCH_LO),
	  LAPV(N),LAPP(m.cptr()),LAPV(lda) LAPWK(work.get()) LAP1 LAP1) :
      LAPNAMEX(clansy) (LAPCM LAPV(cc),
	  (m.iscm() == (m.uplo()==Upper) ? LAPCH_UP : LAPCH_LO),
	  LAPV(N),LAPP(m.cptr()),LAPV(lda) LAPWK(work.get()) LAP1 LAP1);
    return norm;
  }
#endif
#endif // XLAP

  template <class T> RT GenSymMatrix<T>::MaxAbsElement() const
  {
#ifdef XLAP
    if ((isrm() && stepi()>0) || (iscm() && stepj()>0))
      return LapNorm('M',*this);
    else
#endif
      return NonLapMaxAbsElement(*this);
  }
  template <class T> RT GenSymMatrix<T>::Norm1() const
  {
#ifdef XLAP
    if ((isrm() && stepi()>0) || (iscm() && stepj()>0))
      return LapNorm('1',*this);
    else
#endif
      return NonLapNorm1(*this);
  }
  template <class T> RT GenSymMatrix<T>::NormF() const
  {
#ifdef XLAP
    if ((isrm() && stepi()>0) || (iscm() && stepj()>0))
      return LapNorm('F',*this);
    else
#endif
      return NonLapNormF(*this);
  }

  template <class T> RT GenSymMatrix<T>::DoNorm2() const
  {
    if (this->colsize() < this->rowsize()) return Transpose().DoNorm2();
    if (size() == 0) return RT(0);
    DiagMatrix<RT> S(this->size());
    if (this->isherm()) {
      HermMatrix<T> m = *this;
      SV_Decompose(m.View(),S.View());
    } else {
      SymMatrix<T> m = *this;
      SV_Decompose(m.View(),S.View());
    }
    return S(0);
  }

  template <class T> RT GenSymMatrix<T>::DoCondition() const
  {
    if (this->colsize() < this->rowsize()) return Transpose().DoNorm2();
    if (size() == 0) return RT(1);
    DiagMatrix<RT> S(this->size());
    if (this->isherm()) {
      HermMatrix<T> m = *this;
      SV_Decompose(m.View(),S.View());
    } else {
      SymMatrix<T> m = *this;
      SV_Decompose(m.View(),S.View());
    }
    return std::abs(S(0)/S(S.size()-1));
  }

  template <class T> ConstSymMatrixView<T> SymMatrixViewOf(
      const T* m, size_t size, UpLoType uplo, StorageType stor)
  {
    TMVAssert2(stor == RowMajor || stor == ColMajor);
    TMVAssert2(size>0);
    if (stor == RowMajor)
      return ConstSymMatrixView<T>(m,size,size,1,Sym,uplo,RowMajor,NonConj);
    else
      return ConstSymMatrixView<T>(m,size,1,size,Sym,uplo,ColMajor,NonConj);
  }

  template <class T> ConstSymMatrixView<T> HermMatrixViewOf(
      const T* m, size_t size, UpLoType uplo, StorageType stor)
  {
    TMVAssert(stor == RowMajor || stor == ColMajor);
    TMVAssert2(size>0);
    if (stor == RowMajor)
      return ConstSymMatrixView<T>(m,size,size,1,Herm,uplo,RowMajor,NonConj);
    else
      return ConstSymMatrixView<T>(m,size,1,size,Herm,uplo,ColMajor,NonConj);
  }


  template <class T> SymMatrixView<T> SymMatrixViewOf(
      T* m, size_t size, UpLoType uplo, StorageType stor)
  {
    TMVAssert(stor == RowMajor || stor == ColMajor);
    if (stor == RowMajor)
      return SymMatrixView<T>(m,size,size,1,Sym,uplo,RowMajor,NonConj
	  FIRSTLAST1(m,m+size*size));
    else
      return SymMatrixView<T>(m,size,1,size,Sym,uplo,ColMajor,NonConj
	  FIRSTLAST1(m,m+size*size));
  }

  template <class T> SymMatrixView<T> HermMatrixViewOf(
      T* m, size_t size, UpLoType uplo, StorageType stor)
  {
    TMVAssert(stor == RowMajor || stor == ColMajor);
    if (stor == RowMajor)
      return SymMatrixView<T>(m,size,size,1,Herm,uplo,RowMajor,NonConj
	  FIRSTLAST1(m,m+size*size));
    else
      return SymMatrixView<T>(m,size,1,size,Herm,uplo,ColMajor,NonConj
	  FIRSTLAST1(m,m+size*size));
  }


  //
  // I/O
  //

  template <bool sym, bool conj, bool rm, bool compact, bool th, class T> 
    static void DoWrite( std::ostream& os, const GenSymMatrix<T>& m,
	RT thresh)
    {
      TMVAssert(m.uplo() == Lower);
      int rowlen1 = compact?1:0;
      int rowlen2 = m.size();
      const int si = m.stepi();
      const int sj = rm?1:m.stepj();
      const T* mrowi = m.cptr();

      if (compact)
	os << (sym ? "S " : "H ") << m.size() << std::endl;
      else
	os << m.size() <<' '<<m.size() << std::endl;

      for(;rowlen2>0;++rowlen1,--rowlen2,mrowi+=si) {
	os << "( ";
	const T* mij = mrowi;

	for(int k=rowlen1;k>0;--k,rm?++mij:mij+=sj) {
	  if (conj) 
	    if (th)
	      os << ' '<<(ABS(*mij)<thresh ? T(0) : CONJ(*mij))<<' ';
	    else
	      os << ' '<<CONJ(*mij)<<' ';
	  else
	    if (th)
	      os << ' '<<(ABS(*mij)<thresh ? T(0) : *mij)<<' ';
	    else
	      os << ' '<<*mij<<' ';
	}

	if (!compact)
	  for(int k=rowlen2;k>0;--k,mij+=si) {
	    if (sym == conj) 
	      if (th)
		os << ' '<<(ABS(*mij)<thresh ? T(0) : CONJ(*mij))<<' ';
	      else
		os << ' '<<CONJ(*mij)<<' ';
	    else
	      if (th)
		os << ' '<<(ABS(*mij)<thresh ? T(0) : *mij)<<' ';
	      else
		os << ' '<<*mij<<' ';
	  }

	os << " )\n";
      }
    }
  
  template <bool rm, bool compact, bool th, class T> static inline void DoWrite1(
      std::ostream& os, const GenSymMatrix<T>& m, T thresh)
  { DoWrite<false,false,rm,compact,th>(os,m,thresh); }

  template <bool rm, bool compact, bool th, class T> static inline void DoWrite1(
      std::ostream& os, const GenSymMatrix<std::complex<T> >& m, T thresh)
  { 
    if (m.issym())
      if (m.isconj())
	DoWrite<true,true,rm,compact,th>(os,m,thresh); 
      else
	DoWrite<true,false,rm,compact,th>(os,m,thresh); 
    else
      if (m.isconj())
	DoWrite<false,true,rm,compact,th>(os,m,thresh); 
      else
	DoWrite<false,false,rm,compact,th>(os,m,thresh); 
  }

  template <class T> void GenSymMatrix<T>::Write(std::ostream& os) const
  {
    if (uplo() == Upper) 
      if (issym()) Transpose().Write(os);
      else Adjoint().Write(os);
    else {
      if (isrm())
	DoWrite1<true,false,false>(os,*this,RT(0));
      else
	DoWrite1<false,false,false>(os,*this,RT(0));
    }
  }

  template <class T> void GenSymMatrix<T>::Write(std::ostream& os,
      RT thresh) const
  {
    if (uplo() == Upper) 
      if (issym()) Transpose().Write(os,thresh);
      else Adjoint().Write(os,thresh);
    else {
      if (isrm())
	DoWrite1<true,false,true>(os,*this,thresh);
      else
	DoWrite1<false,false,true>(os,*this,thresh);
    }
  }

  template <class T> void GenSymMatrix<T>::WriteCompact(std::ostream& os) const
  {
    if (uplo() == Upper) 
      if (issym()) Transpose().WriteCompact(os);
      else Adjoint().WriteCompact(os);
    else {
      if (isrm())
	DoWrite1<true,true,false>(os,*this,RT(0));
      else
	DoWrite1<false,true,false>(os,*this,RT(0));
    }
  }

  template <class T> void GenSymMatrix<T>::WriteCompact(std::ostream& os,
      RT thresh) const
  {
    if (uplo() == Upper) 
      if (issym()) Transpose().WriteCompact(os);
      else Adjoint().WriteCompact(os);
    else {
      if (isrm())
	DoWrite1<true,true,true>(os,*this,thresh);
      else
	DoWrite1<false,true,true>(os,*this,thresh);
    }
  }

  template <class T> class SymMatrixReadError :
    public ReadError
  {
    public :
      int i,j;
      mutable auto_ptr<SymMatrix<T> > m;
      char exp,got;
      size_t s;
      bool is, iseof, isbad;

      SymMatrixReadError(
	  int _i, int _j, const GenSymMatrix<T>& _m,
	  std::istream& _is) throw() :
	ReadError("SymMatrix."),
	i(_i), j(_j), m(new SymMatrix<T>(_m)), exp(0), got(0), s(_m.size()),
	is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      SymMatrixReadError(std::istream& _is) throw() :
	ReadError("SymMatrix."),
	i(0), j(0), m(0), exp(0), got(0), s(0),
	is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      SymMatrixReadError(
	  int _i, int _j, const GenSymMatrix<T>& _m,
	  std::istream& _is, char _e, char _g) throw() :
	ReadError("SymMatrix."),
	i(_i), j(_j), m(new SymMatrix<T>(_m)), exp(_e), got(_g), s(_m.size()),
	is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      SymMatrixReadError(std::istream& _is, char _e, char _g) throw() :
	ReadError("SymMatrix."),
	i(0), j(0), m(0), exp(_e), got(_g), s(0),
	is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      SymMatrixReadError(
	  const GenSymMatrix<T>& _m, std::istream& _is, size_t _s) throw() :
	ReadError("SymMatrix."),
	i(0), j(0), m(new SymMatrix<T>(_m)), exp(0), got(0), s(_s),
	is(_is), iseof(_is.eof()), isbad(_is.bad()) {}

      SymMatrixReadError(const SymMatrixReadError<T>& rhs) :
	i(rhs.i), j(rhs.j), m(rhs.m), exp(rhs.exp), got(rhs.got), s(rhs.s),
	is(rhs.is), iseof(rhs.iseof), isbad(rhs.isbad) {}
      virtual ~SymMatrixReadError() throw() {}

      virtual void Write(std::ostream& os) const throw();
  };

  template <class T> void SymMatrixReadError<T>::Write(
      std::ostream& os) const throw()
  {
    os<<"TMV Read Error: Reading istream input for SymMatrix\n";
    if (exp != got) {
      os<<"Wrong format: expected '"<<exp<<"'";
      if (IsReal(T()) && exp == 'S') os<<" (or 'H')";
      os<<", got '"<<got<<"'.\n";
    }
    if (m.get() && s != m->size()) {
      os<<"Wrong size: expected "<<m->size()<<", got "<<s<<".\n";
    }
    if (!is) {
      if (iseof) {
	os<<"Input stream reached end-of-file prematurely.\n";
      } else if (isbad) {
	os<<"Input stream is corrupted.\n";
      } else {
	os<<"Input stream cannot read next character.\n";
      }
    }
    if (m.get()) {
      os<<"The portion of the SymMatrix which was successfully read is: \n";
      ConstSymMatrixView<T> mm = m->View();
      for(int ii=0;ii<i;++ii) {
	os<<"( ";
	for(int jj=0;jj<(ii<j?i+1:i);++jj)
	  os<<' '<<mm(ii,jj)<<' ';
	os<<" )\n";
      }
      os<<"( ";
      for(int jj=0;jj<j;++jj)
	os<<' '<<mm(i,jj)<<' ';      
      os<<" )\n";
    }
  }

  template <class T> class HermMatrixReadError :
    public ReadError
  {
    public :
      int i,j;
      mutable auto_ptr<HermMatrix<T> > m;
      char exp,got;
      size_t s;
      T dv;
      bool is, iseof, isbad;

      HermMatrixReadError(
	  int _i, int _j, const GenSymMatrix<T>& _m,
	  std::istream& _is) throw() :
	ReadError("HermMatrix."),
	i(_i), j(_j), m(new HermMatrix<T>(_m)), exp(0), got(0), s(_m.size()),
	dv(0), is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      HermMatrixReadError(std::istream& _is) throw() :
	ReadError("HermMatrix."),
	i(0), j(0), m(0), exp(0), got(0), s(0),
	dv(0), is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      HermMatrixReadError(
	  int _i, int _j, const GenSymMatrix<T>& _m,
	  std::istream& _is, char _e, char _g) throw() :
	ReadError("HermMatrix."),
	i(_i), j(_j), m(new HermMatrix<T>(_m)), exp(_e), got(_g), s(_m.size()),
	dv(0), is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      HermMatrixReadError(
	  int _i, int _j, const GenSymMatrix<T>& _m,
	  std::istream& _is, T _dv) throw() :
	ReadError("HermMatrix."),
	i(_i), j(_j), m(new HermMatrix<T>(_m)), exp(0), got(0), s(_m.size()),
	dv(_dv), is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      HermMatrixReadError(std::istream& _is, char _e, char _g) throw() :
	ReadError("HermMatrix."),
	i(0), j(0), m(0), exp(_e), got(_g), s(0),
	dv(0), is(_is), iseof(_is.eof()), isbad(_is.bad()) {}
      HermMatrixReadError(
	  const GenSymMatrix<T>& _m, std::istream& _is, size_t _s) throw() :
	ReadError("HermMatrix."),
	i(0), j(0), m(new HermMatrix<T>(_m)), exp(0), got(0), s(_s),
	dv(0), is(_is), iseof(_is.eof()), isbad(_is.bad()) {}

      HermMatrixReadError(const HermMatrixReadError<T>& rhs) :
	i(rhs.i), j(rhs.j), m(rhs.m), exp(rhs.exp), got(rhs.got), s(rhs.s),
	dv(rhs.dv), is(rhs.is), iseof(rhs.iseof), isbad(rhs.isbad) {}
      virtual ~HermMatrixReadError() throw() {}

      virtual void Write(std::ostream& os) const throw();
  };

  template <class T> void HermMatrixReadError<T>::Write(
      std::ostream& os) const throw()
  {
    os<<"TMV Read Error: Reading istream input for HermMatrix\n";
    if (exp != got) {
      os<<"Wrong format: expected '"<<exp<<"'";
      if (IsReal(T()) && exp == 'H') os<<" (or 'S')";
      os<<", got '"<<got<<"'.\n";
    }
    if (dv != T(0)) {
      os<<"Non-real value found on diagonal: "<<dv<<std::endl;
    }
    if (m.get() && s != m->size()) {
      os<<"Wrong size: expected "<<m->size()<<", got "<<s<<".\n";
    }
    if (!is) {
      if (iseof) {
	os<<"Input stream reached end-of-file prematurely.\n";
      } else if (isbad) {
	os<<"Input stream is corrupted.\n";
      } else {
	os<<"Input stream cannot read next character.\n";
      }
    }
    if (m.get()) {
      os<<"The portion of the HermMatrix which was successfully read is: \n";
      ConstSymMatrixView<T> mm = m->View();
      for(int ii=0;ii<i;++ii) {
	os<<"( ";
	for(int jj=0;jj<(ii<j?i+1:i);++jj)
	  os<<' '<<mm(ii,jj)<<' ';
	os<<" )\n";
      }
      os<<"( ";
      for(int jj=0;jj<j;++jj)
	os<<' '<<mm(i,jj)<<' ';      
      os<<" )\n";
    }
  }

  template <class T, IndexStyle I> void SymMatrixView<T,I>::Read(
      std::istream& is) const
  {
    if (uplo() == Upper) 
      if (this->issym()) Transpose().Read(is);
      else Adjoint().Read(is);
    else {
      TMVAssert(uplo() == Lower);
      char paren;
      T* mrowi = ptr();
      int rowlen = 1;
      const int si = stepi();
      const int sj = stepj();
      for(int i=size();i>0;--i,++rowlen,mrowi+=si) {
	is >> paren;
	if (!is || paren != '(') {
	  if (this->issym())
	    throw SymMatrixReadError<T>(size()-i,0,*this,is,'(',is?paren:'(');
	  else
	    throw HermMatrixReadError<T>(size()-i,0,*this,is,'(',is?paren:'(');
	}
	T* mij = mrowi;
	if (this->isrm()) {
	  for(int k=rowlen;k>0;--k,++mij) {
	    is >> *mij;
	    if (!is)
	      if (this->issym())
		throw SymMatrixReadError<T>(size()-i,rowlen-k,*this,is);
	      else
		throw HermMatrixReadError<T>(size()-i,rowlen-k,*this,is);
	    if (this->isherm() && k==1 && IMAG(*mij) != RT(0))
	      throw HermMatrixReadError<T>(size()-i,rowlen-k,*this,is,*mij);
	  }
	}
	else {
	  for(int k=rowlen;k>0;--k,mij+=sj) {
	    is >> *mij;
	    if (!is)
	      if (this->issym())
		throw SymMatrixReadError<T>(size()-i,rowlen-k,*this,is);
	      else
		throw HermMatrixReadError<T>(size()-i,rowlen-k,*this,is);
	    if (this->isherm() && k==1 && IMAG(*mij) != RT(0))
	      throw HermMatrixReadError<T>(size()-i,rowlen-k,*this,is,*mij);
	  }
	}
	is >> paren;
	if ((!is && i>1)  || paren != ')') {
	  if (this->issym())
	    throw SymMatrixReadError<T>(size()-i,size()-i+1,*this,is,
		')',is?paren:'(');
	  else
	    throw HermMatrixReadError<T>(size()-i,size()-i+1,*this,is,
		')',is?paren:'(');
	}
      }
    }
    if (this->isconj()) ConjugateSelf();
  }

  template <class T, UpLoType U, StorageType S, IndexStyle I> 
    std::istream& operator>>(std::istream& is, 
	auto_ptr<SymMatrix<T,U,S,I> >& m)
    { 
      char sh;
      is >> sh;
      if (!is)
	throw SymMatrixReadError<T>(is);
      if (IsReal(T())) {
	if (sh != 'S' && sh != 'H') 
	  throw SymMatrixReadError<T>(is,'S',sh);
      } else {
	if (sh != 'S') 
	  throw SymMatrixReadError<T>(is,'S',sh);
      }
      size_t size;
      is >> size;
      if (!is) 
	throw SymMatrixReadError<T>(is);
      m.reset(new SymMatrix<T,U,S,I>(size));
      m->View().Read(is); 
      return is;
    }

  template <class T, UpLoType U, StorageType S, IndexStyle I> 
    std::istream& operator>>(std::istream& is, 
	auto_ptr<HermMatrix<T,U,S,I> >& m)
    { 
      char sh;
      is >> sh;
      if (!is)
	throw HermMatrixReadError<T>(is);
      if (IsReal(T())) {
	if (sh != 'S' && sh != 'H') 
	  throw HermMatrixReadError<T>(is,'H',sh);
      } else {
	if (sh != 'H') 
	  throw HermMatrixReadError<T>(is,'H',sh);
      }
      size_t size;
      is >> size;
      if (!is) 
	throw HermMatrixReadError<T>(is);
      m.reset(new HermMatrix<T,U,S,I>(size));
      m->View().Read(is); 
      return is;
    }

  template <class T> std::istream& operator>>(
      std::istream& is, const SymMatrixView<T>& m)
  { 
    char sh;
    is >> sh;
    if (!is)
      if (m.issym())
	throw SymMatrixReadError<T>(is);
      else
	throw HermMatrixReadError<T>(is);
    if (IsReal(T())) {
      if (sh != 'S' && sh != 'H') 
	if (m.issym())
	  throw SymMatrixReadError<T>(is,'S',sh);
	else
	  throw HermMatrixReadError<T>(is,'H',sh);
    } else if (m.issym()) {
      if (sh != 'S') 
	throw SymMatrixReadError<T>(is,'S',sh);
    } else {
      if (sh != 'H') 
	throw HermMatrixReadError<T>(is,'H',sh);
    }
    size_t s;
    is >> s;
    if (!is) 
      if (m.issym())
	throw SymMatrixReadError<T>(is);
      else
	throw HermMatrixReadError<T>(is);
    if (s != m.size())
      if (m.issym())
	throw SymMatrixReadError<T>(m,is,s);
      else
	throw HermMatrixReadError<T>(m,is,s);
    TMVAssert(m.size() == s);
    m.Read(is);
    return is;
  }


#define InstFile "TMV_SymMatrix.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace mv


