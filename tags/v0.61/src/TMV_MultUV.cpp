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
#include "TMV_TriMatrixArithFunc.h"
#include "TMV_TriMatrix.h"
#include "TMV_Vector.h"
#include "TMV_VectorArith.h"
#include "TMV_MultMV.h"

//#define XDEBUG

#ifdef XDEBUG
#include "TMV_MatrixArith.h"
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

namespace tmv {

  template <class T> const T* UpperTriMatrixComposite<T>::cptr() const
  {
    //cout<<"Start Composite cptr\n";
    if (!itsm.get()) {
      itsm.reset(new T[this->size()*this->size()]);
      UpperTriMatrixViewOf(itsm.get(),this->size(),
	  this->stor(), this->dt()) = *this;
    }
    //cout<<"made itsm: "<<itsm.get()<<std::endl;
    return itsm.get();
  }

  template <class T> int UpperTriMatrixComposite<T>::stepi() const 
  { return this->isrm() ? this->size() : 1; }

  template <class T> int UpperTriMatrixComposite<T>::stepj() const 
  { return this->isrm() ? 1 : this->size(); }

  template <class T> const T* LowerTriMatrixComposite<T>::cptr() const
  {
    if (!itsm.get()) {
      itsm.reset(new T[this->size()*this->size()]);
      LowerTriMatrixViewOf(itsm.get(),this->size(),
	  this->stor(), this->dt()) = *this;
    }
    return itsm.get();
  }

  template <class T> int LowerTriMatrixComposite<T>::stepi() const 
  { return this->isrm() ? this->size() : 1; }

  template <class T> int LowerTriMatrixComposite<T>::stepj() const 
  { return this->isrm() ? 1 : this->size(); }


  //
  // Note: All routines here are designed to work even if x,y,A have 
  // the same storage.  So things like x += L*x will be efficient.
  // This also lets you do things like L *= L without a temporary.
  //

  //
  // MultMV
  //

  template <class T1, class T2> struct ProdType 
  { typedef T1 prodtype; };

  template <class T> struct ProdType<T,std::complex<T> >
  { typedef std::complex<T> prodtype; };

  template <bool rm, bool cx, bool ca, class T, class Ta, class Tx>
    static void DoRowAddMultMV(
	const GenUpperTriMatrix<Ta>& A, const GenVector<Tx>& x,
	const VectorView<T>& y)
    {
      //cout<<"RowAddMultMV Upper\n";
      //cout<<"y = "<<y<<endl;
      TMVAssert(A.size() == x.size());
      TMVAssert(A.size() == y.size());
      TMVAssert(y.size() > 0);
      TMVAssert(x.step()==1);
      TMVAssert(y.step()==1);
      TMVAssert(y.ct() == NonConj);
      TMVAssert(!A.isunit());
      TMVAssert(cx == x.isconj());
      TMVAssert(rm == A.isrm());
      TMVAssert(ca == A.isconj());

      const int N = x.size();
      const int sj = rm ? 1 : A.stepj();
      const int ds = A.stepi()+sj;
      const Tx* xi = x.cptr();
      T* yi = y.ptr();
      const Ta* Aii = A.cptr();

      for(int len=N; len>0; --len,++xi,++yi,Aii+=ds) {
	// i = 0..N-1
	// *yi += A.row(i,i,N) * x.SubVector(i,N);
	const Tx* xj = xi;
	const Ta* Aij = Aii;
	register typename ProdType<Ta,Tx>::prodtype temp(0); 
	for(int j=len;j>0;--j,++xj,(rm?++Aij:Aij+=sj))
	  temp += (cx ? CONJ(*xj) : *xj) * (ca ? CONJ(*Aij) : *Aij);
#ifdef TMVFLDEBUG
	TMVAssert(yi >= y.first);
	TMVAssert(yi < y.last);
#endif
	*yi += temp;
      }
      //cout<<"y => "<<y<<endl;
    }

  template <bool rm, class T, class Ta, class Tx> static void RowAddMultMV(
      const GenUpperTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
  { 
    if (x.isconj())
      if (A.isconj())
	DoRowAddMultMV<rm,true,true>(A,x,y);
      else
	DoRowAddMultMV<rm,true,false>(A,x,y);
    else
      if (A.isconj())
	DoRowAddMultMV<rm,false,true>(A,x,y);
      else
	DoRowAddMultMV<rm,false,false>(A,x,y);
  }

  template <bool cm, bool cx, bool ca, class T, class Ta, class Tx> 
    static void DoColAddMultMV(
	const GenUpperTriMatrix<Ta>& A, const GenVector<Tx>& x,
	const VectorView<T>& y)
    {
      //cout<<"ColAddMultMV Upper\n";
      //cout<<"y = "<<y<<endl;
      TMVAssert(A.size() == x.size());
      TMVAssert(A.size() == y.size());
      TMVAssert(y.size() > 0);
      TMVAssert(x.step()==1);
      TMVAssert(y.step()==1);
      TMVAssert(y.ct() == NonConj);
      TMVAssert(!A.isunit());
      TMVAssert(cx == x.isconj());
      TMVAssert(cm == A.iscm());
      TMVAssert(ca == A.isconj());

      const int N = x.size();

      T* y0 = y.ptr();
      const Tx* xj = x.cptr();
      const int si = cm ? 1 : A.stepi();
      const Ta* A0j = A.cptr();

      for(int j=0; j<N; ++j,++xj,A0j+=A.stepj()) if (*xj != T(0)) {
	// y.SubVector(0,j+1) += *xj * A.col(j,0,j+1);
	const Ta* Aij = A0j;
	T* yi = y0;
	const Tx xxj = (cx ? CONJ(*xj) : *xj);
	for(int i=j+1;i>0;--i,++yi,(cm?++Aij:Aij+=si)) {
#ifdef TMVFLDEBUG
	  TMVAssert(yi >= y.first);
	  TMVAssert(yi < y.last);
#endif
	  *yi += xxj * (ca ? CONJ(*Aij) : *Aij);
	}
      }
      //cout<<"y => "<<y<<endl;
    }

  template <bool cm, class T, class Ta, class Tx> static void ColAddMultMV(
      const GenUpperTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
  { 
    if (x.isconj())
      if (A.isconj())
	DoColAddMultMV<cm,true,true>(A,x,y);
      else
	DoColAddMultMV<cm,true,false>(A,x,y);
    else
      if (A.isconj())
	DoColAddMultMV<cm,false,true>(A,x,y);
      else
	DoColAddMultMV<cm,false,false>(A,x,y);
  }

  template <bool rm, bool cx, bool ca, class T, class Ta, class Tx> 
    static void DoRowAddMultMV(
	const GenLowerTriMatrix<Ta>& A, const GenVector<Tx>& x,
	const VectorView<T>& y)
    {
      //cout<<"RowAddMultMV Lower\n";
      //cout<<"y = "<<y<<endl;
      TMVAssert(A.size() == x.size());
      TMVAssert(A.size() == y.size());
      TMVAssert(y.size() > 0);
      TMVAssert(x.step()==1);
      TMVAssert(y.step()==1);
      TMVAssert(y.ct() == NonConj);
      TMVAssert(!A.isunit());
      TMVAssert(cx == x.isconj());
      TMVAssert(rm == A.isrm());
      TMVAssert(ca == A.isconj());

      const int N = x.size();
      const int sj = rm ? 1 : A.stepj();
      const int si = A.stepi();
      const Tx* x0 = x.cptr();
      T* yi = y.ptr() + N-1;
      const Ta* Ai0 = A.cptr()+(N-1)*A.stepi();

      for(int len=N; len>0; --len,--yi,Ai0-=si) {
	// i = N-1..0
	// *yi += A.row(i,0,i+1) * x.SubVector(0,i+1);
	const Tx* xj = x0;
	const Ta* Aij = Ai0;
	register typename ProdType<Ta,Tx>::prodtype temp(0); 
	for(int j=len;j>0;--j,++xj,(rm?++Aij:Aij+=sj)) {
	  temp += (cx ? CONJ(*xj) : *xj) * (ca ? CONJ(*Aij) : *Aij);
	}
#ifdef TMVFLDEBUG
	TMVAssert(yi >= y.first);
	TMVAssert(yi < y.last);
#endif
	*yi += temp;
      }
      //cout<<"y => "<<y<<endl;
    }

  template <bool rm, class T, class Ta, class Tx> static void RowAddMultMV(
      const GenLowerTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
  { 
    if (x.isconj())
      if (A.isconj())
	DoRowAddMultMV<rm,true,true>(A,x,y);
      else
	DoRowAddMultMV<rm,true,false>(A,x,y);
    else
      if (A.isconj())
	DoRowAddMultMV<rm,false,true>(A,x,y);
      else
	DoRowAddMultMV<rm,false,false>(A,x,y);
  }

  template <bool cm, bool cx, bool ca, class T, class Ta, class Tx> 
    static void DoColAddMultMV(
	const GenLowerTriMatrix<Ta>& A, const GenVector<Tx>& x,
	const VectorView<T>& y)
    {
      //cout<<"ColAddMultMV Lower\n";
      //cout<<"y = "<<y<<endl;
      TMVAssert(A.size() == x.size());
      TMVAssert(A.size() == y.size());
      TMVAssert(y.size() > 0);
      TMVAssert(x.step()==1);
      TMVAssert(y.step()==1);
      TMVAssert(y.ct() == NonConj);
      TMVAssert(!A.isunit());
      TMVAssert(cx == x.isconj());
      TMVAssert(cm == A.iscm());
      TMVAssert(ca == A.isconj());

      const int N = x.size();

      T* yj = y.ptr() + N-1;
      const Tx* xj = x.cptr() + N-1;
      const int si = cm ? 1 : A.stepi();
      const int ds = A.stepj()+si;
      const Ta* Ajj = A.cptr()+(N-1)*ds;

      for(int j=N,len=1;j>0;--j,++len,--yj,--xj,Ajj-=ds) if (*xj!=T(0)) {
	// y.SubVector(j,N) += *xj * A.col(j,j,N);
	T* yi = yj;
	const Ta* Aij = Ajj;
	const Tx xxj = (cx ? CONJ(*xj) : *xj);
	for (int i=len;i>0;--i,++yi,(cm?++Aij:Aij+=si)) {
#ifdef TMVFLDEBUG
	  TMVAssert(yi >= y.first);
	  TMVAssert(yi < y.last);
#endif
	  *yi += xxj * (ca ? CONJ(*Aij) : *Aij);
	}
      }
      //cout<<"y => "<<y<<endl;
    }

  template <bool cm, class T, class Ta, class Tx> static void ColAddMultMV(
      const GenLowerTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
  { 
    if (x.isconj())
      if (A.isconj())
	DoColAddMultMV<cm,true,true>(A,x,y);
      else
	DoColAddMultMV<cm,true,false>(A,x,y);
    else
      if (A.isconj())
	DoColAddMultMV<cm,false,true>(A,x,y);
      else
	DoColAddMultMV<cm,false,false>(A,x,y);
  }

  template <class T, class Ta, class Tx> static void DoAddMultMV(
      const GenUpperTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
  {
    if (A.isunit()) {
      const int N = y.size();
      if (SameStorage(x,y) && N>1) {
	Vector<Tx> xx = x;
	AddMultMV(A.OffDiag(),xx.SubVector(1,N),y.SubVector(0,N-1));
	y += xx;
      } else {
	if (N > 1) 
	  AddMultMV(A.OffDiag(),x.SubVector(1,N),y.SubVector(0,N-1));
	y += x;
      }
    } else {
      if (A.isrm()) RowAddMultMV<true>(A,x,y);
      else if (A.iscm()) ColAddMultMV<true>(A,x,y);
      else RowAddMultMV<false>(A,x,y);
    }
  }

  template <class T, class Ta, class Tx> static void AddMultMV(
      const GenUpperTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
    // y += A * x
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(A.size() == y.size());
    TMVAssert(y.size() > 0);
    TMVAssert(x.step() == 1);
    TMVAssert(y.step() == 1);
    TMVAssert(y.ct() == NonConj);

    //      [ A11 A12 A13 ] [ 0  ]   [ A12 x2 ]
    // y += [  0  A22 A23 ] [ x2 ] = [ A22 x2 ]
    //      [  0   0  A33 ] [ 0  ]   [   0    ]

    const int N = x.size(); // = A.size()
    int j2 = N;
    for(const Tx* x2=x.cptr()+N-1; j2>0 && *x2==Tx(0); --j2,--x2);
    if (j2 == 0) return;
    int j1 = 0;
    for(const Tx* x1=x.cptr(); *x1==Tx(0); ++j1,++x1);
    if (j1 == 0 && j2 == N) {
      DoAddMultMV(A,x,y);
    } else {
      TMVAssert(j1 < j2);
      ConstUpperTriMatrixView<Ta> A22 = A.SubTriMatrix(j1,j2);
      ConstVectorView<Tx> x2 = x.SubVector(j1,j2);
      VectorView<T> y2 = y.SubVector(j1,j2);

      if (j1 != 0) {
	ConstMatrixView<Ta> A12 = A.SubMatrix(0,j1,j1,j2);
	VectorView<T> y1 = y.SubVector(0,j1);
	if (x.isconj())
	  UnitAMultMV1<true,true>(A12,x2,y1);
	else
	  UnitAMultMV1<true,false>(A12,x2,y1);
      }
      DoAddMultMV(A22,x2,y2);
    }
  }

  template <class T, class Ta, class Tx> static void DoAddMultMV(
      const GenLowerTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
    // y += A * x
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(A.size() == y.size());
    TMVAssert(y.size() > 0);
    TMVAssert(x.step() == 1);
    TMVAssert(y.step() == 1);

    if (A.isunit()) {
      const int N = y.size();
      if (SameStorage(x,y) && N>1) {
	Vector<Tx> xx = x;
	AddMultMV(A.OffDiag(),x.SubVector(0,N-1),y.SubVector(1,N));
	y += xx;
      } else {
	if (N > 1) 
	  AddMultMV(A.OffDiag(),x.SubVector(0,N-1),y.SubVector(1,N));
	y += x;
      }
    } else {
      if (A.isrm()) RowAddMultMV<true>(A,x,y);
      else if (A.iscm()) ColAddMultMV<true>(A,x,y);
      else RowAddMultMV<false>(A,x,y);
    }
  }

  template <class T, class Ta, class Tx> static void AddMultMV(
      const GenLowerTriMatrix<Ta>& A, const GenVector<Tx>& x,
      const VectorView<T>& y)
    // y += A * x
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(A.size() == y.size());
    TMVAssert(y.size() > 0);
    TMVAssert(x.step() == 1);
    TMVAssert(y.step() == 1);
    TMVAssert(y.ct() == NonConj);

    //      [ A11  0   0  ] [ 0  ]   [   0    ]
    // y += [ A21 A22  0  ] [ x2 ] = [ A22 x2 ]
    //      [ A31 A32 A33 ] [ 0  ]   [ A32 x2 ]

    const int N = x.size(); // = A.size()
    int j2 = N;
    for(const Tx* x2=x.cptr()+N-1; j2>0 && *x2==Tx(0); --j2,--x2);
    if (j2 == 0) return;
    int j1 = 0;
    for(const Tx* x1=x.cptr(); *x1==Tx(0); ++j1,++x1);
    if (j1 == 0 && j2 == N) {
      DoAddMultMV(A,x,y);
    } else {
      TMVAssert(j1 < j2);
      ConstLowerTriMatrixView<Ta> A22 = A.SubTriMatrix(j1,j2);
      ConstVectorView<Tx> x2 = x.SubVector(j1,j2);
      VectorView<T> y2 = y.SubVector(j1,j2);

      if (j2 != N) {
	ConstMatrixView<Ta> A32 = A.SubMatrix(j2,N,j1,j2);
	VectorView<T> y3 = y.SubVector(j2,N);
	if (x.isconj())
	  UnitAMultMV1<true,true>(A32,x2,y3);
	else
	  UnitAMultMV1<true,false>(A32,x2,y3);
      }
      DoAddMultMV(A22,x2,y2);
    }
  }

  // 
  // MultEqMV
  //

  template <bool rm, bool ca, bool ua, class T, class Ta> 
    static void DoRowMultEqMV(
	const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
    {
      //cout<<"RowMultEqMV Upper\n";
      TMVAssert(x.step()==1);
      TMVAssert(A.size() == x.size());
      TMVAssert(x.size() > 0);
      TMVAssert(x.ct() == NonConj);
      TMVAssert(rm == A.isrm());
      TMVAssert(ca == A.isconj());
      TMVAssert(ua == A.isunit());

      const int N = x.size();

      const int sj = rm ? 1 : A.stepj();
      const int ds = A.stepi()+sj;
      T* xi = x.ptr();
      const Ta* Aii = A.cptr();
      int len = N-1;

      for(; len>0; --len,++xi,Aii+=ds) {
	// i = 0..N-2
	// x(i) = A.row(i,i,N) * x.SubVector(i,N);
	if (!ua) *xi *= (ca ? CONJ(*Aii) : *Aii);
	const T* xj = xi+1;
	const Ta* Aij = Aii+sj;
	for(int j=len;j>0;--j,++xj,(rm?++Aij:Aij+=sj)) {
#ifdef TMVFLDEBUG
	  TMVAssert(xi >= x.first);
	  TMVAssert(xi < x.last);
#endif
	  *xi += (*xj) * (ca ? CONJ(*Aij) : *Aij);
	}
      }
#ifdef TMVFLDEBUG
      TMVAssert(xi >= x.first);
      TMVAssert(xi < x.last);
#endif
      if (!ua) *xi *= (ca ? CONJ(*Aii) : *Aii);
    }

  template <bool rm, class T, class Ta> static void RowMultEqMV(
      const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
  { 
    if (A.isconj())
      if (A.isunit())
	DoRowMultEqMV<rm,true,true>(A,x);
      else
	DoRowMultEqMV<rm,true,false>(A,x);
    else
      if (A.isunit())
	DoRowMultEqMV<rm,false,true>(A,x);
      else
	DoRowMultEqMV<rm,false,false>(A,x);
  }

  template <bool cm, bool ca, bool ua, class T, class Ta> 
    static void DoColMultEqMV(
	const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
    {
      //cout<<"ColMultEqMV Upper\n";
      TMVAssert(x.step()==1);
      TMVAssert(A.size() == x.size());
      TMVAssert(x.size() > 0);
      TMVAssert(x.ct() == NonConj);
      TMVAssert(cm == A.iscm());
      TMVAssert(ca == A.isconj());
      TMVAssert(ua == A.isunit());

      const int N = x.size();

      T* x0 = x.ptr();
      const T* xj = x0+1;
      const int si = cm ? 1 : A.stepi();
      const Ta* A0j = A.cptr();

#ifdef TMVFLDEBUG
      TMVAssert(x0 >= x.first);
      TMVAssert(x0 < x.last);
#endif
      if (!ua) *x0 *= (ca ? CONJ(*A0j) : *A0j);
      A0j += A.stepj();

      for(int len=1; len<N; ++len,++xj,A0j+=A.stepj()) if (*xj != T(0)) {
	// j = 1..N-1
	// x.SubVector(0,j) += *xj * A.col(j,0,j);
	const Ta* Aij = A0j;
	T* xi = x0;
	for(int i=len;i>0;--i,++xi,(cm?++Aij:Aij+=si)) {
#ifdef TMVFLDEBUG
	  TMVAssert(xi >= x.first);
	  TMVAssert(xi < x.last);
#endif
	  *xi += *xj * (ca ? CONJ(*Aij) : *Aij);
	}
	// Now Aij == Ajj, xi == xj
	// so this next statement is really *xj *= *Ajj
#ifdef TMVFLDEBUG
	  TMVAssert(xi >= x.first);
	  TMVAssert(xi < x.last);
#endif
	if (!ua) *xi *= (ca ? CONJ(*Aij) : *Aij);
      }
    }

  template <bool cm, class T, class Ta> static void ColMultEqMV(
      const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
  { 
    if (A.isconj())
      if (A.isunit())
	DoColMultEqMV<cm,true,true>(A,x);
      else
	DoColMultEqMV<cm,true,false>(A,x);
    else
      if (A.isunit())
	DoColMultEqMV<cm,false,true>(A,x);
      else
	DoColMultEqMV<cm,false,false>(A,x);
  }

  template <bool rm, bool ca, bool ua, class T, class Ta> 
    static void DoRowMultEqMV(
	const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
    {
      //cout<<"RowMultEqMV Lower\n";
      TMVAssert(x.step()==1);
      TMVAssert(A.size() == x.size());
      TMVAssert(x.size() > 0);
      TMVAssert(x.ct() == NonConj);
      TMVAssert(rm == A.isrm());
      TMVAssert(ca == A.isconj());
      TMVAssert(ua == A.isunit());

      const int N = x.size();
      const int si = A.stepi();
      const int sj = rm ? 1 : A.stepj();
      const int ds = si+sj;

      const T* x0 = x.cptr();
      T* xi = x.ptr() + N-1;
      const Ta* Ai0 = A.cptr()+(N-1)*si;
      const Ta* Aii = Ai0 + (N-1)*sj;

      for(int len=N-1; len>0; --len,--xi,Ai0-=si,Aii-=ds) {
	// i = N-1..1
	// x(i) = A.row(i,0,i+1) * x.SubVector(0,i+1);
	T xx = *xi;
	if (!ua) xx *= (ca ? CONJ(*Aii) : *Aii);
	const Ta* Aij = Ai0;
	const T* xj = x0;
	for(int j=len;j>0;--j,++xj,(rm?++Aij:Aij+=sj))
	  xx += *xj * (ca ? CONJ(*Aij) : *Aij);
#ifdef TMVFLDEBUG
	TMVAssert(xi >= x.first);
	TMVAssert(xi < x.last);
#endif
	*xi = xx;
      }
#ifdef TMVFLDEBUG
      TMVAssert(xi >= x.first);
      TMVAssert(xi < x.last);
#endif
      if (!ua) *xi *= (ca ? CONJ(*Aii) : *Aii);
    }

  template <bool rm, class T, class Ta> static void RowMultEqMV(
      const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
  { 
    if (A.isconj())
      if (A.isunit())
	DoRowMultEqMV<rm,true,true>(A,x);
      else
	DoRowMultEqMV<rm,true,false>(A,x);
    else
      if (A.isunit())
	DoRowMultEqMV<rm,false,true>(A,x);
      else
	DoRowMultEqMV<rm,false,false>(A,x);
  }

  template <bool cm, bool ca, bool ua, class T, class Ta> 
    static void DoColMultEqMV(
	const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
    {
      //cout<<"ColMultEqMV Lower\n";
      TMVAssert(A.size() == x.size());
      TMVAssert(x.size() > 0);
      TMVAssert(x.ct() == NonConj);
      TMVAssert(x.step() == 1);
      TMVAssert(cm == A.iscm());
      TMVAssert(ca == A.isconj());
      TMVAssert(ua == A.isunit());

      const int N = x.size();

      T* xj = x.ptr() + N-2;
      const int si = cm ? 1 : A.stepi();
      const int ds = A.stepj()+si;
      const Ta* Ajj = A.cptr()+(N-2)*ds;

#ifdef TMVFLDEBUG
      TMVAssert(xj+1 >= x.first);
      TMVAssert(xj+1 < x.last);
#endif
      if (!ua) *(xj+1) *= (ca ? CONJ(*(Ajj+ds)) : *(Ajj+ds));
      for(int jj=N-1,len=1;jj>0;--jj,++len,--xj,Ajj-=ds) if (*xj!=T(0)) {
	// j = N-2..0, jj = j+1
	// x.SubVector(j+1,N) += *xj * A.col(j,j+1,N);
	T* xi = xj+1;
	const Ta* Aij = Ajj+si;
	for (int i=len;i>0;--i,++xi,(cm?++Aij:Aij+=si)) {
#ifdef TMVFLDEBUG
	  TMVAssert(xi >= x.first);
	  TMVAssert(xi < x.last);
#endif
	  *xi += *xj * (ca ? CONJ(*Aij) : *Aij);
	}
#ifdef TMVFLDEBUG
	TMVAssert(xj >= x.first);
	TMVAssert(xj < x.last);
#endif
	if (!ua) *xj *= (ca ? CONJ(*Ajj) : *Ajj);
      }
    }

  template <bool cm, class T, class Ta> static void ColMultEqMV(
      const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
  { 
    if (A.isconj())
      if (A.isunit())
	DoColMultEqMV<cm,true,true>(A,x);
      else
	DoColMultEqMV<cm,true,false>(A,x);
    else
      if (A.isunit())
	DoColMultEqMV<cm,false,true>(A,x);
      else
	DoColMultEqMV<cm,false,false>(A,x);
  }

  template <class T, class Ta> static inline void DoMultEqMV(
      const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
    // x = A * x
  {
    if (A.isrm()) RowMultEqMV<true>(A,x);
    else if (A.iscm()) ColMultEqMV<true>(A,x);
    else RowMultEqMV<false>(A,x);
  }

  template <class T, class Ta> static void NonBlasMultEqMV(
      const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.step() == 1);
    TMVAssert(x.ct() == NonConj);

    //     [ A11 A12 A13 ] [ 0  ]   [ A12 x2 ]
    // x = [  0  A22 A23 ] [ x2 ] = [ A22 x2 ]
    //     [  0   0  A33 ] [ 0  ]   [   0    ]

    const int N = x.size(); // = A.size()
    int j2 = N;
    for(const T* x2=x.cptr()+N-1; j2>0 && *x2==T(0); --j2,--x2);
    if (j2 == 0) return;
    int j1 = 0;
    for(const T* x1=x.cptr(); *x1==T(0); ++j1,++x1);
    if (j1 == 0 && j2 == N) {
      DoMultEqMV(A,x);
    } else {
      TMVAssert(j1 < j2);
      ConstUpperTriMatrixView<Ta> A22 = A.SubTriMatrix(j1,j2);
      VectorView<T> x2 = x.SubVector(j1,j2);

      if (j1 != 0) {
	ConstMatrixView<Ta> A12 = A.SubMatrix(0,j1,j1,j2);
	VectorView<T> x1 = x.SubVector(0,j1);
	UnitAMultMV1<true,false>(A12,x2,x1);
      }
      DoMultEqMV(A22,x2);
    }
  }

  template <class T, class Ta> static inline void DoMultEqMV(
      const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
  {
    if (A.isrm()) RowMultEqMV<true>(A,x);
    else if (A.iscm() && !SameStorage(A,x))
      ColMultEqMV<true>(A,x);
    else RowMultEqMV<false>(A,x);
  }

  template <class T, class Ta> static void NonBlasMultEqMV(
      const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
    // x = A * x
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.step() == 1);
    TMVAssert(x.ct() == NonConj);

    //     [ A11  0   0  ] [ 0  ]   [   0    ]
    // x = [ A21 A22  0  ] [ x2 ] = [ A22 x2 ]
    //     [ A31 A32 A33 ] [ 0  ]   [ A32 x2 ]

    const int N = x.size(); // = A.size()
    int j2 = N;
    for(const T* x2=x.cptr()+N-1; j2>0 && *x2==T(0); --j2,--x2);
    if (j2 == 0) return;
    int j1 = 0;
    for(const T* x1=x.cptr(); *x1==T(0); ++j1,++x1);
    if (j1 == 0 && j2 == N) {
      DoMultEqMV(A,x);
    } else {
      TMVAssert(j1 < j2);
      ConstLowerTriMatrixView<Ta> A22 = A.SubTriMatrix(j1,j2);
      VectorView<T> x2 = x.SubVector(j1,j2);

      if (j2 != N) {
	ConstMatrixView<Ta> A32 = A.SubMatrix(j2,N,j1,j2);
	VectorView<T> x3 = x.SubVector(j2,N);
	UnitAMultMV1<true,false>(A32,x2,x3);
      }
      DoMultEqMV(A22,x2);
    }
  }

#ifdef BLAS
  template <class T, class Ta> static inline void BlasMultEqMV(
      const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
  { NonBlasMultEqMV(A,x); }
  template <class T, class Ta> static inline void BlasMultEqMV(
      const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
  { NonBlasMultEqMV(A,x); }
#ifdef INST_DOUBLE
  template <> void BlasMultEqMV( 
      const GenUpperTriMatrix<double>& A, const VectorView<double>& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    BLASNAME(dtrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
  template <> void BlasMultEqMV( 
      const GenLowerTriMatrix<double>& A, const VectorView<double>& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    BLASNAME(dtrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
  template <> void BlasMultEqMV(
      const GenUpperTriMatrix<std::complex<double> >& A,
      const VectorView<std::complex<double> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    if (A.iscm() && A.isconj()) {
#ifdef CBLAS
      BLASNAME(ztrmv) (BLASRM BLASCH_LO, BLASCH_CT,
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
#else
      x.ConjugateSelf();
      BLASNAME(ztrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	  A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
      x.ConjugateSelf();
#endif
    } else {
      BLASNAME(ztrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	  A.iscm()?BLASCH_NT:A.isconj()?BLASCH_CT:BLASCH_T, 
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    }
  }
  template <> void BlasMultEqMV(
      const GenLowerTriMatrix<std::complex<double> >& A,
      const VectorView<std::complex<double> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    if (A.iscm() && A.isconj()) {
#ifdef CBLAS
      BLASNAME(ztrmv) (BLASRM BLASCH_UP, BLASCH_CT,
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
#else
      x.ConjugateSelf();
      BLASNAME(ztrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	  A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
      x.ConjugateSelf();
#endif
    } else {
      BLASNAME(ztrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	  A.iscm()?BLASCH_NT:A.isconj()?BLASCH_CT:BLASCH_T, 
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    }
  }
  template <> void BlasMultEqMV( 
      const GenUpperTriMatrix<double>& A,
      const VectorView<std::complex<double> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=2*x.step();
    BLASNAME(dtrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((double*)x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    BLASNAME(dtrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((double*)x.ptr()+1),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
  template <> void BlasMultEqMV( 
      const GenLowerTriMatrix<double>& A,
      const VectorView<std::complex<double> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=2*x.step();
    BLASNAME(dtrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((double*)x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    BLASNAME(dtrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((double*)x.ptr()+1),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
#endif
#ifdef INST_FLOAT
  template <> void BlasMultEqMV( 
      const GenUpperTriMatrix<float>& A, const VectorView<float>& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    BLASNAME(strmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
  template <> void BlasMultEqMV( 
      const GenLowerTriMatrix<float>& A, const VectorView<float>& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    BLASNAME(strmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
  template <> void BlasMultEqMV(
      const GenUpperTriMatrix<std::complex<float> >& A,
      const VectorView<std::complex<float> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    if (A.iscm() && A.isconj()) {
#ifdef CBLAS
      BLASNAME(ctrmv) (BLASRM BLASCH_LO, BLASCH_CT,
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
#else
      x.ConjugateSelf();
      BLASNAME(ctrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	  A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
      x.ConjugateSelf();
#endif
    } else {
      BLASNAME(ctrmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	  A.iscm()?BLASCH_NT:A.isconj()?BLASCH_CT:BLASCH_T, 
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    }
  }
  template <> void BlasMultEqMV(
      const GenLowerTriMatrix<std::complex<float> >& A,
      const VectorView<std::complex<float> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=x.step();
    if (A.iscm() && A.isconj()) {
#ifdef CBLAS
      BLASNAME(ctrmv) (BLASRM BLASCH_UP, BLASCH_CT,
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
#else
      x.ConjugateSelf();
      BLASNAME(ctrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	  A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
      x.ConjugateSelf();
#endif
    } else {
      BLASNAME(ctrmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	  A.iscm()?BLASCH_NT:A.isconj()?BLASCH_CT:BLASCH_T, 
	  A.isunit()?BLASCH_U:BLASCH_NU,
	  BLASV(n),BLASP(A.cptr()),BLASV(lda),
	  BLASP(x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    }
  }
  template <> void BlasMultEqMV( 
      const GenUpperTriMatrix<float>& A,
      const VectorView<std::complex<float> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=2*x.step();
    BLASNAME(strmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((float*)x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    BLASNAME(strmv) (BLASCM A.iscm()?BLASCH_UP:BLASCH_LO,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((float*)x.ptr()+1),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
  template <> void BlasMultEqMV( 
      const GenLowerTriMatrix<float>& A,
      const VectorView<std::complex<float> >& x)
  {
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(x.ct() == NonConj);

    int n=A.size();
    int lda=A.isrm()?A.stepi():A.stepj();
    int xs=2*x.step();
    BLASNAME(strmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((float*)x.ptr()),BLASV(xs) BLAS1 BLAS1 BLAS1);
    BLASNAME(strmv) (BLASCM A.iscm()?BLASCH_LO:BLASCH_UP,
	A.iscm()?BLASCH_NT:BLASCH_T, A.isunit()?BLASCH_U:BLASCH_NU,
	BLASV(n),BLASP(A.cptr()),BLASV(lda),
	BLASP((float*)x.ptr()+1),BLASV(xs) BLAS1 BLAS1 BLAS1);
  }
#endif // FLOAT
#endif // BLAS

  template <class T, class Ta> static void MultEqMV(
      const GenUpperTriMatrix<Ta>& A, const VectorView<T>& x)
  {
#ifdef XDEBUG
    Vector<T> x0 = x;
    Matrix<Ta> A0 = A;
    Vector<T> x2 = A0 * x0;
    //cout<<"MultEqMV: A = "<<A<<"x = "<<x<<endl;
#endif
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.step() == 1);

    if (x.isconj()) MultEqMV(A.Conjugate(),x.Conjugate());
    else {
#ifdef BLAS
      if ((A.isrm() && A.stepi()>0) || (A.iscm() && A.stepj()>0)) 
	BlasMultEqMV(A,x);
      else {
	if (A.isunit()) {
	  UpperTriMatrix<Ta,UnitDiag,RowMajor> A2(A);
	  BlasMultEqMV(A2,x);
	} else {
	  UpperTriMatrix<Ta,NonUnitDiag,RowMajor> A2(A);
	  BlasMultEqMV(A2,x);
	}
      }
#else
      NonBlasMultEqMV(A,x);
#endif
    }
#ifdef XDEBUG
    //cout<<"-> x = "<<x<<endl<<"x2 = "<<x2<<endl;
    if (Norm(x-x2) > 0.001*(Norm(A0)*Norm(x0))) {
      cerr<<"TriMultEqMV: \n";
      cerr<<"A = "<<A.cptr()<<"  "<<Type(A)<<"  "<<A0<<endl;
      cerr<<"x = "<<x.cptr()<<"  "<<Type(x)<<" step "<<x.step()<<"  "<<x0<<endl;
      cerr<<"-> x = "<<x<<endl;
      cerr<<"x2 = "<<x2<<endl;
      abort();
    }
#endif
  }

  template <class T, class Ta> static void MultEqMV(
      const GenLowerTriMatrix<Ta>& A, const VectorView<T>& x)
  {
#ifdef XDEBUG
    Vector<T> x0 = x;
    Matrix<Ta> A0 = A;
    Vector<T> x2 = A0 * x0;
#endif
    TMVAssert(A.size() == x.size());
    TMVAssert(x.size() > 0);
    TMVAssert(x.step() == 1);

    if (x.isconj()) MultEqMV(A.Conjugate(),x.Conjugate());
    else {
#ifdef BLAS
      if ( (A.isrm() && A.stepi()>0) || (A.iscm() && A.stepj()>0) )
	BlasMultEqMV(A,x);
      else {
	if (A.isunit()) {
	  LowerTriMatrix<Ta,UnitDiag,RowMajor> A2(A);
	  BlasMultEqMV(A2,x);
	} else {
	  LowerTriMatrix<Ta,NonUnitDiag,RowMajor> A2(A);
	  BlasMultEqMV(A2,x);
	}
      }
#else
      NonBlasMultEqMV(A,x);
#endif
    }

#ifdef XDEBUG
    if (Norm(x-x2) > 0.001*(Norm(A0)*Norm(x0))) {
      cerr<<"TriMultEqMV: \n";
      cerr<<"A = "<<A.cptr()<<"  "<<Type(A)<<"  "<<A0<<endl;
      cerr<<"x = "<<x.cptr()<<"  "<<Type(x)<<" step "<<x.step()<<"  "<<x0<<endl;
      cerr<<"-> x = "<<x<<endl;
      cerr<<"x2 = "<<x2<<endl;
      abort();
    }
#endif
  }

  template <bool add, class T, class Ta, class Tx> void MultMV(
      const T alpha, const GenUpperTriMatrix<Ta>& A,
      const GenVector<Tx>& x, const VectorView<T>& y)
    // y (+)= alpha * A * x
  { 
#ifdef XDEBUG
    Vector<Tx> x0 = x;
    Vector<T> y0 = y;
    Matrix<Ta> A0 = A;
    Vector<T> y2 = alpha*A0*x0;
    if (add) y2 += y0;
#endif
    TMVAssert(A.size() == x.size());
    TMVAssert(A.size() == y.size());

    if (y.size() > 0) {
      if (alpha==T(0)) {
	if (!add) y.Zero();
      } else if (!add && y.step() == 1) {
	y = x;
	MultEqMV(A,y);
	y *= alpha;
      }
      else if (alpha != T(1) || x.step() != 1 || y.step() != 1) {
	Vector<T> xx = alpha*x;
	MultEqMV(A,xx.View());
	if (add) y += xx;
	else y = xx;
      } else {
#ifdef BLAS
	Vector<T> xx = alpha*x;
	MultEqMV(A,xx.View());
	if (add) y += xx;
	else y = xx;
#else
	if (!add) y.Zero();
	AddMultMV(A,x,y);
#endif
      }
    } 
#ifdef XDEBUG
    if (Norm(y-y2) > 0.001*(ABS(alpha)*Norm(A0)*Norm(x0)+
	  (add?Norm(y0):RealType(T)(0)))) {
      cerr<<"TriMultMV: alpha = "<<alpha<<endl;
      cerr<<"add = "<<add<<endl;
      cerr<<"A = "<<Type(A)<<"  "<<A0<<endl;
      cerr<<"x = "<<Type(x)<<" step "<<x.step()<<"  "<<x0<<endl;
      cerr<<"y = "<<Type(y)<<" step "<<y.step()<<"  "<<y0<<endl;
      cerr<<"-> y = "<<y<<endl;
      cerr<<"y2 = "<<y2<<endl;
      abort();
    }
#endif
  }

  template <bool add, class T, class Ta, class Tx> void MultMV(
      const T alpha, const GenLowerTriMatrix<Ta>& A,
      const GenVector<Tx>& x, const VectorView<T>& y)
    // y (+)= alpha * A * x
  { 
#ifdef XDEBUG
    Vector<T> y0 = y;
    Vector<Tx> x0 = x;
    Matrix<Ta> A0 = A;
    Vector<T> y2 = alpha*A0*x0;
    if (add) y2 += y0;
#endif

    TMVAssert(A.size() == x.size());
    TMVAssert(A.size() == y.size());

    if (y.size() > 0) {
      if (alpha==T(0)) {
	if (!add) y.Zero();
      } else if (!add && y.step() == 1) {
	y = x;
	MultEqMV(A,y);
	if (alpha != T(1)) y *= alpha;
      } else if (alpha != T(1) || x.step() != 1 || y.step() != 1) {
	Vector<T> xx = alpha*x;
	MultEqMV(A,xx.View());
	if (add) y += xx;
	else y = xx;
      } else {
#ifdef BLAS
	Vector<T> xx = alpha*x;
	MultEqMV(A,xx.View());
	if (add) y += xx;
	else y = xx;
#else
	if (!add) y.Zero();
	AddMultMV(A,x,y);
#endif
      }
    }
#ifdef XDEBUG
    if (Norm(y-y2) > 0.001*(ABS(alpha)*Norm(A0)*Norm(x0)+
	  (add?Norm(y0):RealType(T)(0)))) {
      cerr<<"TriMultMV: alpha = "<<alpha<<endl;
      cerr<<"add = "<<add<<endl;
      cerr<<"A = "<<A.cptr()<<"  "<<Type(A)<<"  "<<A0<<endl;
      cerr<<"x = "<<x.cptr()<<"  "<<Type(x)<<" step "<<x.step()<<"  "<<x0<<endl;
      cerr<<"y = "<<y.cptr()<<"  "<<Type(y)<<" step "<<y.step()<<"  "<<y0<<endl;
      cerr<<"-> y = "<<y<<endl;
      cerr<<"y2 = "<<y2<<endl;
      abort();
    }
#endif
  }

#define InstFile "TMV_MultUV.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace mv


