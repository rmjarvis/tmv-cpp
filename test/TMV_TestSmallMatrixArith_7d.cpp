
#define NONSQUARE
#include "TMV_TestSmallMatrixArith_7.h"

template <class T> void TestSmallMatrixArith_7d()
{
    TestSmallMatrixArith_7<T,3,6>("3 6");
}

#ifdef TEST_DOUBLE
template void TestSmallMatrixArith_7d<double>();
#endif
#ifdef TEST_FLOAT
template void TestSmallMatrixArith_7d<float>();
#endif
#ifdef TEST_LONGDOUBLE
template void TestSmallMatrixArith_7d<long double>();
#endif
#ifdef TEST_INT
template void TestSmallMatrixArith_7d<int>();
#endif
