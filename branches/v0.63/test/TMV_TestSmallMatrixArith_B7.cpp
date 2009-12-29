#include "TMV_Test.h"
#include "TMV_Test3.h"
#include "TMV.h"
#include "TMV_Small.h"

#include "TMV_TestMatrixArith.h"

template <class T, int N> 
static void DoTestSmallMatrixArith_B7()
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

    tmv::SmallMatrix<T,7,N,tmv::RowMajor> a3;
    for(int i=0;i<7;++i) for(int j=0;j<N;++j) a3(i,j) = T(1-3*i+2*j);
    a3.SubMatrix(2,N+2,0,N) += a1;
    tmv::SmallMatrix<std::complex<T>,7,N,tmv::RowMajor> ca3 = 
        a3*std::complex<T>(1,2);
    ca3.SubMatrix(2,N+2,0,N) += ca1;
    if (N > 1) ca3.col(1) *= std::complex<T>(2,1);
    ca3.row(6).AddToAll(std::complex<T>(-7,2));

    tmv::SmallMatrix<T,N,7,tmv::RowMajor> a5 = a3.Transpose();
    a5.SubMatrix(0,N,1,N+1) -= a1;
    tmv::SmallMatrix<std::complex<T>,N,7,tmv::RowMajor> ca5 = ca3.Adjoint();
    ca5.SubMatrix(0,N,1,N+1) -= ca1;
    ca5.col(3) *= std::complex<T>(-1,3);
    ca5.row(0).AddToAll(std::complex<T>(1,9));

    tmv::SmallMatrix<T,7,N> a3x;
    tmv::SmallMatrix<std::complex<T>,7,N> ca3x;
    tmv::SmallMatrix<T,N,7> a5x;
    tmv::SmallMatrix<std::complex<T>,N,7> ca5x;

    tmv::SmallVector<T,N> v1 = a1.col(0);
    tmv::SmallVector<std::complex<T>,N> cv1 = ca1.col(0);
    tmv::SmallVector<T,7> v2 = a3.col(0);
    tmv::SmallVector<std::complex<T>,7> cv2 = ca3.col(0);

    if (showstartdone) {
        std::cout<<"B7\n";
    }
    TestMatrixArith7<T>(a3x,ca3x,a3,ca3,v2,cv2,v1,cv1,"NonSquare");
    TestMatrixArith7<T>(a5x,ca5x,a5,ca5,v1,cv1,v2,cv2,"NonSquare");
#ifdef XTEST
    tmv::SmallMatrix<T,7,N,tmv::ColMajor> a4 = a3;
    tmv::SmallMatrix<std::complex<T>,7,N,tmv::ColMajor> ca4 = ca3;
    tmv::SmallMatrix<T,N,7,tmv::ColMajor> a6 = a5;
    tmv::SmallMatrix<std::complex<T>,N,7,tmv::ColMajor> ca6 = ca5;

    tmv::SmallMatrix<T,7,N,tmv::RowMajor,tmv::FortranStyle> a3f = a3;
    tmv::SmallMatrix<std::complex<T>,7,N,tmv::RowMajor,tmv::FortranStyle> ca3f = ca3;
    tmv::SmallMatrix<T,N,7,tmv::RowMajor,tmv::FortranStyle> a5f = a5;
    tmv::SmallMatrix<std::complex<T>,N,7,tmv::RowMajor,tmv::FortranStyle> ca5f = ca5;

    tmv::SmallVector<T,N,tmv::FortranStyle> v1f = v1;
    tmv::SmallVector<std::complex<T>,N,tmv::FortranStyle> cv1f = cv1;
    tmv::SmallVector<T,7,tmv::FortranStyle> v2f = v2;
    tmv::SmallVector<std::complex<T>,7,tmv::FortranStyle> cv2f = cv2;

    TestMatrixArith7<T>(a3x,ca3x,a4,ca4,v2,cv2,v1,cv1,"NonSquare");
    TestMatrixArith7<T>(a5x,ca5x,a6,ca6,v1,cv1,v2,cv2,"NonSquare");
    TestMatrixArith7<T>(a3x,ca3x,a3f,ca3f,v2,cv2,v1,cv1,"NonSquare");
    TestMatrixArith7<T>(a5x,ca5x,a5f,ca5f,v1,cv1,v2,cv2,"NonSquare");
    TestMatrixArith7<T>(a3x,ca3x,a3f,ca3f,v2f,cv2f,v1,cv1,"NonSquare");
    TestMatrixArith7<T>(a5x,ca5x,a5f,ca5f,v1f,cv1f,v2,cv2,"NonSquare");
    TestMatrixArith7<T>(a3x,ca3x,a3f,ca3f,v2f,cv2f,v1f,cv1f,"NonSquare");
    TestMatrixArith7<T>(a5x,ca5x,a5f,ca5f,v1f,cv1f,v2f,cv2f,"NonSquare");
#endif
}

template <class T> 
void TestSmallMatrixArith_B7()
{
    DoTestSmallMatrixArith_B7<T,2>();
    DoTestSmallMatrixArith_B7<T,5>();
#ifdef XTEST
    DoTestSmallMatrixArith_B7<T,1>();
    DoTestSmallMatrixArith_B7<T,3>();
    DoTestSmallMatrixArith_B7<T,4>();
#endif
}


#ifdef INST_DOUBLE
template void TestSmallMatrixArith_B7<double>();
#endif
#ifdef INST_FLOAT
template void TestSmallMatrixArith_B7<float>();
#endif
#ifdef INST_LONGDOUBLE
template void TestSmallMatrixArith_B7<long double>();
#endif
#ifdef INST_INT
template void TestSmallMatrixArith_B7<int>();
#endif
