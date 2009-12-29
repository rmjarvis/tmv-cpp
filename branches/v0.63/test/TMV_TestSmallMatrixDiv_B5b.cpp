
#include "TMV_Test.h"
#include "TMV_Test3.h"
#include "TMV.h"
#include "TMV_Small.h"

#define NORDIVEQ
#include "TMV_TestMatrixDivArith.h"

template <class T, tmv::StorageType stor, int N> 
static void TestSmallNonSquareDiv_B5b()
{
    tmv::SmallMatrix<T,6,N,stor> m;

    for(int i=0;i<6;++i) for(int j=0;j<N;++j) m(i,j) = T(2+4*i-5*j);
    m.diag() *= T(11);
    m /= T(7);
    if (N > 1) m(1,0) = -2;
    if (N > 2) m(2,0) = 7;
    if (N > 3) m(3,0) = -10;
    if (N > 2) m(2,2) = 30;

    tmv::SmallMatrix<T,N,6,stor> a2 = m.Transpose();
    tmv::SmallMatrix<std::complex<T>,N,6,stor> c2 = a2 * std::complex<T>(-3,4);

    tmv::SmallMatrix<T,7,N,stor> a4;
    for(int i=0;i<7;++i) for(int j=0;j<N;++j) a4(i,j) = T(1-3*i+2*j);
    a4.SubMatrix(0,6,0,N) += a2.Transpose();
    tmv::SmallMatrix<std::complex<T>,7,N,stor> c4 = a4*std::complex<T>(1,2);
    c4.SubMatrix(0,6,0,N) += c2.Transpose();
    if (N > 1) c4.col(1) *= std::complex<T>(2,1);
    c4.row(2).AddToAll(std::complex<T>(-7,2));

    tmv::SmallMatrix<T,7,6,stor> a6;
    for(int i=0;i<7;++i) for(int j=0;j<6;++j) a6(i,j) = T(5+2*i-2*j);
    a6.SubMatrix(1,7,1,1+N) += a2.Transpose();
    tmv::SmallMatrix<std::complex<T>,7,6,stor> c6 = a6*std::complex<T>(1,2);
    c6.SubMatrix(1,7,1,1+N) += c2.Transpose();
    c6.col(1) *= std::complex<T>(2,1);
    c6.row(5).AddToAll(std::complex<T>(-7,2));

    tmv::SmallMatrix<T,7,6,stor> a6x;
    tmv::SmallMatrix<std::complex<T>,7,6,stor> c6x;

    TestMatrixDivArith3c<T>(tmv::QR,a6x,c6x,a2,a6,a4,c2,c6,c4,
                            "NonSquare/NonSquare"); 
#ifdef XTEST
    tmv::SmallMatrix<T,N,6,stor,tmv::FortranStyle> a2f = a2;
    tmv::SmallMatrix<std::complex<T>,N,6,stor,tmv::FortranStyle> c2f = c2;
    tmv::SmallMatrix<T,7,N,stor,tmv::FortranStyle> a4f = a4;
    tmv::SmallMatrix<std::complex<T>,7,N,stor,tmv::FortranStyle> c4f = c4;
    tmv::SmallMatrix<T,7,6,stor,tmv::FortranStyle> a6f = a6;
    tmv::SmallMatrix<std::complex<T>,7,6,stor,tmv::FortranStyle> c6f = c6;


    TestMatrixDivArith3c<T>(tmv::QR,a6x,c6x,a2f,a6,a4,c2f,c6,c4,
                            "NonSquare/NonSquare"); 
    TestMatrixDivArith3c<T>(tmv::QR,a6x,c6x,a2f,a6f,a4,c2f,c6f,c4,
                            "NonSquare/NonSquare"); 
    TestMatrixDivArith3c<T>(tmv::QR,a6x,c6x,a2f,a6f,a4f,c2f,c6f,c4f,
                            "NonSquare/NonSquare"); 

#endif
}

template <class T> 
void TestSmallMatrixDiv_B5b()
{
    TestSmallNonSquareDiv_B5b<T,tmv::ColMajor,2>();
    TestSmallNonSquareDiv_B5b<T,tmv::ColMajor,5>();
#ifdef XTEST
    TestSmallNonSquareDiv_B5b<T,tmv::ColMajor,1>();
    TestSmallNonSquareDiv_B5b<T,tmv::ColMajor,3>();
    TestSmallNonSquareDiv_B5b<T,tmv::ColMajor,4>();
    TestSmallNonSquareDiv_B5b<T,tmv::RowMajor,1>();
    TestSmallNonSquareDiv_B5b<T,tmv::RowMajor,2>();
    TestSmallNonSquareDiv_B5b<T,tmv::RowMajor,3>();
    TestSmallNonSquareDiv_B5b<T,tmv::RowMajor,4>();
    TestSmallNonSquareDiv_B5b<T,tmv::RowMajor,5>();
#endif
}

#ifdef INST_DOUBLE
template void TestSmallMatrixDiv_B5b<double>();
#endif
#ifdef INST_FLOAT
template void TestSmallMatrixDiv_B5b<float>();
#endif
#ifdef INST_LONGDOUBLE
template void TestSmallMatrixDiv_B5b<long double>();
#endif
