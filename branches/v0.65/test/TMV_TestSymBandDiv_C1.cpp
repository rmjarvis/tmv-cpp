
#define START 0

#include "TMV.h"
#include "TMV_SymBand.h"
#include "TMV_Test.h"
#include "TMV_Test2.h"
#include "TMV_TestSymBandArith.h"

#define NOLDIVEQ
#define NORDIVEQ
#include "TMV_TestMatrixDivArith.h"

template <class T> 
void TestSymBandDiv_C1(tmv::DivType dt, PosDefCode pdc)
{
    const int N = 10;

    std::vector<tmv::SymBandMatrixView<T> > sb;
    std::vector<tmv::SymBandMatrixView<std::complex<T> > > csb;
    std::vector<tmv::BaseMatrix<T>*> B;
    std::vector<tmv::BaseMatrix<std::complex<T> >*> CB;
    MakeSymBandList(sb,csb,B,CB,pdc);

    tmv::Matrix<T> a1(N,N);
    for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) a1(i,j) = T(1-3*i+j);
    a1.diag().addToAll(T(10)*N);
    a1 /= T(10);
    tmv::Matrix<std::complex<T> > ca1 = a1 * std::complex<T>(3,-4);

    tmv::DiagMatrix<T> d(a1);
    tmv::DiagMatrix<std::complex<T> > cd(ca1);
    tmv::DiagMatrixView<T> dv = d.view();
    tmv::DiagMatrixView<std::complex<T> > cdv = cd.view();

    for(size_t i=START;i<sb.size();i++) {
        if (showstartdone)
            std::cout<<"Start loop: i = "<<i<<", si = "<<tmv::TMV_Text(sb[i])<<
                "  "<<sb[i]<<std::endl;
        tmv::SymBandMatrixView<T> si = sb[i];
        tmv::SymBandMatrixView<std::complex<T> > csi = csb[i];
        if (dt == tmv::CH && csi.issym()) continue;

        si.saveDiv();
        csi.saveDiv();

        TestMatrixDivArith1<T>(dt,si,dv,csi,cdv,"DiagMatrix/SymBand");
    }
    for(size_t i=0;i<B.size();++i) delete B[i];
    for(size_t i=0;i<CB.size();++i) delete CB[i];
}

#ifdef TEST_DOUBLE
template void TestSymBandDiv_C1<double>(tmv::DivType dt, PosDefCode pdc);
#endif
#ifdef TEST_FLOAT
template void TestSymBandDiv_C1<float>(tmv::DivType dt, PosDefCode pdc);
#endif
#ifdef TEST_LONGDOUBLE
template void TestSymBandDiv_C1<long double>(tmv::DivType dt, PosDefCode pdc);
#endif
