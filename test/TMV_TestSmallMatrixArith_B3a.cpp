// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:
#include "TMV_Test.h"
#include "TMV_Test3.h"
#include "TMV_Mat.h"

#include "TMV_TestMatrixArith.h"

template <class T, int N> static void DoTestSmallMatrixArith_B3a()
{
  tmv::SmallMatrix<T,N,N,tmv::RowMajor> a1;
  for(int i=0;i<N;++i) for(int j=0;j<N;++j) {
    a1(i,j) = T(2.9+4.3*i-5.1*j);
  }
  a1(0,0) = 14;
  if (N > 1) a1(1,0) = -2;
  if (N > 2) a1(2,0) = 7;
  if (N > 3) a1(3,0) = -10;
  if (N > 2) a1(2,2) = 30;

  tmv::SmallMatrix<std::complex<T>,N,N,tmv::RowMajor> ca1 = 
  std::complex<T>(3,2)*a1;
  if (N > 3) ca1(2,3) += std::complex<T>(2.4,3.7);
  if (N > 1) ca1(1,0) *= std::complex<T>(0.8,2.8);
  if (N > 1) ca1.col(1) *= std::complex<T>(-1.1,3.6);
  if (N > 3) ca1.row(3) += 
    tmv::SmallVector<std::complex<T>,N>(std::complex<T>(1.8,9.2));

  if (N > 10) {
    a1 /= T(N*N); a1 += T(1); 
    ca1 /= T(N*N); ca1 += T(1); 
  }

  tmv::SmallMatrix<T,7,N,tmv::RowMajor> a3;
  for(int i=0;i<7;++i) for(int j=0;j<N;++j) a3(i,j) = T(1-3*i+2*j);
  if (N > 7) a3 += a1.Rows(1,8);
  else a3.SubMatrix(2,N+2,0,N) += a1;
  tmv::SmallMatrix<std::complex<T>,7,N,tmv::RowMajor> ca3 = 
  a3*std::complex<T>(1,2);
  if (N > 7) ca3 += ca1.Rows(1,8);
  else ca3.SubMatrix(2,N+2,0,N) += ca1;
  if (N > 1) ca3.col(1) *= std::complex<T>(2,1);
  ca3.row(6).AddToAll(std::complex<T>(-7,2));

  tmv::SmallMatrix<T,N,7,tmv::RowMajor> a5 = a3.Transpose();
  if (N > 7) a5 -= a1.Cols(0,7);
  else a5.SubMatrix(0,N,1,N+1) -= a1;
  tmv::SmallMatrix<std::complex<T>,N,7,tmv::RowMajor> ca5 = ca3.Adjoint();
  if (N > 7) ca5 -= ca1.Cols(0,7);
  else ca5.SubMatrix(0,N,1,N+1) -= ca1;
  ca5.col(3) *= std::complex<T>(-1,3);
  ca5.row(0).AddToAll(std::complex<T>(1,9));

  tmv::SmallVector<T,N> v1 = a1.row(0);
  tmv::SmallVector<std::complex<T>,N> cv1 = ca1.row(0);
  tmv::SmallVector<T,7> v2 = a3.col(0);
  tmv::SmallVector<std::complex<T>,7> cv2 = ca3.col(0);

  if (showstartdone) {
    std::cout<<"B3a\n";
  }
  TestMatrixArith3a<T>(a3,ca3,v1,cv1,v2,cv2,"NonSquare");
  TestMatrixArith3b<T>(a5,ca5,v2,cv2,v1,cv1,"NonSquare");

#ifdef XTEST
  tmv::SmallMatrix<T,7,N,tmv::ColMajor> a4 = a3;
  tmv::SmallMatrix<std::complex<T>,7,N,tmv::ColMajor> ca4 = ca3;

  tmv::SmallMatrix<T,7,N,tmv::RowMajor,tmv::FortranStyle> a3f = a3;
  tmv::SmallMatrix<std::complex<T>,7,N,tmv::RowMajor,tmv::FortranStyle> ca3f = ca3;
  tmv::SmallMatrix<T,7,N,tmv::ColMajor,tmv::FortranStyle> a4f = a4;
  tmv::SmallMatrix<std::complex<T>,7,N,tmv::ColMajor,tmv::FortranStyle> ca4f = ca4;

  tmv::SmallMatrix<T,N,7,tmv::ColMajor> a6 = a5;
  tmv::SmallMatrix<std::complex<T>,N,7,tmv::ColMajor> ca6 = ca5;

  tmv::SmallMatrix<T,N,7,tmv::RowMajor,tmv::FortranStyle> a5f = a5;
  tmv::SmallMatrix<std::complex<T>,N,7,tmv::RowMajor,tmv::FortranStyle> ca5f = ca5;
  tmv::SmallMatrix<T,N,7,tmv::ColMajor,tmv::FortranStyle> a6f = a6;
  tmv::SmallMatrix<std::complex<T>,N,7,tmv::ColMajor,tmv::FortranStyle> ca6f = ca6;

  tmv::SmallVector<T,N,tmv::FortranStyle> v1f = v1;
  tmv::SmallVector<std::complex<T>,N,tmv::FortranStyle> cv1f = cv1;
  tmv::SmallVector<T,7,tmv::FortranStyle> v2f = v2;
  tmv::SmallVector<std::complex<T>,7,tmv::FortranStyle> cv2f = cv2;

  TestMatrixArith3a<T>(a4,ca4,v1,cv1,v2,cv2,"NonSquare");
  TestMatrixArith3a<T>(a3f,ca3f,v1,cv1,v2,cv2,"NonSquare");
  TestMatrixArith3a<T>(a4f,ca4f,v1,cv1,v2,cv2,"NonSquare");
  TestMatrixArith3a<T>(a3f,ca3f,v1f,cv1f,v2,cv2,"NonSquare");
  TestMatrixArith3a<T>(a3f,ca3f,v1f,cv1f,v2f,cv2f,"NonSquare");
  TestMatrixArith3a<T>(a6,ca6,v2,cv2,v1,cv1,"NonSquare");
  TestMatrixArith3a<T>(a5f,ca5f,v2,cv2,v1,cv1,"NonSquare");
  TestMatrixArith3a<T>(a6f,ca6f,v2,cv2,v1,cv1,"NonSquare");
  TestMatrixArith3a<T>(a5f,ca5f,v2f,cv2f,v1,cv1,"NonSquare");
  TestMatrixArith3a<T>(a5f,ca5f,v2f,cv2f,v1f,cv1f,"NonSquare");
#endif
}

template <class T> void TestSmallMatrixArith_B3a()
{
  DoTestSmallMatrixArith_B3a<T,2>();
  DoTestSmallMatrixArith_B3a<T,12>();
#ifdef XTEST
  DoTestSmallMatrixArith_B3a<T,1>();
  DoTestSmallMatrixArith_B3a<T,3>();
  DoTestSmallMatrixArith_B3a<T,4>();
  DoTestSmallMatrixArith_B3a<T,22>();
  DoTestSmallMatrixArith_B3a<T,555>();
#endif
}


#ifdef TEST_DOUBLE
template void TestSmallMatrixArith_B3a<double>();
#endif
#ifdef TEST_FLOAT
template void TestSmallMatrixArith_B3a<float>();
#endif
#ifdef TEST_LONGDOUBLE
template void TestSmallMatrixArith_B3a<long double>();
#endif
#ifdef TEST_INT
template void TestSmallMatrixArith_B3a<int>();
#endif
