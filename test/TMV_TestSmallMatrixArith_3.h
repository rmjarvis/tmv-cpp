// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:

#include "TMV_Test.h"
#include "TMV_Test3.h"

#include "TMV.h"
#include <fstream>

// Remove this once it's ok to test Det()
#define NODIV

#include "TMV_TestMatrixArith.h"
#define CT std::complex<T>

template <class T> void TestSmallMatrixArith_3a();
template <class T> void TestSmallMatrixArith_3b();
template <class T> void TestSmallMatrixArith_3c();
template <class T> void TestSmallMatrixArith_3d();
template <class T> void TestSmallMatrixArith_3e();
template <class T> void TestSmallMatrixArith_3f();

template <class T, int M, int N> void TestSmallMatrixArith_3(std::string label)
{
  if (showstartdone) {
    std::cout<<"Start Arith_3: M,N = "<<M<<','<<N<<std::endl;
  }

  tmv::SmallMatrix<T,M,N,tmv::RowMajor> a1x;
  for(int i=0;i<M;++i) for(int j=0;j<N;++j) {
    a1x(i,j) = T(2.9+4.3*i-5.1*j);
  }
  a1x.diag().AddToAll(T(6));
  a1x(0,0) = 14; 
  if (M > 1) a1x(1,0) = -2; 
  if (M > 2) a1x(2,0) = 7; 
  if (M > 3) a1x(3,0) = -10;
  if (M > 2 && N > 2) a1x(2,2) = 30;

  tmv::SmallMatrix<CT,M,N,tmv::RowMajor> ca1x = CT(3,2)*a1x;
  if (M > 2 && N > 3) ca1x(2,3) += CT(2.4,3.7);
  if (M > 1) ca1x(1,0) *= CT(0.8,2.8);
  if (N > 1) ca1x.col(1) *= CT(-1.1,3.6);
  if (M > 3) ca1x.row(3) += tmv::SmallVector<CT,N>(CT(1.8,9.2));

#ifndef NONSQUARE
  // These next two is to make sure Det is calculable without overflow.
  if (N > 10) {
    a1x /= T(N*N); a1x += T(1);
    ca1x /= T(N*N); ca1x += T(1);
  }
#endif

  tmv::SmallMatrix<T,N,M,tmv::ColMajor> a2x = a1x.Transpose();
  if (N > 1) a2x.row(1) *= T(3.1);
  if (M > 2) a2x.col(2) -= tmv::SmallVector<T,N>(4.9);
  tmv::SmallMatrix<CT,N,M,tmv::ColMajor> ca2x = ca1x.Transpose();
  ca2x -= T(1.3)*a2x;
  ca2x *= CT(1.1,-2.5);

  tmv::SmallVector<T,M> v1x = a1x.col(0);
  tmv::SmallVector<T,N> v2x = a1x.row(2);
  tmv::SmallVector<CT,M> cv1x = ca1x.col(0);
  tmv::SmallVector<CT,N> cv2x = ca1x.row(2);
  tmv::SmallVectorView<T,M,1> v1 = v1.View();
  tmv::SmallVectorView<T,N,1> v2 = v2.View();
  tmv::SmallVectorView<CT,M,1> cv1 = cv1.View();
  tmv::SmallVectorView<CT,N,1> cv2 = cv2.View();

  tmv::SmallMatrixView<T,M,N,1,M> a1 = a1x.View();
  tmv::SmallMatrixView<T,N,M,M,1> a2 = a2x.View();
  tmv::SmallMatrixView<CT,M,N,1,M> ca1 = ca1x.View();
  tmv::SmallMatrixView<CT,N,M,M,1> ca2 = ca2x.View();

  TestMatrixArith3a<T>(a1,ca1,v1,cv1,v2,cv2,label+" ColMajor");
  TestMatrixArith3a<T>(a2,ca2,v1,cv1,v2,cv2,label+" RowMajor");

  TestMatrixArith3b<T>(a1,ca1,v1,cv1,v2,cv2,label+" ColMajor");
  TestMatrixArith3b<T>(a2,ca2,v1,cv1,v2,cv2,label+" RowMajor");

#ifdef XTEST
  tmv::SmallMatrix<T,3*M,4*N,tmv::ColMajor> a3x;
  tmv::SmallMatrixView<T,M,N,3,12*M> a3 = a3x.SubMatrix(0,3*M,0,4*N,3,4);
  a3 = a1;
  tmv::SmallMatrix<CT,3*M,4*N,tmv::ColMajor> ca3x;
  tmv::SmallMatrixView<CT,M,N,3,12*M> ca3 = ca3x.SubMatrix(0,3*M,0,4*N,3,4);
  ca3 = ca1;

  tmv::SmallVector<T,5*M> v15;
  tmv::SmallVectorView<T,M,5> v1s = v15.SubVector(0,5*M,5);
  v1s = v1;
  tmv::SmallVector<T,5*N> v25;
  tmv::SmallVectorView<T,N,5> v2s = v25.SubVector(0,5*N,5);
  v2s = v2;
  tmv::SmallVector<CT,5*M> cv15(20);
  tmv::SmallVectorView<CT,M,5> cv1s = cv15.SubVector(0,5*M,5);
  cv1s = cv1;
  tmv::SmallVector<CT,5*N> cv25(20);
  tmv::SmallVectorView<CT,N,5> cv2s = cv25.SubVector(0,5*N,5);
  cv2s = cv2;

  TestMatrixArith3a<T>(a1,ca1,v1s,cv1s,v2,cv2,label+" ColMajor Step1");
  TestMatrixArith3a<T>(a1,ca1,v1,cv1,v2s,cv2s,label+" ColMajor Step2");
  TestMatrixArith3a<T>(a1,ca1,v1s,cv1s,v2s,cv2s,label+" ColMajor Step12");
  TestMatrixArith3a<T>(a2,ca2,v1s,cv1s,v2,cv2,label+" RowMajor Step1");
  TestMatrixArith3a<T>(a2,ca2,v1,cv1,v2s,cv2s,label+" RowMajor Step2");
  TestMatrixArith3a<T>(a2,ca2,v1s,cv1s,v2s,cv2s,label+" RowMajor Step12");
  TestMatrixArith3a<T>(a3,ca3,v1,cv1,v2,cv2,label+" NonMajor");
  TestMatrixArith3a<T>(a3,ca3,v1s,cv1s,v2,cv2,label+" NonMajor Step1");
  TestMatrixArith3a<T>(a3,ca3,v1,cv1,v2s,cv2s,label+" NonMajor Step2");
  TestMatrixArith3a<T>(a3,ca3,v1s,cv1s,v2s,cv2s,label+" NonMajor Step12");

  TestMatrixArith3b<T>(a1,ca1,v1s,cv1s,v2,cv2,label+" ColMajor Step1");
  TestMatrixArith3b<T>(a1,ca1,v1,cv1,v2s,cv2s,label+" ColMajor Step2");
  TestMatrixArith3b<T>(a1,ca1,v1s,cv1s,v2s,cv2s,label+" ColMajor Step12");
  TestMatrixArith3b<T>(a2,ca2,v1s,cv1s,v2,cv2,label+" RowMajor Step1");
  TestMatrixArith3b<T>(a2,ca2,v1,cv1,v2s,cv2s,label+" RowMajor Step2");
  TestMatrixArith3b<T>(a2,ca2,v1s,cv1s,v2s,cv2s,label+" RowMajor Step12");
  TestMatrixArith3b<T>(a3,ca3,v1,cv1,v2,cv2,label+" NonMajor");
  TestMatrixArith3b<T>(a3,ca3,v1s,cv1s,v2,cv2,label+" NonMajor Step1");
  TestMatrixArith3b<T>(a3,ca3,v1,cv1,v2s,cv2s,label+" NonMajor Step2");
  TestMatrixArith3b<T>(a3,ca3,v1s,cv1s,v2s,cv2s,label+" NonMajor Step12");
#endif
}
