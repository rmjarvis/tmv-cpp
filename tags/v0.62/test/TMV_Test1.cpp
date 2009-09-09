// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:

#include <fstream>
#include "TMV_Test.h"
#include "TMV_Test1.h"

bool XXDEBUG1 = false;
bool XXDEBUG2 = false;
bool XXDEBUG3 = false;
bool XXDEBUG4 = false;
bool XXDEBUG5 = false;
bool XXDEBUG6 = false;
bool XXDEBUG7 = false;
bool XXDEBUG8 = false;
bool XXDEBUG9 = false;

bool showtests = false;
bool showacc = false;
bool showdiv = false;
bool showstartdone = false;
bool donorm2 = true;
bool symoprod = true;
bool dontthrow = false;
std::string lastsuccess = "";

int main() try {
  std::ofstream log("tmvtest1.log");
  tmv::WriteWarningsTo(&log);

  //showacc=true;
  //showdiv=true;
  //showtests=true;
  //showstartdone=true;
//#define SKIPREST

#ifndef SKIPREST

#ifdef INST_DOUBLE
  TestAllVector<double>();
  TestAllMatrix<double>();
  TestDiagMatrix<double>();
  TestDiagDiv<double>();
  TestTriMatrix<double>();
  TestAllTriDiv<double>();
  TestAllMatrixDiv<double>();
#endif // DOUBLE

#ifdef INST_FLOAT
  TestAllVector<float>();
  TestAllMatrix<float>();
  TestDiagMatrix<float>();
  TestDiagDiv<float>();
  TestTriMatrix<float>();
  TestAllTriDiv<float>();
  TestAllMatrixDiv<float>();
#endif // FLOAT

#ifdef INST_INT
  TestAllVector<int>();
  TestAllMatrix<int>();
  TestDiagMatrix<int>();
  TestTriMatrix<int>();
#endif  // INT

#ifdef INST_LONGDOUBLE
  TestAllVector<long double>();
  TestAllMatrix<long double>();
  TestDiagMatrix<long double>();
  TestDiagDiv<long double>();
  TestTriMatrix<long double>();
  TestAllTriDiv<long double>();
  TestAllMatrixDiv<long double>();
#endif // LONGDOUBLE

#endif // SKIPREST

  return 0;
}
#if 1
#ifndef NOTHROW
catch (tmv::Error& e) {
  std::cerr<<e<<std::endl;
  std::cerr<<"Last successful test was "<<lastsuccess<<std::endl;
  return 1;
}
#endif
catch (std::exception& e) {
  std::cerr<<e.what()<<std::endl;
  std::cerr<<"Last successful test was "<<lastsuccess<<std::endl;
  return 1;
}
catch (...) {
  std::cerr<<"Unknown exception thrown\n";
  std::cerr<<"Last successful test was "<<lastsuccess<<std::endl;
  return 1;
}
#else
catch (double) {}
#endif

void PreAssert(std::string s)
{
  if (showtests) { 
    std::cout<<"Trying: "<<s;  
    std::cout.flush(); 
  } 
}

void DoAssert(bool x, std::string s)
{
  if (x) { 
    if (showtests) std::cout<<"  Passed"<<std::endl;
    lastsuccess = s; 
  } else { 
    if (showtests) std::cout<<"  Failed"<<std::endl;
    if (dontthrow) std::cout<<"Failed test: "<<s<<std::endl;  
    else
#ifdef NOTHROW
    { std::cerr<<"Error in test: "<<s<<std::endl; exit(1); }
#else
    throw tmv::Error("Error in test: ",s);  
#endif
  } 
}

