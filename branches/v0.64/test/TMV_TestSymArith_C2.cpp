#define START 0

#include "TMV.h"
#include "TMV_Sym.h"
#include "TMV_Test.h"
#include "TMV_Test2.h"
#include "TMV_TestSymArith.h"

#define NOADDEQ
#define NOMULTEQ

#include "TMV_TestMatrixArith.h"

template <class T> 
void TestSymMatrixArith_C2()
{
#if (XTEST & 2)
    const int N = 10;

    std::vector<tmv::SymMatrixView<T> > s;
    std::vector<tmv::SymMatrixView<std::complex<T> > > cs;
    std::vector<tmv::BaseMatrix<T>*> B;
    std::vector<tmv::BaseMatrix<std::complex<T> >*> CB;
    MakeSymList(s,cs,B,CB,InDef);

    tmv::Matrix<T> a1(N,N);
    for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) a1(i,j) = T(3+i-5*j);
    tmv::Matrix<std::complex<T> > ca1(N,N);
    for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) ca1(i,j) = 
        std::complex<T>(3+i-5*j,2-3*i);

    tmv::DiagMatrix<T> d1(a1);
    tmv::DiagMatrix<std::complex<T> > cd1(ca1);
    tmv::DiagMatrixView<T> d1v = d1.view();
    tmv::DiagMatrixView<std::complex<T> > cd1v = cd1.view();
    tmv::DiagMatrix<T> d1x = d1v;
    tmv::DiagMatrix<std::complex<T> > cd1x = cd1v;

    for(size_t i=START;i<s.size();i++) {
        if (showstartdone) {
            std::cout<<"Start loop i = "<<i<<std::endl;
            std::cout<<"si = "<<s[i]<<std::endl;
        }

        tmv::SymMatrixView<T> si = s[i];
        tmv::SymMatrixView<std::complex<T> > csi = cs[i];

        TestMatrixArith4<T>(d1v,cd1v,si,csi,"Diag/Sym");
        TestMatrixArith5<T>(d1v,cd1v,si,csi,"Diag/Sym");
        TestMatrixArith6x<T>(d1v,cd1v,si,csi,"Diag/Sym");
    }
    for(size_t i=0;i<B.size();++i) delete B[i];
    for(size_t i=0;i<CB.size();++i) delete CB[i];
#endif
}

#ifdef TEST_DOUBLE
template void TestSymMatrixArith_C2<double>();
#endif
#ifdef TEST_FLOAT
template void TestSymMatrixArith_C2<float>();
#endif
#ifdef TEST_LONGDOUBLE
template void TestSymMatrixArith_C2<long double>();
#endif
#ifdef TEST_INT
template void TestSymMatrixArith_C2<int>();
#endif
