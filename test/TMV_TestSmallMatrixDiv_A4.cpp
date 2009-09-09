// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:

#include "TMV_Test.h"
#include "TMV_Test3.h"
#include "TMV.h"
#include "TMV_Small.h"

#include "TMV_TestMatrixDivArith.h"

template <class T, tmv::StorageType stor, int N> static void TestSmallSquareDiv_A4()
{
  tmv::SmallMatrix<T,N,N,stor> m;

  for(int i=0;i<N;++i) for(int j=0;j<N;++j) m(i,j) = T(2+4*i-5*j);
  m.diag() *= T(11);
  m /= T(7);
  if (N > 1) m(1,0) = -2;
  if (N > 2) m(2,0) = 7;
  if (N > 3) m(3,0) = -10;
  if (N > 2) m(2,2) = 30;

  tmv::SmallMatrix<std::complex<T>,N,N,stor> c = m*std::complex<T>(-1,4);

  tmv::SmallMatrix<T,N,N,stor> a1 = m;
  tmv::SmallMatrix<std::complex<T>,N,N,stor> c1 = a1 * std::complex<T>(1,2);
  c1.diag().AddToAll(std::complex<T>(3,1));

  tmv::SmallMatrix<T,N,7,stor> a3;
  for(int i=0;i<N;++i) for(int j=0;j<7;++j) a3(i,j) = T(1-3*i+2*j);
  a3.SubMatrix(0,N,2,2+N) += a1;

  tmv::SmallMatrix<std::complex<T>,N,7,stor> c3 = a3*std::complex<T>(1,2);
  c3.SubMatrix(0,N,2,2+N) += c1;

  tmv::SmallMatrix<T,N,7,stor> a3x;
  tmv::SmallMatrix<T,N,7,stor> a3b;
  tmv::SmallMatrix<std::complex<T>,N,7,stor> c3x;
  tmv::SmallMatrix<std::complex<T>,N,7,stor> c3b;

  TestMatrixDivArith3b<T>(tmv::LU,a3x,c3x,a1,a3,a3b,c1,c3,c3b,"NonSquare/Square");
#ifdef XTEST
  tmv::SmallMatrix<T,N,N,stor,tmv::FortranStyle> a1f = a1;
  tmv::SmallMatrix<std::complex<T>,N,N,stor,tmv::FortranStyle> c1f = c1;

  tmv::SmallMatrix<T,N,7,stor,tmv::FortranStyle> a3f = a3;
  tmv::SmallMatrix<std::complex<T>,N,7,stor,tmv::FortranStyle> c3f = c3;
  tmv::SmallMatrix<T,N,7,stor,tmv::FortranStyle> a3fb = a3;
  tmv::SmallMatrix<std::complex<T>,N,7,stor,tmv::FortranStyle> c3fb = c3;

  TestMatrixDivArith3b<T>(tmv::LU,a3x,c3x,a1f,a3,a3b,c1f,c3,c3b,"NonSquare/Square");
  TestMatrixDivArith3b<T>(tmv::LU,a3x,c3x,a1f,a3f,a3b,c1f,c3f,c3b,"NonSquare/Square");
  TestMatrixDivArith3b<T>(tmv::LU,a3x,c3x,a1f,a3f,a3fb,c1f,c3f,c3fb,"NonSquare/Square");
#endif
}

template <class T> void TestSmallMatrixDiv_A4()
{
  TestSmallSquareDiv_A4<T,tmv::ColMajor,2>();
  TestSmallSquareDiv_A4<T,tmv::ColMajor,5>();
#ifdef XTEST
  TestSmallSquareDiv_A4<T,tmv::ColMajor,1>();
  TestSmallSquareDiv_A4<T,tmv::ColMajor,3>();
  TestSmallSquareDiv_A4<T,tmv::ColMajor,4>();
  TestSmallSquareDiv_A4<T,tmv::RowMajor,1>();
  TestSmallSquareDiv_A4<T,tmv::RowMajor,2>();
  TestSmallSquareDiv_A4<T,tmv::RowMajor,3>();
  TestSmallSquareDiv_A4<T,tmv::RowMajor,4>();
  TestSmallSquareDiv_A4<T,tmv::RowMajor,5>();
#endif
}

#ifdef INST_DOUBLE
template void TestSmallMatrixDiv_A4<double>();
#endif
#ifdef INST_FLOAT
template void TestSmallMatrixDiv_A4<float>();
#endif
#ifdef INST_LONGDOUBLE
template void TestSmallMatrixDiv_A4<long double>();
#endif
