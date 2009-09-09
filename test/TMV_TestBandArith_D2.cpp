// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:
#define START 0

#include "TMV_Test.h"
#include "TMV_Test2.h"
#include "TMV.h"
#include "TMV_Band.h"
#include "TMV_TestBandArith.h"

template <class T1, class T2> inline bool CanAddEq(
    const tmv::UpperTriMatrixView<T2>& a,
    const tmv::BandMatrixView<T1>& b) 
{
  return b.colsize() == a.size() && b.rowsize() == a.size() && 
  b.nlo() == 0 && !a.isunit(); 
}

template <class T1, class T2> inline bool CanAddEq(
    const tmv::LowerTriMatrixView<T2>& a,
    const tmv::BandMatrixView<T1>& b) 
{
  return b.colsize() == a.size() && b.rowsize() == a.size() && 
  b.nhi() == 0 && !a.isunit(); 
}

template <class T1, class T2, class T3> inline bool CanMultMM(
    const tmv::UpperTriMatrixView<T2>& a, const tmv::BandMatrixView<T1>& b,
    const tmv::UpperTriMatrixView<T3>& c)
{ 
  return a.rowsize() == b.colsize() && a.colsize() == c.colsize() && 
  b.rowsize() == c.rowsize() && b.nlo() == 0 && !c.isunit(); 
}

template <class T1, class T2, class T3> inline bool CanMultMM(
    const tmv::BandMatrixView<T1>& a, const tmv::UpperTriMatrixView<T2>& b,
    const tmv::UpperTriMatrixView<T3>& c)
{ 
  return a.rowsize() == b.colsize() && a.colsize() == c.colsize() && 
  b.rowsize() == c.rowsize() && a.nlo() == 0 && !c.isunit(); 
}

template <class T1, class T2, class T3> inline bool CanMultMM(
    const tmv::BandMatrixView<T1>& a, const tmv::BandMatrixView<T2>& b,
    const tmv::UpperTriMatrixView<T3>& c)
{ 
  return a.rowsize() == b.colsize() && a.colsize() == c.colsize() && 
  b.rowsize() == c.rowsize() && a.nlo() == 0 && b.nlo() == 0 && !c.isunit(); 
}

template <class T1, class T2, class T3> inline bool CanMultMM(
    const tmv::LowerTriMatrixView<T2>& a, const tmv::BandMatrixView<T1>& b,
    const tmv::LowerTriMatrixView<T3>& c)
{ 
  return a.rowsize() == b.colsize() && a.colsize() == c.colsize() && 
  b.rowsize() == c.rowsize() && b.nhi() == 0 && !c.isunit(); 
}

template <class T1, class T2, class T3> inline bool CanMultMM(
    const tmv::BandMatrixView<T1>& a, const tmv::LowerTriMatrixView<T2>& b,
    const tmv::LowerTriMatrixView<T3>& c)
{ 
  return a.rowsize() == b.colsize() && a.colsize() == c.colsize() && 
  b.rowsize() == c.rowsize() && a.nhi() == 0 && !c.isunit(); 
}

template <class T1, class T2, class T3> inline bool CanMultMM(
    const tmv::BandMatrixView<T1>& a, const tmv::BandMatrixView<T2>& b,
    const tmv::LowerTriMatrixView<T3>& c)
{ 
  return a.rowsize() == b.colsize() && a.colsize() == c.colsize() && 
  b.rowsize() == c.rowsize() && a.nhi() == 0 && b.nhi() == 0 && !c.isunit(); 
}

#include "TMV_TestMatrixArith.h"

template <class T> void TestBandMatrixArith_D2()
{
#ifdef XTEST
  const int N = 10;

  std::vector<tmv::BandMatrixView<T> > b;
  std::vector<tmv::BandMatrixView<std::complex<T> > > cb;
  std::vector<tmv::BaseMatrix<T>*> B;
  std::vector<tmv::BaseMatrix<std::complex<T> >*> CB;
  MakeBandList(b,cb,B,CB);

  tmv::Matrix<T> a1(N,N);
  for (int i=0; i<N; ++i) for (int j=0; j<N; ++j) a1(i,j) = T(3+i-5*j);
  tmv::Matrix<std::complex<T> > ca1(N,N);
  for (int i=0; i<N; ++i) for (int j=0; j<N; ++j)
    ca1(i,j) = std::complex<T>(3+i-5*j,4-8*i-j);

  tmv::UpperTriMatrix<T,tmv::NonUnitDiag,tmv::RowMajor> u1(a1);
  tmv::UpperTriMatrix<std::complex<T>,tmv::NonUnitDiag,tmv::RowMajor> cu1(ca1);
  tmv::UpperTriMatrixView<T> u1v = u1.View();
  tmv::UpperTriMatrixView<std::complex<T> > cu1v = cu1.View();
  tmv::UpperTriMatrix<T,tmv::NonUnitDiag> u1x = u1v;
  tmv::UpperTriMatrix<std::complex<T>,tmv::NonUnitDiag> cu1x = cu1v;

  tmv::UpperTriMatrix<T,tmv::UnitDiag,tmv::RowMajor> u2(a1);
  tmv::UpperTriMatrix<T,tmv::NonUnitDiag,tmv::ColMajor> u3(a1);
  tmv::UpperTriMatrix<T,tmv::UnitDiag,tmv::ColMajor> u4(a1);
  tmv::LowerTriMatrix<T,tmv::NonUnitDiag,tmv::RowMajor> l1(a1);
  tmv::LowerTriMatrix<T,tmv::UnitDiag,tmv::RowMajor> l2(a1);
  tmv::LowerTriMatrix<T,tmv::NonUnitDiag,tmv::ColMajor> l3(a1);
  tmv::LowerTriMatrix<T,tmv::UnitDiag,tmv::ColMajor> l4(a1);

  tmv::UpperTriMatrix<std::complex<T>,tmv::UnitDiag,tmv::RowMajor> cu2(ca1);
  tmv::UpperTriMatrix<std::complex<T>,tmv::NonUnitDiag,tmv::ColMajor> cu3(ca1);
  tmv::UpperTriMatrix<std::complex<T>,tmv::UnitDiag,tmv::ColMajor> cu4(ca1);
  tmv::LowerTriMatrix<std::complex<T>,tmv::NonUnitDiag,tmv::RowMajor> cl1(ca1);
  tmv::LowerTriMatrix<std::complex<T>,tmv::UnitDiag,tmv::RowMajor> cl2(ca1);
  tmv::LowerTriMatrix<std::complex<T>,tmv::NonUnitDiag,tmv::ColMajor> cl3(ca1);
  tmv::LowerTriMatrix<std::complex<T>,tmv::UnitDiag,tmv::ColMajor> cl4(ca1);

  tmv::UpperTriMatrixView<T> u2v = u2.View();
  tmv::UpperTriMatrixView<T> u3v = u3.View();
  tmv::UpperTriMatrixView<T> u4v = u4.View();
  tmv::LowerTriMatrixView<T> l1v = l1.View();
  tmv::LowerTriMatrixView<T> l2v = l2.View();
  tmv::LowerTriMatrixView<T> l3v = l3.View();
  tmv::LowerTriMatrixView<T> l4v = l4.View();
  tmv::UpperTriMatrixView<std::complex<T> > cu2v = cu2.View();
  tmv::UpperTriMatrixView<std::complex<T> > cu3v = cu3.View();
  tmv::UpperTriMatrixView<std::complex<T> > cu4v = cu4.View();
  tmv::LowerTriMatrixView<std::complex<T> > cl1v = cl1.View();
  tmv::LowerTriMatrixView<std::complex<T> > cl2v = cl2.View();
  tmv::LowerTriMatrixView<std::complex<T> > cl3v = cl3.View();
  tmv::LowerTriMatrixView<std::complex<T> > cl4v = cl4.View();

  tmv::UpperTriMatrix<T,tmv::UnitDiag> u2x = u2v;
  tmv::LowerTriMatrix<T,tmv::NonUnitDiag> l1x = l1v;
  tmv::LowerTriMatrix<T,tmv::UnitDiag> l2x = l2v;
  tmv::UpperTriMatrix<std::complex<T>,tmv::UnitDiag> cu2x = cu2v;
  tmv::LowerTriMatrix<std::complex<T>,tmv::NonUnitDiag> cl1x = cl1v;
  tmv::LowerTriMatrix<std::complex<T>,tmv::UnitDiag> cl2x = cl2v;

  for(size_t i=START;i<b.size();i++) {
    if (showstartdone) {
      std::cerr<<"Start loop "<<i<<std::endl;
      std::cerr<<"bi = "<<b[i]<<std::endl;
    }
    tmv::BandMatrixView<T> bi = b[i];
    tmv::BandMatrixView<std::complex<T> > cbi = cb[i];
    tmv::BandMatrix<T> bx = bi;
    tmv::BandMatrix<std::complex<T> > cbx = cbi;

    TestMatrixArith456<T>(u1x,cu1x,u1v,cu1v,bi,cbi,"UpperTri/Band");
    TestMatrixArith456<T>(l1x,cl1x,l1v,cl1v,bi,cbi,"LowerTri/Band");
    TestMatrixArith456<T>(u2x,cu2x,u2v,cu2v,bi,cbi,"UpperTri/Band");
    TestMatrixArith456<T>(l2x,cl2x,l2v,cl2v,bi,cbi,"LowerTri/Band");
    TestMatrixArith456<T>(u1x,cu1x,u3v,cu3v,bi,cbi,"UpperTri/Band");
    TestMatrixArith456<T>(l1x,cl1x,l3v,cl3v,bi,cbi,"LowerTri/Band");
    TestMatrixArith456<T>(u2x,cu2x,u4v,cu4v,bi,cbi,"UpperTri/Band");
    TestMatrixArith456<T>(l2x,cl2x,l4v,cl4v,bi,cbi,"LowerTri/Band");
  }
  for(size_t i=0;i<B.size();i++) delete B[i];
  for(size_t i=0;i<CB.size();i++) delete CB[i];
#endif
}

#ifdef TEST_DOUBLE
template void TestBandMatrixArith_D2<double>();
#endif
#ifdef TEST_FLOAT
template void TestBandMatrixArith_D2<float>();
#endif
#ifdef TEST_LONGDOUBLE
template void TestBandMatrixArith_D2<long double>();
#endif
#ifdef TEST_INT
template void TestBandMatrixArith_D2<int>();
#endif
