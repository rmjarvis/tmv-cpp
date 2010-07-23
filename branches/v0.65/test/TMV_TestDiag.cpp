
#include "TMV_Test.h"
#include "TMV_Test1.h"
#include "TMV.h"
#include <fstream>
#include <cstdio>

template <class T> void TestDiagMatrix()
{
    const int N = 10;

    tmv::DiagMatrix<T> a(N);
    tmv::DiagMatrix<T,tmv::FortranStyle> af(N);
    Assert(a.colsize() == size_t(N) && a.rowsize() == size_t(N),
           "Creating DiagMatrix(N)");
    Assert(af.colsize() == size_t(N) && af.rowsize() == size_t(N),
           "Creating DiagMatrix(N)");

    for (int i=0, k=0; i<N; ++i) for (int j=0; j<N; ++j, ++k) {
        if (i == j) a(i,j) = T(k);
        if (i == j) af(i+1,j+1) = T(k);
    }
    tmv::ConstDiagMatrixView<T> acv = a.view();
    tmv::DiagMatrixView<T> av = a.view();
    tmv::ConstDiagMatrixView<T,tmv::FortranStyle> afcv = af.view();
    tmv::DiagMatrixView<T,tmv::FortranStyle> afv = af.view();

    for (int i=0, k=0; i<N; ++i) for (int j=0; j<N; ++j, ++k)
        if (i == j) {
            Assert(a(i,j) == k,"Read/Write DiagMatrix");
            Assert(acv(i,j) == k,"Access DiagMatrix CV");
            Assert(av(i,j) == k,"Access DiagMatrix V");
            Assert(af(i+1,j+1) == k,"Read/Write DiagMatrixF");
            Assert(afcv(i+1,i+1) == k,"Access DiagMatrixF CV");
            Assert(afv(i+1,i+1) == k,"Access DiagMatrixF V");
            Assert(a(i) == k,"Single argument access for DiagMatrix");
            Assert(acv(i) == k,"Single argument access for DiagMatrix CV");
            Assert(av(i) == k,"Single argument access for DiagMatrix V");
            Assert(af(i+1) == k,"Single argument access for DiagMatrixF");
            Assert(afcv(i+1) == k,"Single argument access for DiagMatrixF CV");
            Assert(afv(i+1) == k,"Single argument access for DiagMatrixF V");
        }

    Assert(a==af,"CStyle Matrix == FortranStyle Matrix");
    Assert(a==acv,"Matrix == ConstMatrixView");
    Assert(a==av,"Matrix == MatrixView");
    Assert(a==afcv,"Matrix == FortranStyle ConstMatrixView");
    Assert(a==afv,"Matrix == FortranStyle MatrixView");

    T qar[] = { T(0), T(3), T(6) };
    std::vector<T> qv(3);
    for(int i=0;i<3;i++) qv[i] = qar[i];
    tmv::DiagMatrix<T> q1(3,qar);
    tmv::DiagMatrix<T> q2(qv);
    tmv::ConstDiagMatrixView<T> q3 = tmv::DiagMatrixViewOf(qar,3);
    for(int i=0;i<3;i++) {
        Assert(q1(i,i) == T(3*i),"Create DiagMatrix from T*");
        Assert(q2(i,i) == T(3*i),"Create DiagMatrix from vector");
        Assert(q3(i,i) == T(3*i),"Create DiagMatrixView of T*");
    }

    tmv::DiagMatrix<T> b(N);
    for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) 
        if (i == j) {
            a(i,j) = T(3+i+5*j);
            b(i,j) = T(5+2*i+4*j);
        }
    af = a;
    Assert(a==af,"Copy CStyle DiagMatrix to FotranStyle");

    tmv::DiagMatrix<T> c(N);
    c = a+b;
    for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) 
        if (i == j)
            Assert(c(i,j) == T(8+3*i+9*j),"Add DiagMatrices");

    c = a-b;
    for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) 
        if (i == j)
            Assert(c(i,j) == T(-2-i+j),"Subtract DiagMatrices");

    tmv::Matrix<T> m = a;
    for (int i=0, k=0; i<N; ++i) for (int j=0; j<N; ++j, ++k)
        if (i == j)
            Assert(a(i,j) == m(i,j),"DiagMatrix -> Matrix");
    Assert(a == tmv::DiagMatrix<T>(m),"Matrix -> DiagMatrix");

    if (!(std::numeric_limits<T>::is_integer)) {
        tmv::DiagMatrix<T> ainv = a;
        ainv.invertSelf();
        tmv::DiagMatrix<T> ainv2 = a.inverse();
        for(int i=0;i<N;++i)
            Assert(std::abs(a(i)*ainv(i) - T(1)) < 1.e-6,"DiagMatrix invertSelf");
        for(int i=0;i<N;++i)
            Assert(std::abs(a(i)*ainv2(i) - T(1)) < 1.e-6,"DiagMatrix inverse()");
    }

    tmv::DiagMatrix<std::complex<T> > ca = a*std::complex<T>(1,2);
    tmv::DiagMatrix<std::complex<T> > cb = b*std::complex<T>(-5,-1);

    // Test I/O

    std::ofstream fout("tmvtest_diagmatrix_io.dat");
    if (!fout) {
#ifdef NOTHROW
        std::cerr<<"Couldn't open tmvtest_diagmatrix_io.dat for output\n"; 
        exit(1); 
#else
        throw std::runtime_error(
            "Couldn't open tmvtest_diagmatrix_io.dat for output");
#endif
    }
    fout << ca << std::endl;
    ca.writeCompact(fout);
    fout.close();

    tmv::Matrix<std::complex<T> > xcm1(N,N);
    tmv::DiagMatrix<std::complex<T> > xcd1(N);
    std::ifstream fin("tmvtest_diagmatrix_io.dat");
    if (!fin) {
#ifdef NOTHROW
        std::cerr<<"Couldn't open tmvtest_diagmatrix_io.dat for input\n"; 
        exit(1); 
#else
        throw std::runtime_error(
            "Couldn't open tmvtest_diagmatrix_io.dat for input");
#endif
    }
    fin >> xcm1 >> xcd1;
    fin.close();
    Assert(tmv::Matrix<std::complex<T> >(ca) == xcm1,"DiagMatrix I/O check #1");
    Assert(ca == xcd1,"DiagMatrix Compact I/O check #1");

    std::auto_ptr<tmv::Matrix<std::complex<T> > > xcm2;
    std::auto_ptr<tmv::DiagMatrix<std::complex<T> > > xcd2;
    fin.open("tmvtest_diagmatrix_io.dat");
    if (!fin) {
#ifdef NOTHROW
        std::cerr<<"Couldn't open tmvtest_diagmatrix_io.dat for input\n"; 
        exit(1); 
#else
        throw std::runtime_error(
            "Couldn't open tmvtest_diagmatrix_io.dat for input");
#endif
    }
    fin >> xcm2 >> xcd2;
    fin.close();
    Assert(tmv::Matrix<std::complex<T> >(ca) == *xcm2,"DiagMatrix I/O check #2");
    Assert(ca == *xcd2,"DiagMatrix Compact I/O check #2");

#ifndef XTEST
    std::remove("tmvtest_diagmatrix_io.dat");
#endif

#if 1
    TestDiagMatrixArith_A1<T>();
    TestDiagMatrixArith_A2<T>();
    TestDiagMatrixArith_A3<T>();
    TestDiagMatrixArith_A4<T>();
    TestDiagMatrixArith_A5<T>();
    TestDiagMatrixArith_A6<T>();
    TestDiagMatrixArith_B4a<T>();
    TestDiagMatrixArith_B4b<T>();
    TestDiagMatrixArith_B5a<T>();
    TestDiagMatrixArith_B5b<T>();
    TestDiagMatrixArith_B6a<T>();
    TestDiagMatrixArith_B6b<T>();
#endif

    std::cout<<"DiagMatrix<"<<tmv::TMV_Text(T())<<"> passed all tests\n";
}

#ifdef TEST_DOUBLE
template void TestDiagMatrix<double>();
#endif
#ifdef TEST_FLOAT
template void TestDiagMatrix<float>();
#endif
#ifdef TEST_LONGDOUBLE
template void TestDiagMatrix<long double>();
#endif
#ifdef TEST_INT
template void TestDiagMatrix<int>();
#endif
