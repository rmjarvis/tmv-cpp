
#include "TMV.h"
#include "TMV_Tri.h"
#include "TMV_Diag.h"
#include "TMV_Sym.h"

//#define XDEBUG

namespace tmv {

  bool RecursiveCH = true;

#ifdef TMV_BLOCKSIZE
  const size_t CH_BLOCKSIZE = TMV_BLOCKSIZE;
  const size_t CH_BLOCKSIZE2 = TMV_BLOCKSIZE/2;
  // MJ: BLOCKSIZE2 needs to be optimized
#else
  const size_t CH_BLOCKSIZE = 64;
  const size_t CH_BLOCKSIZE2 = 32;
#endif

#define APTR inplace ? A.NonConst().ptr() : new T[A.size()*A.size()]
#define LLX \
  inplace ? A.uplo()==Upper ? A.NonConst().Adjoint() : A.NonConst() : \
  HermMatrixViewOf(Aptr,A.size(),Lower,BaseStorOf(A.LowerTri()))

  template <class T> HermCHDiv<T>::HermCHDiv(const GenSymMatrix<T>& A,
      bool _inplace) :
    inplace(_inplace), Aptr(APTR), LLx(LLX), det(T(0)),donedet(false)
  {
    TMVAssert(IsReal(T()) || A.isherm());
    if (inplace) { TMVAssert(A==LLx); }
    else LLx = A;
    HermCH_Decompose(LLx);
  }

  template <class T> HermCHDiv<T>::~HermCHDiv() 
  { if (!inplace) delete[] Aptr; }

#undef APTR
#undef LLX

  template <class T> bool HermCHDiv<T>::CheckDecomp(
      const BaseMatrix<T>& m, ostream* fout) const
  {
    const GenSymMatrix<T>* sm = dynamic_cast<const GenSymMatrix<T>*>(&m);
    TMVAssert(sm);
    if (fout) {
      *fout << "M = "<<tmv::Type(*sm)<<"  "<<*sm<<endl;
      *fout << "L = "<<GetL()<<endl;
    }
    Matrix<T> lu = GetL()*GetL().Adjoint();
    RealType(T) nm = Norm(lu-*sm);
    nm /= SQR(Norm(GetL()));
    if (fout) {
      *fout << "LLt = "<<lu<<endl;
      *fout << "Norm(M-LLt)/Norm(LLt) = "<<nm<<endl;
    }
    HermMatrix<T> sm2 = *sm;
    sm2.DivideUsing(SVS);
    sm2.SetDiv();
    return nm < sm2.Condition()*sm2.size()*Epsilon<T>();
  }

  //
  // LDivEq
  //

  template <class T1, class T2> void HermCH_LDivEq(
      const GenSymMatrix<T1>& LL, const MatrixView<T2>& m)
  {
    TMVAssert(LL.size() == m.colsize());
    // m = (LLT)^-1 m
    //   = Lt^-1 L^-1 m
    m /= LL.LowerTri();
    m /= LL.UpperTri();
  }

  //
  // RDivEq Matrix
  //

  template <class T1, class T2> void HermCH_RDivEq(
      const GenSymMatrix<T1>& LL, const MatrixView<T2>& m)
  {
    TMVAssert(LL.size() == m.rowsize());
    // m = m (LLt)^-1 
    //   = m Lt^-1 L^-1
    m %= LL.UpperTri();
    m %= LL.LowerTri();
  }

  template <class T> T HermCHDiv<T>::Det() const
  {
    if (!donedet) {
      det = DiagMatrixViewOf(LLx.diag()).Det();
      det *= det;
      donedet = true;
    }
    return det;
  }

  template <class T> void HermCHDiv<T>::Inverse(
      const MatrixView<T>& minv) const
  {
    TMVAssert(minv.colsize() == LLx.size());
    TMVAssert(minv.rowsize() == LLx.size());
    minv.SetToIdentity();
    LDivEq(minv);
  }

  template <class T> void HermCHDiv<T>::InverseATA(
      const MatrixView<T>& minv) const
  {
    TMVAssert(minv.colsize() == LLx.size());
    TMVAssert(minv.rowsize() == LLx.size());
    Matrix<T,ColMajor> temp(LLx.size(),LLx.size());
    Inverse(temp.View());
    minv = temp*temp.Adjoint();
  }

  //
  // Decompose
  //

  template <class T> void NonBlockHermCH_Decompose(const SymMatrixView<T>& A)
  {
    // Cholesky Decompostion is basically a simpler version of LU Decomp,
    // since U is just Lt for Hermitian matrices.  Cholesky cannot
    // be used for complex symmetric matrices.
    //
    // Also, the matrix must be positive definite.
    // Positive definite by definition means that:
    // - All eignevalues are positive.
    // Necessary, but not sufficient, conditions of positive definite
    // matrices are:
    // - All diagonal elements are positive.
    // - Aii + Ajj > 2*Re[Aij]
    // - Det(A) > 0
    // - MaxElement(A) is on main diagonal
    // A necessary and sufficient condition is:
    // - det(A(0:i,0:i)) > 0 for all 0<i<=N
    //
    // We want to decompose the matrix (input as A) into L * Lt.
    //
    // The equations for the rows of A are:
    //
    // A(i,0:i)   = L(i,0:i) * Lt(0:i,0:i)
    // A(i,i)     = L(i,0:i) * Lt(0:i,i) + |L(i,i)|^2
    // A(i,i+1:N) = L(i,0:i) * Lt(0:i,i+1:N) + L(i,i) * Lt(i,i+1:N)
    // 
    // These are solved as: (Taking A to be stored in its LowerTriangle part)
    //
    // L(i,0:i) = A(i,0:i) % L(0:i,0:i)t
    // L(i,i) = sqrt( A(i,i) - NormSq(L(i,0:i)) )
    // [ ie. row by row from the top. ]
    //
    // or 
    // 
    // L(i,i) = sqrt( A(i,i) - NormSq(L(i,0:i)) )
    // L(i+1:N,i) = (A(i+1:N,i) - L(i,0:i) * L(i+1:N,0:i)t) / L(i,i)
    // [ ie. col by col from the left ]
    //
    // The second version is not so good as written, since it accesses L
    // by both rows and columns, which means there will be non-unit strided
    // vectors in the algorithm.  However, we can save it by
    // noting that the second equation can be calculated piecemeal:
    //
    // L(i+1:N,i) = A(i+1:N,i)
    // for(j=0..i) L(i+1:N,i) -= L(i,j) * L(i+1:N,j)*
    // L(i+1:N,i) /= L(i,i)
    //
    // (The row access in the first equation can proceed similarly.)
    // 
    // Then note that for a given j, all the i's with i>j can have 
    // L(i+1:N,i) calculated at once.  So the final column algorithm
    // is:
    //
    // L = A.LowerTri();
    // for(j=0..N) 
    //   L(j,j) = sqrt(L(j,j))
    //   L(j+1:N,j) /= L(j,j)
    //   L(j+1:N,j+1:N) -= L(j+1:N,j) ^ L(j+1:N,j)*
    //
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(IsReal(T()) || A.isherm());
    const size_t N = A.size();
#ifdef XDEBUG
    Matrix<T> A0 = A;
    //for(size_t i=1;i<=N;i++) {
    //  cerr<<"Det(0.."<<i<<") = "<<Matrix<T>(A.SubSymMatrix(0,i)).Det()<<endl;
    //}
#endif

    const VectorView<RealType(T)> Adiag = A.Real().diag();
    if (A.isrm()) {
      RealType(T)* Aii= Adiag.ptr();
      const size_t ds = Adiag.step();
      if (*Aii <= RealType(T)(0)) {
	tmv_error(
	  "Non-positive definite Matrix found in Cholesky decomposition.");
      }
      *Aii = sqrt(*Aii);
      Aii+=ds;
      for(size_t i=1; i<N; ++i,Aii+=ds) {
	A.row(i,0,i) %= A.LowerTri().SubTriMatrix(0,i).Adjoint();
	*Aii -= NormSq(A.row(i,0,i));
	if (*Aii <= RealType(T)(0)) {
	  tmv_error(
	    "Non-positive definite Matrix found in Cholesky decomposition.");
	}
	*Aii = sqrt(*Aii);
      }
    } else {
      RealType(T)* Ajj= Adiag.ptr();
      const size_t ds = Adiag.step();
      for(size_t j=0;j<N-1;++j,Ajj+=ds) {
	if (*Ajj <= RealType(T)(0)) {
	  tmv_error(
	    "Non-positive definite Matrix found in Cholesky decomposition.");
	}
	*Ajj = sqrt(*Ajj);
	A.col(j,j+1,N) /= *Ajj;
	A.SubSymMatrix(j+1,N) -= A.col(j,j+1,N) ^ A.col(j,j+1,N).Conjugate();
      }
      if (*Ajj <= RealType(T)(0)) {
	tmv_error(
	  "Non-positive definite Matrix found in Cholesky decomposition.");
      }
      *Ajj = sqrt(*Ajj);
    }
#ifdef XDEBUG
    RealType(T) norml = Norm(A.LowerTri());
    Matrix<T> A2 = A.LowerTri()*A.LowerTri().Adjoint();
    if (Norm(A2-A0) > 0.001*max(RealType(T)(1),norml*norml)) {
      cerr<<"HermCHDecomp: A = "<<Type(A)<<"  "<<A0<<endl;
      cerr<<"Done: A = "<<A<<endl;
      cerr<<"L = "<<A.LowerTri()<<endl;
      cerr<<"Lt = "<<A.LowerTri().Adjoint()<<endl;
      cerr<<"L*Lt = "<<A2<<endl;
      abort();
    }
#endif
  }

  template <class T> void BlockHermCH_Decompose(const SymMatrixView<T>& A)
  {
    // If A is large, we can take advantage of Blas Level 3 speed
    // by partitioning the matrix into blocks.
    //
    // A = [ A00  A01 ]
    //     [ A10  A11 ]
    // A00, A11 are Hermitian,
    // A01 = A10t
    // A00 is k by k, A11 is (N-k) by (N-k)
    //
    // Then, the L Lt decomposition of A is:
    //
    // A = [ L00   0  ] [ L00t L10t ]
    //     [ L10  L11 ] [  0   L11t ]
    //   = [ L00 L00t         L00 L10t      ]
    //     [ L10 L00t   L10 L10t + L11 L11t ]
    //
    // So A00 = L00 L00t
    //    A10 = L10 L00t
    //    A11 = L10 L10t + L11 L11t
    //
    // The block routine is:
    //
    // 1) First Cholesky decompose A00 into L00 L00t.  
    // 2) Solve for L10 = A10 L00t^-1.
    // 3) Find A11' = A11 - L10 L10t.
    // 4) Repeat with A11'.
    //
#ifdef XDEBUG
    //cerr<<"Start Block HermCH_Decomp: A = "<<Type(A)<<"  "<<A<<endl;
    HermMatrix<T> A0 = A;
    HermMatrix<T,Lower,ColMajor> A2 = A;
    NonBlockHermCH_Decompose(A2.View());
#endif
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(IsReal(T()) || A.isherm());

    const size_t N = A.size();

    for (size_t jk=0; jk<N; jk+=CH_BLOCKSIZE)
    {
      size_t jkpk = min(jk+CH_BLOCKSIZE,N);

      SymMatrixView<T> A00 = A.SubSymMatrix(jk,jkpk);
      SymMatrixView<T> A11 = A.SubSymMatrix(jkpk,N);
      MatrixView<T> A10 = A.SubMatrix(jkpk,N,jk,jkpk);

      NonBlockHermCH_Decompose(A00);

      UpperTriMatrixView<T> L00t = A00.UpperTri();
      A10 %= L00t;

      A11 -= A10 * A10.Adjoint();
    }

#ifdef XDEBUG
    ConstLowerTriMatrixView<T> L = A.LowerTri();
    Matrix<T> AA = L * L.Adjoint();
    if (Norm(AA-A0) > 0.001*max(RealType(T)(1),Norm(A0))) {
      cerr<<"Done Block CH: \n";
      cerr<<"A (block) = "<<A<<endl;
      cerr<<"A (nonblock) = "<<A2<<endl;
      cerr<<"Orig A = "<<A0<<endl;
      cerr<<"LLt (block) = "<<AA<<endl;
      cerr<<"Norm(A-A2) = "<<Norm(A-A2)<<endl;
      cerr<<"Norm(L-L2) = "<<Norm(L-A2.LowerTri())<<endl;
      cerr<<"Norm(AA-A0) = "<<Norm(AA-A0)<<endl;
      abort();
    }
#endif
  }

  template <class T> void RecursiveHermCH_Decompose(const SymMatrixView<T>& A)
  {
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(A.isherm());

    const size_t N = A.size();
    if (N <= CH_BLOCKSIZE2) NonBlockHermCH_Decompose(A);
    else if (N > 2) {
      size_t k = N/2;
      if (k > CH_BLOCKSIZE) k = k/CH_BLOCKSIZE*CH_BLOCKSIZE;

      SymMatrixView<T> A00 = A.SubSymMatrix(0,k);
      SymMatrixView<T> A11 = A.SubSymMatrix(k,N);
      MatrixView<T> A10 = A.SubMatrix(k,N,0,k);

      RecursiveHermCH_Decompose(A00);

      UpperTriMatrixView<T> L00t = A00.UpperTri();
      A10 %= L00t;
      A11 -= A10 * A10.Adjoint();

      RecursiveHermCH_Decompose(A11);

    } else if (N > 0) {
      VectorView<RealType(T)> D = A.diag().Real();
      if (D(0) <= RealType(T)(0)) {
	tmv_error(
	  "Non-positive definite Matrix found in Cholesky decomposition.");
      }
      D(0) = SQRT(D(0));
      if (N == 2) {
	A(1,0) /= D(0);
	D(1) -= NORM(A(1,0));
	if (D(1) <= RealType(T)(0)) {
	  tmv_error(
	      "Non-positive definite Matrix found in Cholesky decomposition.");
	}
	D(1) = SQRT(D(1));
      }
    }
  }

  template <class T> void NonLapHermCH_Decompose(const SymMatrixView<T>& A)
  {
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.ct() == NonConj);
    TMVAssert(IsReal(T()) || A.isherm());

    if (RecursiveCH)
      RecursiveHermCH_Decompose(A);
    else if (A.size() >= 2*CH_BLOCKSIZE) 
      BlockHermCH_Decompose(A);
    else
      NonBlockHermCH_Decompose(A);
  }

#ifdef LAP
  template <class T> inline void LapHermCH_Decompose(
      const SymMatrixView<T>& A)
  { NonLapHermCH_Decompose(A); }
  template <> inline void LapHermCH_Decompose(const SymMatrixView<double>& A)
  {
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.iscm() || A.isrm());
    TMVAssert(A.ct()==NonConj);
    char u = A.iscm() ? 'L' : 'U';
    int n = A.size();
    int lda = A.iscm() ? A.stepj() : A.stepi();
    int info;
    dpotrf(&u,&n,A.ptr(),&lda,&info);
    if (info < 0) tmv_error("dpotrf returned info < 0");
  }
  template <> inline void LapHermCH_Decompose(
      const SymMatrixView<complex<double> >& A)
  {
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.iscm() || A.isrm());
    TMVAssert(A.ct()==NonConj);
    TMVAssert(A.isherm());
    char u = A.iscm() ? 'L' : 'U';
    int n = A.size();
    int lda = A.iscm() ? A.stepj() : A.stepi();
    int info;
    zpotrf(&u,&n,LAP_Complex(A.ptr()),&lda,&info);
    if (info < 0) tmv_error("zpotrf returned info < 0");
  }
#ifndef NOFLOAT
  template <> inline void LapHermCH_Decompose(const SymMatrixView<float>& A)
  {
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.iscm() || A.isrm());
    TMVAssert(A.ct()==NonConj);
    char u = A.iscm() ? 'L' : 'U';
    int n = A.size();
    int lda = A.iscm() ? A.stepj() : A.stepi();
    int info;
    spotrf(&u,&n,A.ptr(),&lda,&info);
    if (info < 0) tmv_error("dpotrf returned info < 0");
  }
  template <> inline void LapHermCH_Decompose(
      const SymMatrixView<complex<float> >& A)
  {
    TMVAssert(A.uplo() == Lower);
    TMVAssert(A.iscm() || A.isrm());
    TMVAssert(A.ct()==NonConj);
    TMVAssert(A.isherm());
    char u = A.iscm() ? 'L' : 'U';
    int n = A.size();
    int lda = A.iscm() ? A.stepj() : A.stepi();
    int info;
    cpotrf(&u,&n,LAP_Complex(A.ptr()),&lda,&info);
    if (info < 0) tmv_error("zpotrf returned info < 0");
  }
#endif // NOFLOAT
#endif // LAP
  template <class T> inline void HermCH_Decompose(const SymMatrixView<T>& A)
  {
    TMVAssert(IsReal(T()) || A.isherm());
    if (A.uplo() == Upper) HermCH_Decompose(A.Adjoint());
    else if (A.isconj()) HermCH_Decompose(A.Conjugate());
    else if (A.size() > 0) {
#ifdef LAP
      if (A.iscm() || A.isrm())
	LapHermCH_Decompose(A);
      else
#endif
	NonLapHermCH_Decompose(A);
    }
  }

#define InstFile "TMV_SymCHDiv.inst"
#include "TMV_Inst.h"
#undef InstFile

} // namespace mv


