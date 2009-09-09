///////////////////////////////////////////////////////////////////////////////
// vim:et:ts=2:sw=2:ci:cino=f0,g0,t0,+0:
//                                                                           //
// The Template Matrix/Vector Library for C++ was created by Mike Jarvis     //
// Copyright (C) 1998 - 2009                                                 //
//                                                                           //
// The project is hosted at http://sourceforge.net/projects/tmv-cpp/         //
// where you can find the current version and current documention.           //
//                                                                           //
// For concerns or problems with the software, Mike may be contacted at      //
// mike_jarvis@users.sourceforge.net                                         //
//                                                                           //
// This program is free software; you can redistribute it and/or             //
// modify it under the terms of the GNU General Public License               //
// as published by the Free Software Foundation; either version 2            //
// of the License, or (at your option) any later version.                    //
//                                                                           //
// This program is distributed in the hope that it will be useful,           //
// but WITHOUT ANY WARRANTY; without even the implied warranty of            //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
// GNU General Public License for more details.                              //
//                                                                           //
// You should have received a copy of the GNU General Public License         //
// along with this program in the file LICENSE.                              //
//                                                                           //
// If not, write to:                                                         //
// The Free Software Foundation, Inc.                                        //
// 51 Franklin Street, Fifth Floor,                                          //
// Boston, MA  02110-1301, USA.                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
//
// This file defines the TMV Matrix class.
//
// The Matrix class and all associated functions are contained
// in the namespace tmv.  Alse, the Matrix class is a template, 
// so for a Matrix of long doubles, one would write 
// tmv::Matrix<long double>.  
//
// There is also an optional second template parameter which is 
// either RowMajor or ColMajor, referring to the storage pattern of 
// the matrix elements.
//
// Finally, a third optional template parameter is either CStyle or
// FortranStyle to indicate whether to use C-style or Fortran-style indexing.  
// With C-style (the default), the upper left corner of an MxN matrix is m(0,0), 
// and the lower right is m(M-1,N-1).  With Fortran-style, these are m(1,1) and
// m(M,N) respectively.  Also, when a function takes an index range, i1,i2, 
// then with C-style, this means elements from i1...i2-1 inclusive. 
// With Fortran-style, this means i1..i2 inclusive.
//
// Note that if you want to specify Fortran-style, then you must also 
// specify the storage pattern, since that parameter precedes it.  For example:
// Matrix<T,RowMajor,FortranStyle> m(10,10);
//
// Also, GenMatrix and MatrixComposite always use C-style indexing.
// However, they are both castable as a Fortran-style matrix.  Just remember
// to do so if you want to use Fortran-style indexing.
//
// Constructors:
//
//    Matrix<T,stor,I>(size_t colsize, size_t rowsize)
//        Makes a Matrix with column size = colsize and row size = rowsize
//        with _uninitialized_ values
//
//    Matrix<T,stor,I>(size_t colsize, size_t rowsize, T x)
//        Makes a Matrix of size n with all values = x
//
//    Matrix<T,stor,I>(const vector<vector<T> >& m)
//        Makes a Matrix with a_ij = m[i][j]
//
//    Matrix<T,stor,I>(size_t colsize, size_t rowsize, const T* vv)
//    Matrix<T,stor,I>(size_t colsize, size_t rowsize, 
//            const std::vector<T>& vv)
//        Make a Matrix which copies the elements of vv.
//        If stor is tmv::RowMajor then the elements are taken in row major
//        order (m00,m01,..m0n,m10,m11...).  If stor is tmv::ColMajor
//        then the elements are taken in column major order.
//        If stor is omitted, then tmv::ColMajor is assumed.
//        (stor is also an optional last parameter on the other above 
//        constructors as well.)
//
//
// Special Constructors
//
//    MatrixView RowVectorViewOf(Vector& v)
//    MatrixView RowVectorViewOf(const VectorView& v)
//    ConstMatrixView RowVectorViewOf(const Vector& v)
//    ConstMatrixView RowVectorViewOf(const ConstVectorView& v)
//        Returns a 1xn MatrixView with v in the (only) row.
//        Unlike creating a Matrix with RowVector(v), this uses the same
//        data storage as the actual Vector v.
//        For a const Vector or a ConstVectorView, this returns a 
//        ConstMatrixView.
//
//    MatrixView ColVectorViewOf(Vector& v)
//    MatrixView ColVectorViewOf(const VectorView& v)
//    ConstMatrixView ColVectorViewOf(const Vector& v)
//    ConstMatrixView ColVectorViewOf(const ConstVectorView& v)
//        Returns an nx1 MatrixView with v in the (only) column.
//
//    MatrixView MatrixViewOf(T* m, size_t colsize, size_t rowsize,
//            StorageType stor)
//    ConstMatrixView MatrixViewOf(const T* m, size_t colsize, size_t rowsize,
//            StorageType stor)
//        Returns a MatrixView of the elements in m, using the actual
//        elements m for the storage.  This is essentially the same as the 
//        constructor with (const T*m), except that the data isn't duplicated.
//
// Access Functions
//
//    size_t colsize() const
//    size_t rowsize() const
//        Return the dimensions of the Matrix
//
//    T& operator()(int i, int j)
//    T operator()(int i, int j) const
//        Return the (i,j) element of the Matrix
//
//    VectorView& operator[](int i)
//    ConstVectorView& operator[](int i) const
//        Return the ith row of the Matrix as a Vector.
//        This allows for m[i][j] style access.
//
//    VectorView& row(int i, int j1, int j2)
//    ConstVectorView& row(int i, int j1, int j2) const
//        Return the ith row of the Matrix as a Vector
//        If j1,j2 are given, it returns the SubVector from j1 to j2 
//        (not including j2) within the row.
//
//    VectorView& col(int j, int i1, int i2)
//    ConstVectorView& col(int j) const
//        Return the jth column of the Matrix as a Vector
//        If i1,i2 are given, it returns the SubVector from i1 to i2 
//        (not including i2) within the column.
//
//    VectorView& diag()
//    ConstVectorView& diag() const
//        Return the diagonal of the Matrix as a Vector
//
//    VectorView& diag(int i, int j1, int j2)
//    ConstVectorView& diag(i, j1, j2) const
//        Return the super- or sub-diagonal i
//        If i > 0 return the super diagonal starting at m_0i
//        If i < 0 return the sub diagonal starting at m_|i|0
//        If j1,j2 are given, it returns the diagonal SubVector 
//        either from m_j1,i+j1 to m_j2,i+j2 (for i>0) 
//        or from m_|i|+j1,j1 to m_|i|+j2,j2 (for i<0)
//
// Modifying Functions
//
//    Matrix& Zero()
//        Sets all elements to 0
//
//    Matrix& Clip(RT thresh)
//        Set to 0 all elements whose abolute value is < thresh
//
//    Matrix& SetAllTo(T x)
//        Sets all elements to x
//
//    Matrix<T>& TransposeSelf() 
//        Transposes the elements of a square Matrix or SubMatrix
//
//    Matrix& ConjugateSelf()
//        Sets all elements to its conjugate
//
//    Matrix& SetToIdentity(x = 1)
//        Set to Identity Matrix, or 
//        with a parameter, set to x times Identity Matrix
//
//    Matrix& SwapRows(i1, i2)
//        Swap two rows
//
//    Matrix& SwapCols(j1, j2)
//        Swap two columns
//
//    Matrix& PermuteRows(const int* p)
//    Matrix& PermuteRows(const int* p, int i1, int i2)
//        Perform a series of row swaps (0,p[0]), (1,p[1])...
//        In the second case, only do (i1,p[i1)...(i2-1,p[i2-1])
//    Matrix& ReversePermuteRows(const int* p)
//    Matrix& ReversePermuteRows(const int* p, int i1, int i2)
//        The same, but perform the swaps in reverse order
//
//    Matrix& PermuteCols(const int* p)
//    Matrix& PermuteCols(const int* p, int j1, int j2)
//        Perform a series of column swaps (0,p[0]), (1,p[1])...
//        In the second case, only do (j1,p[j1)...(j2-1,p[j2-1])
//    Matrix& ReversePermuteCols(const int* p)
//    Matrix& ReversePermuteCols(const int* p, int j1, int j2)
//        The same, but perform the swaps in reverse order
//
//    void Swap(Matrix& m1, Matrix& m2)
//        Swap the values of two Matrices
//        The Matrices must be the same size
//
// MatrixViews:
//
//    As with VectorViews, we have both constant and mutable views of Matrices.
//    A ConstMatrixView object can only view the underlying Matrix object
//    whereas a MatrixView object can modify it as well.
//    For the below routines, a ConstMatrixView is returned from 
//    ConstMatrixViews and const Matrix objects.
//    A MatrixView is returned from MatrixViews and (non-const) Matrix objects.
//
//    MatrixView SubMatrix(int i1, int i2, int j1, int j2,
//            int istep=1, int jstep=1)
//        This member function will return a submatrix using rows i1 to i2
//        and columns j1 to j2 (not inclusive of i2,j2) which refers
//        to the same physical elements as the original.
//        Thus, to make the matrix:
//           ( 0 0 1 0 )
//           ( 0 0 0 1 )
//           ( 2 2 0 0 )
//           ( 2 2 0 0 )
//        you could write:
//           Matrix<int> A(4,4,0);
//           A.SubMatrix(0,2,2,4).SetToIdentity();
//           A.SubMatrix(2,4,0,2).SetAllTo(2);
//        The substep values allow you to space the elements of 
//        the submatrix at steps larger than 1.
//        eg. To make an 8x8 checkerboard of 1's and 0's, you could write:
//           Matrix<int> A(8,8,0);
//           A.SubMatrix(0,8,1,9,2,2) = 1;
//           A.SubMatrix(1,9,0,8,2,2) = 1;
//        Note that in this case the i2,j2 values need to be the 
//        "one past the end" value given the step size, so 9 here when
//        starting at 1.
//
//    VectorView SubVector(int i, int j, int istep, int jstep, int size)
//        Returns a SubVector which starts at position (i,j) in the 
//        matrix, moves in the directions (istep,jstep) and has a length
//        of size.
//        For example, the cross-diagonal from the lower left to the upper
//        right of a 6x6 matrix could be accessed using:
//        m.SubVector(5,0,-1,1,6)
//
//    MatrixView ColPair(int j1, int j2)
//        This returns an Mx2 submatrix which consists of the 
//        columns j1 and j2.  This is useful for multiplying two 
//        (not necessarily adjacent) columns of a matrix by a 2x2 matrix.
//
//    MatrixView RowPair(int i1, int i2)
//        Same as above, but two rows.
//
//    MatrixView Cols(int j1, int j2)
//        This is shorthand for SubMatrix(0,m.colsize(),j1,j2)
//
//    MatrixView Rows(int i1, int i2)
//        This is shorthand for SubMatrix(i1,i2,0,m.rowsize())
//
//    MatrixView Real()
//    MatrixView Imag()
//        Returns a view to the real/imag elements of a complex Matrix.
//
//    MatrixView View()
//        Returns a view of a matrix.
//
//    MatrixView Conjugate()
//        Returns a view to the conjugate of a Matrix.
//
//    MatrixView Transpose()
//        Returns a view to the transpose of a Matrix.
//
//    MatrixView Adjoint()
//        Returns a view to the adjoint (conjugate transpose) of a Matrix.
//        Note: Some people define the adjoint as the cofactor matrix.
//              This is not the same as our definition of the Adjoint.
//
//    VectorView LinearView()
//        Returns a VectorView with all the elements of the Matrix.
//        This is mostly used internally for things like MaxElement
//        and ConjugateSelf, where the matrix structure is irrelevant,
//        and we just want to do something to all the elements.
//        The correlary function CanLinearize() returns whether this is 
//        allowed.
//
// Functions of Matrixs:
//
//    Det(m)
//    m.Det()
//        Returns the determinant of a Matrix.
//        Note: If the matrix is not square, the determinant is not
//              well defined.  The returned value is such that
//              conj(det) * det = Det(Adjoint(m) * m)
//              So for real nonsquare matrices, the sign is arbitrary,
//              and for complex nonsquare matrices, it is multiplied
//              by an arbitrary phase.
//
//    LogDet(m)
//    m.LogDet(T* sign=0)
//        Returns the logarithm of the absolute value of the determinant.
//        For many large matrices, the determinant yields to overflow.
//        Hence, this function is provided, which stably calculates the
//        natural logarithm of the absolute value of the determinant.
//        The optional sign argument returns the sign of the determinant
//        if T is real, or the factor exp(it) factor by which exp(logdet) 
//        would have to be multiplied to get the actual determinant.
//
//    Trace(m)
//    m.Trace()
//        Returns the trace of a Matrix.
//        = sum_i ( a_ii )
//
//    Norm(m) or NormF(m)
//    m.Norm() or m.NormF()
//        Return the Frobenius norm of a Matrix.
//        = sqrt( sum_ij |a_ij|^2 )
//
//    NormSq(m)
//    m.NormSq(RT scale = 1.)
//        Returns the square of Norm().
//
//    Norm1(m) 
//    m.Norm1() 
//        Returns the 1-norm of a Matrix.
//        = max_j (sum_i |a_ij|)
//
//    Norm2(m) 
//    m.Norm2() 
//        Returns the 2-norm of a Matrix.
//        = sqrt( Max Eigenvalue of (A.Adjoint * A) )
//        = Maximum singular value
//        Note: This norm is costly to calculate if one is not 
//              otherwise doing a singular value decomposition
//              of the Matrix.
//
//    NormInf(m) 
//    m.NormInf() 
//        Returns the infinity-norm of a Matrix.
//        = max_i (sum_j |a_ij|)
//
//    m.Inverse(minv)
//        Sets minv to the inverse of m if it exists.
//        If m is singular and square, and LU is set for dividing
//          (LU is default for square matrices)
//          then a run-time error will occur.
//        If m is singular or not square and SV is set 
//          then the returned matrix is the pseudo-inverse which satisfies:
//          MXM = M
//          XMX = X
//          (MX)T = MX
//          (XM)T = XM
//        [If dividing using QR or QRP, all but the last of these will 
//         be true.]
//
//    m.InverseATA(invata)
//        Sets invata to the Inverse of (A.Adjoint * A) for matrix m = A
//        If Ax=b is solved for x, then (AT A)^-1 is the 
//        covariance matrix of the least-squares solution x
//
//    m.Inverse()
//    Inverse(m)
//        Returns an auxiliary object that delays the calculation of the 
//        inverse until there is appropriate storage for it.
//        m.Inverse(minv) is equivalent to minv = m.Inverse().
//
//    m.NewTranspose()
//    m.NewConjugate()
//    m.NewAdjoint()
//    m.NewView()
//    m.NewInverse()
//    m.NewCopy()
//        These all return pointers to new BaseMatrix's equal to the 
//        Transpose, Conjugate, Adjoint, View, Inverse, or itself respectively.
//        Inverse and Copy allocate new memory, the others are view.
//
// Operators:
//        Here we use m for a Matrix, v for a Vector and x for a Scalar.
//
//        You can also mix real and complex Vectors of the same
//        underlying type.  eg. Matrix<double> and Matrix<complex<double> >
//
//    -m
//
//    m = m
//
//    m += m
//    m + m
//    m += x
//    m + x
//    x + m
//
//    m -= m
//    m - m
//    m -= x
//    m - x
//    x - m
//
//    m *= x
//    m *= m
//    m * x
//    x * m
//    m * v
//    v * m
//    v *= m
//    m * m
//
//    m /= x
//    v /= m
//    v %= m
//    m /= m
//    m %= m
//    m / x
//    x / m
//    v / m
//    v % m
//    m / m
//    m % m
//
//    m == m
//    m != m
//
//       Most of these behave in the logical way for dealing with matrices.
//       Two comments about behavior that might not be obvious:
//
//       1) Vectors are either row or column Vectors as appropriate.
//          eg. For m*v, v is a column Vector, but for v*m, v is a row Vector
//
//       2) Sometimes x should be thought of a x*I, where I is an appropriately
//          sized identity matrix.  For example m-1 means m-I,
//          and m += 3 means m += 3*I (or equivalently, m.diag().AddToAll(3)).
//
//       3) Division by a matrix can be from either the left or the right.
//          ie. v/m can mean either m^-1 v or v m^-1
//          The first case is the solution of mx=v, the second is of xm = v.
//          Since the first case is the way one generally poses a problem
//          for solving a set of equations, we take v/m to be left-division.
//          If you want right-division (v m^-1), then we supply the % operator
//          to do so.  
//          ie. v%m means v m^-1
//          If you want to be more explicit, you can write:
//          v/m as m.Inverse() * v and v%m as v * m.Inverse().
//          In all cases, the actual calculation is delayed until there is
//          storage to put it.  (Unless you string too many calculations 
//          together, in which case it will use a temporary.)
//
//
// I/O: 
//
//    os << m 
//        Writes m to ostream os in the following format:
//          colsize rowsize
//          m(0,0) m(0,1) m(0,2) ... m(0,rowsize)
//          m(1,0) m(1,1) m(1,2) ... m(1,rowsize)
//          ...
//          m(colsize,0) ... 
//
//    is >> m
//        Reads m from istream is in the same format
//        m must already be the correct size for this to work.
//
//    is >> mptr
//        If you do not know the size of the Matrix to be read, you can
//        use this form where mptr is an auto_ptr to an undefined Matrix.
//
//
// Division Control Functions:
//
//    There are a number of algorithms available for dividing
//    matrices.  We provide functions to allow you to 
//    change the algorithm used by the code on the fly.
//    In particular, you can write:
//    m.DivideUsing(dt)
//    where dt is LU, QR, QRP, or SV
//    (ie. anything but CH)
//    Each of these also has an in-place version whcih overwrites the
//    current Matrix memory with the decomposition needed for 
//    doing the division.  Obviously, if you try to use the Matrix
//    after doing SetDiv (implicitly or explicitly), the results will
//    be invalid.
//
//    The default method is LU (LU Decomposition) for square
//    matrices and QR (QR Decomposition) for non-square.
//   
//    See the header comment to TMV_Divider.h for more info about
//    the different algorithms.
//
//    There are also shorthands for accessing the decomposition.
//    If dt = LU, and the decomposition has been set, then 
//    LUD() returns the LUDiv<T> class
//    Likewise:
//    QRD(), QRPD(), SVD() return the corresponding Divider classes.
//


#ifndef TMV_Matrix_H
#define TMV_Matrix_H

#include "tmv/TMV_BaseMatrix.h"
#include "tmv/TMV_BaseTriMatrix.h"
#include "tmv/TMV_Vector.h"
#include <vector>

namespace tmv {

  template <class T1, class T2> 
  inline void Copy(const GenMatrix<T1>& m1, const MatrixView<T2>& m2);

  template <class T, class Tm> 
  class QuotXM;

  template <class T> 
  class LUDiv;

  template <class T> 
  class QRDiv;

  template <class T> 
  class QRPDiv;

  template <class T> 
  class SVDiv;

  template <class T> 
  class GenMatrix : 
    public BaseMatrix<T>,
    private DivHelper<T>
  {

  public:

    //
    // Constructors
    //

    inline GenMatrix() {}
    inline GenMatrix(const GenMatrix<T>&) {}
    virtual inline ~GenMatrix() {}

    //
    // Access Functions
    //

    using AssignableToMatrix<T>::colsize;
    using AssignableToMatrix<T>::rowsize;

    inline ConstVectorView<T> row(int i) const 
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      return ConstVectorView<T>(cptr()+i*stepi(),rowsize(),stepj(),ct()); 
    }

    inline ConstVectorView<T> col(int j) const
    {
      TMVAssert(j>=0 && j<int(rowsize()));
      return ConstVectorView<T>(cptr()+j*stepj(),colsize(),stepi(),ct()); 
    }

    inline ConstVectorView<T> diag() const
    {
      return ConstVectorView<T>(cptr(),MIN(colsize(),rowsize()),
          stepi()+stepj(),ct());
    }

    inline ConstVectorView<T> diag(int i) const
    {
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        const size_t diagsize = MIN(colsize(),rowsize()-i);
        return ConstVectorView<T>(cptr()+i*stepj(),diagsize,diagstep,ct());
      } else {
        const size_t diagsize = MIN(colsize()+i,rowsize());
        return ConstVectorView<T>(cptr()-i*stepi(),diagsize,diagstep,ct());
      }
    }

    inline ConstVectorView<T> row(int i, int j1, int j2) const 
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      return ConstVectorView<T>(cptr()+i*stepi()+j1*stepj(),
          j2-j1,stepj(),ct()); 
    }

    inline ConstVectorView<T> col(int j, int i1, int i2) const
    {
      TMVAssert(j>=0 && j<int(rowsize()));
      TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      return ConstVectorView<T>(cptr()+i1*stepi()+j*stepj(),
          i2-i1,stepi(),ct()); 
    }

    inline ConstVectorView<T> diag(int i, int j1, int j2) const
    {
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        TMVAssert(j1>=0 && j1-j2<=0 && 
            j2<=MIN(int(colsize()),int(rowsize())-i));
        return ConstVectorView<T>(cptr()+i*stepj()+j1*diagstep, 
            j2-j1, diagstep,ct());
      } else {
        TMVAssert(j1>=0 && j1-j2<=0 && 
            j2<=MIN(int(colsize())+i,int(rowsize())));
        return ConstVectorView<T>(cptr()-i*stepi()+j1*diagstep, 
            j2-j1, diagstep,ct());
      }
    }

    inline T operator()(int i, int j) const
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      TMVAssert(j>=0 && j<int(rowsize()));
      return cref(i,j);
    }

    inline ConstVectorView<T> operator[](int i) const
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      return row(i); 
    }

    template <class T2> 
    inline bool SameAs(const BaseMatrix<T2>&) const
    { return false; }

    inline bool SameAs(const GenMatrix<T>& m2) const
    { 
      return (this==&m2 || (cptr()==m2.cptr() && 
            rowsize()==m2.rowsize() && colsize()==m2.colsize() &&
            stepi()==m2.stepi() && stepj()==m2.stepj() && ct()==m2.ct()));
    }

    inline void AssignToM(const MatrixView<RealType(T)>& m2) const
    { 
      TMVAssert(m2.colsize() == colsize());
      TMVAssert(m2.rowsize() == rowsize());
      TMVAssert(IsReal(T()));
      if (!SameAs(m2)) Copy(*this,m2);
    }

    inline void AssignToM(const MatrixView<ComplexType(T)>& m2) const
    { 
      TMVAssert(m2.colsize() == colsize());
      TMVAssert(m2.rowsize() == rowsize());
      if (!SameAs(m2)) Copy(*this,m2);
    }

    //
    // SubMatrix
    //

    bool OKSubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const;

    inline ConstMatrixView<T> SubMatrix(int i1, int i2, int j1, int j2) const
    {
      TMVAssert(OKSubMatrix(i1,i2,j1,j2,1,1));
      return ConstMatrixView<T>(cptr()+i1*stepi()+j1*stepj(),
          i2-i1,j2-j1,stepi(),stepj(),stor(),ct());
    }

    inline ConstMatrixView<T> SubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const
    {
      StorageType newstor = 
      iscm() ? (istep == 1 ? ColMajor : NoMajor) :
      isrm() ? (jstep == 1 ? RowMajor : NoMajor) : NoMajor;
      TMVAssert(OKSubMatrix(i1,i2,j1,j2,istep,jstep));
      return ConstMatrixView<T>(cptr()+i1*stepi()+j1*stepj(),
          (i2-i1)/istep,(j2-j1)/jstep,istep*stepi(),jstep*stepj(),
          newstor,ct());
    }

    bool OKSubVector(int i, int j, int istep, int jstep, int s) const;

    inline ConstVectorView<T> SubVector(
        int i, int j, int istep, int jstep, int s) const
    {
      TMVAssert(OKSubVector(i,j,istep,jstep,s));
      return ConstVectorView<T>(cptr()+i*stepi()+j*stepj(),s,
          istep*stepi()+jstep*stepj(),ct());
    }

    inline ConstMatrixView<T> ColPair(int j1, int j2) const
    {
      StorageType newstor = 
      iscm() ? ColMajor : 
      isrm() ? (j2==j1+1 ? RowMajor : NoMajor) : NoMajor;
      TMVAssert(j1>=0 && j1<int(rowsize()) && j2>=0 && j2<int(rowsize()));
      return ConstMatrixView<T>(cptr()+j1*stepj(),colsize(),2,
          stepi(),(j2-j1)*stepj(),newstor,ct());
    }

    inline ConstMatrixView<T> RowPair(int i1, int i2) const
    {
      StorageType newstor = 
      isrm() ? RowMajor : 
      iscm() ? (i2==i1+1 ? ColMajor : NoMajor) : NoMajor;
      TMVAssert(i1>=0 && i1<int(colsize()) && i2>=0 && i2<int(colsize()));
      return ConstMatrixView<T>(cptr()+i1*stepi(),2,rowsize(),
          (i2-i1)*stepi(),stepj(),newstor,ct());
    }

    inline ConstMatrixView<T> Cols(int j1, int j2) const
    {
      TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      return ConstMatrixView<T>(cptr()+j1*stepj(),colsize(),j2-j1,
          stepi(),stepj(),stor(),ct(),
          (iscm()&&ls())?1:0);
    }

    inline ConstMatrixView<T> Rows(int i1, int i2) const
    {
      TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      return ConstMatrixView<T>(cptr()+i1*stepi(),i2-i1,rowsize(),
          stepi(),stepj(),stor(),ct(),
          (isrm()&&ls())?1:0);
    }

    inline ConstMatrixView<RealType(T)> Real() const
    {
      return ConstMatrixView<RealType(T)>(
          reinterpret_cast<const RealType(T)*>(cptr()),colsize(),rowsize(),
          IsReal(T()) ? stepi() : 2*stepi(),
          IsReal(T()) ? stepj() : 2*stepj(),
          IsReal(T()) ? stor() : NoMajor,NonConj);
    }

    inline ConstMatrixView<RealType(T)> Imag() const
    {
      TMVAssert(IsComplex(T()));
      return ConstMatrixView<RealType(T)>(
          reinterpret_cast<const RealType(T)*>(cptr())+1,
          colsize(),rowsize(),2*stepi(),2*stepj(),NoMajor,NonConj);
    }

    //
    // Views
    //

    inline ConstMatrixView<T> View() const
    { 
      return ConstMatrixView<T>(cptr(),colsize(),rowsize(),
          stepi(),stepj(),stor(),ct(),ls());
    }

    inline ConstMatrixView<T> Transpose() const
    { 
      return ConstMatrixView<T>(cptr(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(stor()),ct(),ls());
    }

    inline ConstMatrixView<T> Conjugate() const
    { 
      return ConstMatrixView<T>(cptr(),colsize(),rowsize(),
          stepi(),stepj(),stor(),ConjOf(T,ct()),ls());
    }

    inline ConstMatrixView<T> Adjoint() const
    { 
      return ConstMatrixView<T>(cptr(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(stor()),ConjOf(T,ct()),ls());
    }

    inline ConstUpperTriMatrixView<T> UpperTri(DiagType dt=NonUnitDiag) const
    { 
      return ConstUpperTriMatrixView<T>(cptr(),rowsize(),
          stepi(),stepj(),dt,stor(),ct() );
    }

    inline ConstLowerTriMatrixView<T> LowerTri(DiagType dt=NonUnitDiag) const
    { 
      return ConstLowerTriMatrixView<T>(cptr(),colsize(),
          stepi(),stepj(),dt,stor(),ct() );
    }

    inline ConstVectorView<T> ConstLinearView() const
    {
      TMVAssert(ls() != 1 || (rowsize() == 1 && colsize() == 1));
      // To assure that next Assert has no effect

      TMVAssert(CanLinearize());
      TMVAssert(ls() == colsize()*rowsize());
      return ConstVectorView<T>(cptr(),ls(),1,ct());
    }

    inline MatrixView<T> NonConst() const
    {
      return MatrixView<T>(const_cast<T*>(cptr()),colsize(),rowsize(),
          stepi(),stepj(),stor(),ct(),ls()
          FIRSTLAST1(cptr(),row(colsize()-1).end().GetP()));
    }

    //
    // Functions of Matrix
    //

    inline T Det() const
    { return DivHelper<T>::Det(); }

    inline RealType(T) LogDet(T* sign=0) const
    { return DivHelper<T>::LogDet(sign); }

    inline T Trace() const
    { return diag().SumElements(); }

    inline RealType(T) Norm() const 
    { return NormF(); }

    RealType(T) NormF() const;

    // NormF()^2
    RealType(T) NormSq(const RealType(T) scale = RealType(T)(1)) const;

    // 1-Norm = max_j (sum_i |a_ij|)
    RealType(T) Norm1() const;

    // inf-Norm = max_i (sum_j |a_ij|)
    inline RealType(T) NormInf() const
    { return Transpose().Norm1(); }

    // = max_i,j (|a_ij|)
    RealType(T) MaxAbsElement() const;

    inline bool Singular() const
    { return DivHelper<T>::Singular(); }

    RealType(T) DoNorm2() const;
    inline RealType(T) Norm2() const
    {
      if (this->DivIsSet() && this->GetDivType() == SV)
        return DivHelper<T>::Norm2();
      TMV_Warning("Calling Matrix::Norm2 without previously calling DivideUsing(SV)");
      return DoNorm2();
    }

    RealType(T) DoCondition() const;
    inline RealType(T) Condition() const
    {
      if (this->DivIsSet() && this->GetDivType() == SV)
        return DivHelper<T>::Condition();
      TMV_Warning("Calling Matrix::Condition without previously calling DivideUsing(SV)");
      return DoCondition();
    }

    // icc gives a strange compiler error if I don't do this 
    // throwugh QInverse.  I think I should be able to just write:
    // QuotXM<T,T> Inverse() const;
    // and then define this in TMV_Matrix.cpp, but that doesn't work.
    QuotXM<T,T> QInverse() const;
    inline QuotXM<T,T> Inverse() const
    { return QInverse(); }

    inline void Inverse(const MatrixView<T>& minv) const
    { DivHelper<T>::Inverse(minv); }

    template <class T1> 
    inline void Inverse(const MatrixView<T1>& minv) const
    { DivHelper<T>::Inverse(minv); }

    template <class T1, StorageType S, IndexStyle I> 
    inline void Inverse(Matrix<T1,S,I>& minv) const
    { DivHelper<T>::Inverse(minv); }

    inline void InverseATA(const MatrixView<T>& ata) const
    { DivHelper<T>::InverseATA(ata); }

    template <StorageType S, IndexStyle I> 
    inline void InverseATA(Matrix<T,S,I>& ata) const
    { DivHelper<T>::InverseATA(ata); }

    auto_ptr<BaseMatrix<T> > NewCopy() const;
    auto_ptr<BaseMatrix<T> > NewView() const;
    auto_ptr<BaseMatrix<T> > NewTranspose() const ;
    auto_ptr<BaseMatrix<T> > NewConjugate() const;
    auto_ptr<BaseMatrix<T> > NewAdjoint() const;
    auto_ptr<BaseMatrix<T> > NewInverse() const;

    // 
    // Division Control
    //

    using DivHelper<T>::DivideInPlace;
    using DivHelper<T>::SaveDiv;
    using DivHelper<T>::SetDiv;
    using DivHelper<T>::UnSetDiv;
    using DivHelper<T>::ReSetDiv;
    using DivHelper<T>::DivIsSet;
    using DivHelper<T>::CheckDecomp;

    inline void DivideUsing(DivType dt) const
    {
      TMVAssert(dt == LU || dt == QR || dt == QRP || dt == SV);
      DivHelper<T>::DivideUsing(dt); 
    }

    inline const LUDiv<T>& LUD() const
    {
      DivideUsing(LU);
      SetDiv();
      TMVAssert(GetDiv());
      TMVAssert(dynamic_cast<const LUDiv<T>*>(GetDiv()));
      return *dynamic_cast<const LUDiv<T>*>(GetDiv());
    }

    inline const QRDiv<T>& QRD() const
    {
      DivideUsing(QR);
      SetDiv();
      TMVAssert(GetDiv());
      TMVAssert(dynamic_cast<const QRDiv<T>*>(GetDiv()));
      return *dynamic_cast<const QRDiv<T>*>(GetDiv());
    }

    inline const QRPDiv<T>& QRPD() const
    {
      DivideUsing(QRP);
      SetDiv();
      TMVAssert(GetDiv());
      TMVAssert(dynamic_cast<const QRPDiv<T>*>(GetDiv()));
      return *dynamic_cast<const QRPDiv<T>*>(GetDiv());
    }

    inline const SVDiv<T>& SVD() const
    {
      DivideUsing(SV);
      SetDiv();
      TMVAssert(GetDiv());
      TMVAssert(dynamic_cast<const SVDiv<T>*>(GetDiv()));
      return *dynamic_cast<const SVDiv<T>*>(GetDiv());
    }

    template <class T1> 
    inline void LDivEq(const VectorView<T1>& v) const 
    { DivHelper<T>::LDivEq(v); }

    template <class T1> 
    inline void LDivEq(const MatrixView<T1>& m) const 
    { DivHelper<T>::LDivEq(m); }

    template <class T1> 
    inline void RDivEq(const VectorView<T1>& v) const 
    { DivHelper<T>::RDivEq(v); }

    template <class T1> 
    inline void RDivEq(const MatrixView<T1>& m) const 
    { DivHelper<T>::RDivEq(m); }

    template <class T1, class T0> 
    inline void LDiv(const GenVector<T1>& v1, const VectorView<T0>& v0) const
    { DivHelper<T>::LDiv(v1,v0); }

    template <class T1, class T0> 
    inline void LDiv(const GenMatrix<T1>& m1, const MatrixView<T0>& m0) const
    { DivHelper<T>::LDiv(m1,m0); }

    template <class T1, class T0> 
    inline void RDiv(const GenVector<T1>& v1, const VectorView<T0>& v0) const
    { DivHelper<T>::RDiv(v1,v0); }

    template <class T1, class T0> 
    inline void RDiv(const GenMatrix<T1>& m1, const MatrixView<T0>& m0) const
    { DivHelper<T>::RDiv(m1,m0); }

    //
    // I/O
    //

    void Write(std::ostream& os) const;
    void Write(std::ostream& os, RealType(T) minnonzero) const;

    virtual const T* cptr() const = 0;
    virtual int stepi() const = 0;
    virtual int stepj() const = 0;
    virtual size_t ls() const = 0;
    virtual inline bool isrm() const { return stor() == RowMajor; }
    virtual inline bool iscm() const { return stor() == ColMajor; }
    virtual inline bool isconj() const 
    { return IsComplex(T()) && ct()==Conj; }
    virtual StorageType stor() const =0;
    virtual ConjType ct() const =0;

    virtual bool CanLinearize() const = 0;

    virtual T cref(int i, int j) const;

  protected :

    using DivHelper<T>::GetDiv;

    void NewDivider() const;
    inline const BaseMatrix<T>& GetMatrix() const { return *this; }

  private :

    inline GenMatrix<T>& operator=(const GenMatrix<T>&) 
    { TMVAssert(FALSE); return *this; }

  }; // GenMatrix

  template <class T, IndexStyle I> 
  class ConstMatrixView : 
    public GenMatrix<T>
  {
  public :

    inline ConstMatrixView(const ConstMatrixView<T,I>& rhs) :
      itsm(rhs.itsm), itscs(rhs.itscs), itsrs(rhs.itsrs), 
      itssi(rhs.itssi), itssj(rhs.itssj),
      itsstor(rhs.itsstor), itsct(rhs.itsct), linsize(rhs.linsize) {}

    inline ConstMatrixView(const GenMatrix<T>& rhs) :
      itsm(rhs.cptr()), itscs(rhs.colsize()), itsrs(rhs.rowsize()), 
      itssi(rhs.stepi()), itssj(rhs.stepj()),
      itsstor(rhs.stor()), itsct(rhs.ct()), linsize(rhs.ls()) {}

    inline ConstMatrixView(
        const T* _m, size_t _cs, size_t _rs, int _si, int _sj, 
        StorageType _stor, ConjType _ct, size_t _ls=0) : 
      itsm(_m), itscs(_cs), itsrs(_rs), itssi(_si), itssj(_sj),
      itsstor(_stor), itsct(_ct), linsize(_ls)
    { 
      TMVAssert(_stor==RowMajor ? _sj==1 : _stor==ColMajor ? _si==1 : true);
    }

    virtual inline ~ConstMatrixView()
    {
#ifdef TMVDEBUG
      const_cast<const T*&>(itsm) = 0;
#endif
    }

    virtual inline size_t colsize() const { return itscs; }
    virtual inline size_t rowsize() const { return itsrs; }
    virtual inline const T* cptr() const { return itsm; }
    virtual inline int stepi() const { return itssi; }
    virtual inline int stepj() const { return itssj; }
    virtual inline StorageType stor() const { return itsstor; }
    virtual inline ConjType ct() const { return itsct; }
    virtual inline size_t ls() const { return linsize; }
    using GenMatrix<T>::isrm;
    using GenMatrix<T>::iscm;

    virtual inline bool CanLinearize() const 
    { 
      if (linsize == 1 && !(rowsize() == 1 && colsize() == 1))
        linsize = rowsize() * colsize();
      TMVAssert(linsize == 0 || isrm() || iscm());
      TMVAssert(linsize == 0 || !isrm() || stepi() == int(rowsize()));
      TMVAssert(linsize == 0 || !iscm() || stepj() == int(colsize()));
      return linsize > 0; 
    }

  protected :

    const T*const itsm;
    const size_t itscs;
    const size_t itsrs;
    const int itssi;
    const int itssj;
    const StorageType itsstor;
    const ConjType itsct;

    mutable size_t linsize;

  private :

    inline ConstMatrixView<T,I>& operator=(const ConstMatrixView<T,I>&) 
    { TMVAssert(FALSE); return *this; }

  }; // ConstMatrixView

  template <class T> 
  class ConstMatrixView<T,FortranStyle> : 
  public ConstMatrixView<T,CStyle>
  {
  public :

    inline ConstMatrixView(const ConstMatrixView<T>& rhs) :
      ConstMatrixView<T,CStyle>(rhs) {}

    inline ConstMatrixView(const GenMatrix<T>& rhs) :
      ConstMatrixView<T,CStyle>(rhs) {}

    inline ConstMatrixView(
        const T* _m, size_t _cs, size_t _rs, int _si, int _sj, 
        StorageType instor, ConjType inct, size_t linsize=0) : 
      ConstMatrixView<T,CStyle>(_m,_cs,_rs,_si,_sj,instor,inct,linsize) {}

    virtual inline ~ConstMatrixView() {}


    // 
    // Access
    //

    inline T operator()(int i, int j)
    { 
      TMVAssert(i>0 && i<=int(this->colsize()));
      TMVAssert(j>0 && j<=int(this->rowsize()));
      return GenMatrix<T>::cref(i-1,j-1);
    }

    inline ConstVectorView<T,FortranStyle> row(int i) const 
    { 
      TMVAssert(i>0 && i<=int(this->colsize()));
      return GenMatrix<T>::row(i-1);
    }

    inline ConstVectorView<T,FortranStyle> col(int j) const
    {
      TMVAssert(j>0 && j<=int(this->rowsize()));
      return GenMatrix<T>::col(j-1);
    }

    inline ConstVectorView<T,FortranStyle> diag() const
    { return GenMatrix<T>::diag(); }

    inline ConstVectorView<T,FortranStyle> diag(int i) const
    { return GenMatrix<T>::diag(i); }

    inline ConstVectorView<T,FortranStyle> row(
        int i, int j1, int j2) const 
    { 
      TMVAssert(i>0 && i<=int(this->colsize()));
      TMVAssert(j1>0 && j1-j2<=0 && j2<=int(this->rowsize()));
      return GenMatrix<T>::row(i-1,j1-1,j2);
    }

    inline ConstVectorView<T,FortranStyle> col(
        int j, int i1, int i2) const
    {
      TMVAssert(j>0 && j<=int(this->rowsize()));
      TMVAssert(i1 > 0 && i1 <= i2 && i2 <= int(this->colsize()));
      return GenMatrix<T>::col(j-1,i1-1,i2);
    }

    inline ConstVectorView<T,FortranStyle> diag(
        int i, int j1, int j2) const
    {
      TMVAssert(j1 > 0);
      return GenMatrix<T>::diag(i,j1-1,j2);
    }

    inline ConstVectorView<T,FortranStyle> operator[](int i) const
    { return row(i); }

    //
    // SubMatrix
    //

    bool OKSubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const;

    inline ConstMatrixView<T,FortranStyle> SubMatrix(
        int i1, int i2, int j1, int j2) const
    {
      TMVAssert(OKSubMatrix(i1,i2,j1,j2,1,1));
      return GenMatrix<T>::SubMatrix(i1-1,i2,j1-1,j2);
    }

    inline ConstMatrixView<T,FortranStyle> SubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const
    {
      TMVAssert(OKSubMatrix(i1,i2,j1,j2,istep,jstep));
      return GenMatrix<T>::SubMatrix(
          i1-1,i2-1+istep,j1-1,j2-1+jstep,istep,jstep);
    }

    bool OKSubVector(
        int i, int j, int istep, int jstep, int s) const;

    inline ConstVectorView<T,FortranStyle> SubVector(
        int i, int j, int istep, int jstep, int s) const
    {
      TMVAssert(OKSubVector(i,j,istep,jstep,s));
      return GenMatrix<T>::SubVector(i-1,j-1,istep,jstep,s);
    }

    inline ConstMatrixView<T,FortranStyle> ColPair(int j1, int j2) const
    {
      TMVAssert(j1 > 0 && j1 <= int(this->rowsize()));
      TMVAssert(j2 > 0 && j2 <= int(this->rowsize()));
      return GenMatrix<T>::ColPair(j1-1,j2-1);
    }

    inline ConstMatrixView<T,FortranStyle> RowPair(int i1, int i2) const
    {
      TMVAssert(i1 > 0 && i1 <= int(this->colsize()));
      TMVAssert(i2 > 0 && i2 <= int(this->colsize()));
      return GenMatrix<T>::RowPair(i1-1,i2-1);
    }

    inline ConstMatrixView<T,FortranStyle> Cols(int j1, int j2) const
    {
      TMVAssert(j1 > 0 && j1 <= j2 && j2 <= int(this->rowsize()));
      return GenMatrix<T>::Cols(j1-1,j2);
    }

    inline ConstMatrixView<T,FortranStyle> Rows(int i1, int i2) const
    {
      TMVAssert(i1 > 0 && i1 <= i2 && i2 <= int(this->colsize()));
      return GenMatrix<T>::Rows(i1-1,i2);
    }

    inline ConstMatrixView<RealType(T),FortranStyle> Real() const
    { return GenMatrix<T>::Real(); }

    inline ConstMatrixView<RealType(T),FortranStyle> Imag() const
    { return GenMatrix<T>::Imag(); }

    //
    // Views
    //

    inline ConstMatrixView<T,FortranStyle> View() const
    { return GenMatrix<T>::View(); }

    inline ConstMatrixView<T,FortranStyle> Transpose() const
    { return GenMatrix<T>::Transpose(); }

    inline ConstMatrixView<T,FortranStyle> Conjugate() const
    { return GenMatrix<T>::Conjugate(); }

    inline ConstMatrixView<T,FortranStyle> Adjoint() const
    { return GenMatrix<T>::Adjoint(); }

    inline ConstUpperTriMatrixView<T,FortranStyle> UpperTri(
        DiagType dt=NonUnitDiag) const
    { return GenMatrix<T>::UpperTri(dt); }

    inline ConstLowerTriMatrixView<T,FortranStyle> LowerTri(
        DiagType dt=NonUnitDiag) const
    { return GenMatrix<T>::LowerTri(dt); }

    inline ConstVectorView<T,FortranStyle> ConstLinearView() const
    { return GenMatrix<T>::ConstLinearView(); }

    inline MatrixView<T,FortranStyle> NonConst() const
    { return GenMatrix<T>::NonConst(); }

  private :

    inline ConstMatrixView<T,FortranStyle>& operator=(
        const ConstMatrixView<T,FortranStyle>&) 
    { TMVAssert(FALSE); return *this; }

  }; // FortranStyle ConstMatrixView

  template <class T, IndexStyle I> 
  class MatrixView : 
    public GenMatrix<T>
  {

  public:

    //
    // Constructors
    //

    inline MatrixView(const MatrixView<T,I>& rhs) : 
      itsm(rhs.itsm), itscs(rhs.itscs), itsrs(rhs.itsrs),
      itssi(rhs.itssi), itssj(rhs.itssj),
      itsstor(rhs.itsstor), itsct(rhs.itsct),
      linsize(rhs.linsize) DEFFIRSTLAST(rhs.first,rhs.last) {}

    inline MatrixView(T* _m, size_t _cs, size_t _rs, int _si, int _sj,
        StorageType _stor, ConjType _ct, size_t _ls PARAMFIRSTLAST(T) ) :
      itsm(_m), itscs(_cs), itsrs(_rs), itssi(_si), itssj(_sj),
      itsstor(_stor), itsct(_ct), linsize(_ls) DEFFIRSTLAST(_first,_last)
    {
      TMVAssert(_stor==RowMajor ? _sj==1 : _stor==ColMajor ? _si==1 : true);
    }

    inline MatrixView(T* _m, size_t _cs, size_t _rs, int _si, int _sj,
        StorageType _stor, ConjType _ct PARAMFIRSTLAST(T) ) :
      itsm(_m), itscs(_cs), itsrs(_rs), itssi(_si), itssj(_sj),
      itsstor(_stor), itsct(_ct), linsize(0) DEFFIRSTLAST(_first,_last)
    {
      TMVAssert(_stor==RowMajor ? _sj==1 : _stor==ColMajor ? _si==1 : true);
    }

    virtual inline ~MatrixView() 
    {
#ifdef TMVDEBUG
      const_cast<T*&>(itsm) = 0;
#endif
    }

    //
    // Op=
    //

    inline const MatrixView<T,I>& operator=(const MatrixView<T,I>& m2) const
    {
      TMVAssert(m2.colsize() == colsize());
      TMVAssert(m2.rowsize() == rowsize());
      m2.AssignToM(*this); 
      return *this; 
    }

    // This next one is to make sure the compiler doesn't try to make
    // an implicit non-const assignment operator.  I think the above 
    // const version should be sufficient, but some compilers need the 
    // non-const one as well.  (e.g. icc version 10)
    inline const MatrixView<T,I>& operator=(const MatrixView<T,I>& m2)
    {
      TMVAssert(m2.colsize() == colsize());
      TMVAssert(m2.rowsize() == rowsize());
      m2.AssignToM(*this); 
      return *this; 
    }

    inline const MatrixView<T,I>& operator=(
        const GenMatrix<RealType(T)>& m2) const
    { 
      TMVAssert(m2.colsize() == colsize());
      TMVAssert(m2.rowsize() == rowsize());
      m2.AssignToM(*this); 
      return *this; 
    }

    inline const MatrixView<T,I>& operator=(
        const GenMatrix<ComplexType(T)>& m2) const
    { 
      TMVAssert(m2.colsize() == colsize());
      TMVAssert(m2.rowsize() == rowsize());
      TMVAssert(IsComplex(T()));
      m2.AssignToM(*this); 
      return *this; 
    }

    template <class T2> 
    inline const MatrixView<T,I>& operator=(const GenMatrix<T2>& m2) const
    { 
      TMVAssert(IsComplex(T()) || IsReal(T2()));
      Copy(m2,*this);
      return *this; 
    }

    inline const MatrixView<T,I>& operator=(T x) const 
    { return SetToIdentity(x); }

    inline const MatrixView<T,I>& operator=(
        const AssignableToMatrix<RealType(T)>& m2) const
    { 
      TMVAssert(colsize() == m2.colsize());
      TMVAssert(rowsize() == m2.rowsize());
      m2.AssignToM(*this);
      return *this;
    }

    inline const MatrixView<T,I>& operator=(
        const AssignableToMatrix<ComplexType(T)>& m2) const
    { 
      TMVAssert(colsize() == m2.colsize());
      TMVAssert(rowsize() == m2.rowsize());
      TMVAssert(IsComplex(T()));
      m2.AssignToM(*this);
      return *this;
    }

    template <class T2, int M, int N, StorageType S2, IndexStyle I2> 
    inline const MatrixView<T,I>& operator=(
        const SmallMatrix<T2,M,N,S2,I2>& m2) const
    { 
      TMVAssert(colsize() == M && rowsize() == N);
      TMVAssert(IsComplex(T()) || IsReal(T2()));
      Copy(m2.View(),*this);
      return *this; 
    }

    template <int M, int N> 
    inline const MatrixView<T,I>& operator=(
        const SmallMatrixComposite<RealType(T),M,N>& m2) const
    { 
      TMVAssert(colsize() == M && rowsize() == N);
      m2.AssignToM(*this);
      return *this; 
    }

    template <int M, int N> 
    inline const MatrixView<T,I>& operator=(
        const SmallMatrixComposite<ComplexType(T),M,N>& m2) const
    { 
      TMVAssert(colsize() == M && rowsize() == N);
      TMVAssert(IsComplex(T()));
      m2.AssignToM(*this);
      return *this; 
    }

    ListAssigner<T,VIter<T> > inline operator=(ListInitClass)
    { 
      TMVAssert(ls() != 1 || (rowsize() == 1 && colsize() == 1));
      // To assure that next Assert has no effect
      TMVAssert(CanLinearize());
      TMVAssert(ls() == colsize()*rowsize());
      return ListAssigner<T,VIter<T> >(LinearView().begin(),ls());
    }

    //
    // Access
    //

    typedef RefType(T) reference;

    inline reference operator()(int i,int j) const 
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      TMVAssert(j>=0 && j<int(rowsize()));
      return ref(i,j); 
    }

    inline VectorView<T,I> operator[](int i) const 
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      return row(i); 
    }

    inline VectorView<T,I> row(int i) const
    {
      TMVAssert(i>=0 && i<int(colsize()));
      return VectorView<T,I>(ptr()+i*stepi(),
          rowsize(),stepj(),ct() FIRSTLAST ); 
    }

    inline VectorView<T,I> col(int j) const
    {
      TMVAssert(j>=0 && j<int(rowsize()));
      return VectorView<T,I>(ptr()+j*stepj(),
          colsize(),stepi(),ct() FIRSTLAST ); 
    }

    inline VectorView<T,I> diag() const
    {
      return VectorView<T,I>(ptr(),
          MIN(colsize(),rowsize()),stepi()+stepj(),ct() FIRSTLAST);
    }

    inline VectorView<T,I> diag(int i) const
    {
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        const size_t diagsize = MIN(colsize(),rowsize()-i);
        return VectorView<T,I>(ptr()+i*stepj(),
            diagsize,diagstep,ct() FIRSTLAST );
      } else {
        const size_t diagsize = MIN(colsize()+i,rowsize());
        return VectorView<T,I>(ptr()-i*stepi(),
            diagsize,diagstep,ct() FIRSTLAST );
      }
    }

    inline VectorView<T,I> row(int i, int j1, int j2) const 
    { 
      TMVAssert(i>=0 && i<int(colsize()));
      TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      return VectorView<T,I>(ptr()+i*stepi()+j1*stepj(),
          j2-j1,stepj(),ct() FIRSTLAST ); 
    }

    inline VectorView<T,I> col(int j, int i1, int i2) const
    {
      TMVAssert(j>=0 && j<int(rowsize()));
      TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      return VectorView<T,I>(ptr()+i1*stepi()+j*stepj(),
          i2-i1,stepi(),ct() FIRSTLAST ); 
    }

    inline VectorView<T,I> diag(int i, int j1, int j2) const
    {
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        TMVAssert(j1>=0 && j1-j2<=0 && 
            j2<=MIN(int(colsize()),int(rowsize())-i));
        return VectorView<T,I>(ptr()+i*stepj()+j1*diagstep,
            j2-j1,diagstep,ct() FIRSTLAST );
      } else {
        TMVAssert(j1>=0 && j1-j2<=0 && 
            j2<=MIN(int(colsize())+i,int(rowsize())));
        return VectorView<T,I>(ptr()-i*stepi()+j1*diagstep,
            j2-j1,diagstep,ct() FIRSTLAST );
      }
    }

    //
    // Modifying Functions
    //

    const MatrixView<T,I>& Zero() const;

    const MatrixView<T,I>& Clip(RealType(T) thresh) const;

    const MatrixView<T,I>& SetAllTo(T x) const;

    const MatrixView<T,I>& TransposeSelf() const;

    const MatrixView<T,I>& ConjugateSelf() const;

    const MatrixView<T,I>& SetToIdentity(T x=T(1)) const;

    inline const MatrixView<T,I>& SwapRows(int i1, int i2) const
    {
      TMVAssert(i1>=0 && i1 < int(colsize()) && i2>=0 && i2 < int(colsize()));
      if (i1!=i2) Swap(row(i1),row(i2));
      return *this;
    }

    inline const MatrixView<T,I>& SwapCols(int j1, int j2) const
    {
      TMVAssert(j1>=0 && j1 < int(rowsize()) && j2>=0 && j2 < int(rowsize()));
      if (j1!=j2) Swap(col(j1),col(j2));
      return *this;
    }

    const MatrixView<T,I>& PermuteRows(const int* p, int i1, int i2) const;

    inline const MatrixView<T,I>& PermuteRows(const int* p) const
    { return PermuteRows(p,0,colsize()); }

    inline const MatrixView<T,I>& PermuteCols(
        const int* p, int j1, int j2) const
    { Transpose().PermuteRows(p,j1,j2); return *this; }

    inline const MatrixView<T,I>& PermuteCols(const int* p) const
    { return PermuteCols(p,0,rowsize()); }

    const MatrixView<T,I>& ReversePermuteRows(
        const int* p, int i1, int i2) const;

    inline const MatrixView<T,I>& ReversePermuteRows(const int* p) const
    { return ReversePermuteRows(p,0,colsize()); }

    inline const MatrixView<T,I>& ReversePermuteCols(
        const int* p, int j1, int j2) const
    { Transpose().ReversePermuteRows(p,j1,j2); return *this; }

    inline const MatrixView<T,I>& ReversePermuteCols(const int* p) const
    { return ReversePermuteCols(p,0,rowsize()); }

    //
    // SubMatrix
    //

    inline MatrixView<T,I> SubMatrix(int i1, int i2, int j1, int j2) const
    {
      TMVAssert(GenMatrix<T>::OKSubMatrix(i1,i2,j1,j2,1,1));
      return MatrixView<T,I>(ptr()+i1*stepi()+j1*stepj(),
          i2-i1,j2-j1,stepi(),stepj(),stor(),ct() FIRSTLAST );
    }

    inline MatrixView<T,I> SubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const
    {
      StorageType newstor = 
      this->iscm() ? (istep == 1 ? ColMajor : NoMajor) :
      this->isrm() ? (jstep == 1 ? RowMajor : NoMajor) : NoMajor;
      TMVAssert(GenMatrix<T>::OKSubMatrix(i1,i2,j1,j2,istep,jstep));
      return MatrixView<T,I>(ptr()+i1*stepi()+j1*stepj(),
          (i2-i1)/istep, (j2-j1)/jstep, istep*stepi(), jstep*stepj(),
          newstor,ct() FIRSTLAST );
    }

    inline VectorView<T,I> SubVector(
        int i, int j, int istep, int jstep, int size) const
    {
      TMVAssert(GenMatrix<T>::OKSubVector(i,j,istep,jstep,size));
      return VectorView<T,I>(ptr()+i*stepi()+j*stepj(),size,
          istep*stepi()+jstep*stepj(),ct() FIRSTLAST );
    }

    inline MatrixView<T,I> ColPair(int j1, int j2) const
    {
      StorageType newstor = 
      this->iscm() ? ColMajor : 
      this->isrm() ? (j2==j1+1 ? RowMajor : NoMajor) : NoMajor;
      TMVAssert(j1>=0 && j1<int(rowsize()) && j2>=0 && j2<int(rowsize()));
      return MatrixView<T,I>(ptr()+j1*stepj(),colsize(),2,
          stepi(),(j2-j1)*stepj(),newstor,ct() FIRSTLAST );
    }

    inline MatrixView<T,I> RowPair(int i1, int i2) const
    {
      StorageType newstor = 
      this->isrm() ? RowMajor : 
      this->iscm() ? (i2==i1+1 ? ColMajor : NoMajor) : NoMajor;
      TMVAssert(i1>=0 && i1<int(colsize()) && i2>=0 && i2<int(colsize()));
      return MatrixView<T,I>(ptr()+i1*stepi(),2,rowsize(),
          (i2-i1)*stepi(),stepj(),newstor,ct() FIRSTLAST );
    }

    inline MatrixView<T,I> Cols(int j1, int j2) const
    {
      TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      return MatrixView<T,I>(ptr()+j1*stepj(),colsize(),j2-j1,
          stepi(),stepj(),stor(),ct(),
          (this->iscm()&&ls())?1:0 FIRSTLAST);
    }

    inline MatrixView<T,I> Rows(int i1, int i2) const
    {
      TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      return MatrixView<T,I>(ptr()+i1*stepi(),i2-i1,rowsize(),
          stepi(),stepj(),stor(),ct(),
          (this->isrm()&&ls())?1:0 FIRSTLAST);
    }

    inline MatrixView<RealType(T)> Real() const
    {
      return MatrixView<RealType(T)>(
          reinterpret_cast<RealType(T)*>(ptr()),colsize(),rowsize(),
          IsReal(T()) ? stepi() : 2*stepi(),
          IsReal(T()) ? stepj() : 2*stepj(),
          IsReal(T()) ? stor() : NoMajor,NonConj
#ifdef TMVFLDEBUG
          ,reinterpret_cast<const RealType(T)*>(first)
          ,reinterpret_cast<const RealType(T)*>(last)
#endif
          );
    }

    inline MatrixView<RealType(T)> Imag() const
    {
      TMVAssert(IsComplex(T()));
      return MatrixView<RealType(T)>(
          reinterpret_cast<RealType(T)*>(ptr())+1,
          colsize(),rowsize(),2*stepi(),2*stepj(), NoMajor,NonConj
#ifdef TMVFLDEBUG
          ,reinterpret_cast<const RealType(T)*>(first)+1
          ,reinterpret_cast<const RealType(T)*>(last)+1
#endif
          );
    }

    //
    // Views
    //

    inline MatrixView<T,I> View() const
    { return *this; }

    inline MatrixView<T,I> Transpose() const
    { 
      return MatrixView<T,I>(ptr(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(stor()),ct(),ls() FIRSTLAST);
    }

    inline MatrixView<T,I> Conjugate() const
    { 
      return MatrixView<T,I>(ptr(),colsize(),rowsize(),
          stepi(),stepj(),stor(),ConjOf(T,ct()),ls() FIRSTLAST);
    }

    inline MatrixView<T,I> Adjoint() const
    { 
      return MatrixView<T,I>(ptr(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(stor()),ConjOf(T,ct()),ls() FIRSTLAST);
    }

    inline UpperTriMatrixView<T,I> UpperTri(DiagType dt=NonUnitDiag) const
    { 
      return UpperTriMatrixView<T,I>(ptr(),rowsize(),stepi(),stepj(),
          dt,stor(),ct() FIRSTLAST);
    }

    inline LowerTriMatrixView<T,I> LowerTri(DiagType dt=NonUnitDiag) const
    { 
      return LowerTriMatrixView<T,I>(ptr(),colsize(),stepi(),stepj(),
          dt,stor(),ct() FIRSTLAST);
    }

    inline VectorView<T,I> LinearView() const
    {
      TMVAssert(ls() != 1 || (rowsize() == 1 && colsize() == 1));
      // To assure that next Assert has no effect

      TMVAssert(CanLinearize());
      TMVAssert(ls() == colsize()*rowsize());
      return VectorView<T,I>(ptr(),ls(),1,ct() FIRSTLAST );
    }


    //
    // I/O
    //

    void Read(std::istream& is) const;

    virtual inline size_t colsize() const { return itscs; }
    virtual inline size_t rowsize() const { return itsrs; }
    virtual inline const T* cptr() const { return itsm; }
    inline T* ptr() const { return itsm; }
    virtual inline int stepi() const { return itssi; }
    virtual inline int stepj() const { return itssj; }
    virtual inline StorageType stor() const { return itsstor; }
    virtual inline ConjType ct() const { return itsct; }
    virtual inline size_t ls() const { return linsize; }

    virtual inline bool CanLinearize() const 
    { 
      if (linsize == 1 && !(rowsize() == 1 && colsize() == 1))
        linsize = rowsize() * colsize();
      TMVAssert(linsize == 0 || this->isrm() || this->iscm());
      TMVAssert(linsize == 0 || !this->isrm() || stepi() == int(rowsize()));
      TMVAssert(linsize == 0 || !this->iscm() || stepj() == int(colsize()));
      return linsize > 0; 
    }

    reference ref(int i, int j) const;

  protected:

    T*const itsm;
    const size_t itscs;
    const size_t itsrs;
    const int itssi;
    const int itssj;
    const StorageType itsstor;
    const ConjType itsct;

    mutable size_t linsize;

#ifdef TMVFLDEBUG
  public:
    const T*const first;
    const T*const last;
#endif

  }; // MatrixView

  template <class T> 
  class MatrixView<T,FortranStyle> : 
  public MatrixView<T,CStyle>
  {

  public:

    //
    // Constructors
    //

    inline MatrixView(const MatrixView<T,CStyle>& rhs) : 
      MatrixView<T,CStyle>(rhs) {}

    inline MatrixView(const MatrixView<T,FortranStyle>& rhs) : 
      MatrixView<T,CStyle>(rhs) {}

    inline MatrixView(
        T* _m, size_t _cs, size_t _rs, int _si, int _sj,
        StorageType instor, ConjType inct, size_t linsize
        PARAMFIRSTLAST(T) ) :
      MatrixView<T,CStyle>(_m,_cs,_rs,_si,_sj,instor,inct,linsize 
          FIRSTLAST1(_first,_last) ) {}

    inline MatrixView(
        T* _m, size_t _cs, size_t _rs, int _si, int _sj,
        StorageType instor, ConjType inct PARAMFIRSTLAST(T) ) :
      MatrixView<T,CStyle>(_m,_cs,_rs,_si,_sj,instor,inct 
          FIRSTLAST1(_first,_last) ) {}

    virtual inline ~MatrixView() {} 

    //
    // Op=
    //

    inline const MatrixView<T,FortranStyle>& operator=(
        const MatrixView<T,FortranStyle>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const MatrixView<T,FortranStyle>& m2) 
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const MatrixView<T,CStyle>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const MatrixView<T,CStyle>& m2)
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const GenMatrix<RealType(T)>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const GenMatrix<ComplexType(T)>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    template <class T2> 
    inline const MatrixView<T,FortranStyle>& operator=(
        const GenMatrix<T2>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(T x) const 
    { MatrixView<T,CStyle>::operator=(x); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const AssignableToMatrix<RealType(T)>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    inline const MatrixView<T,FortranStyle>& operator=(
        const AssignableToMatrix<ComplexType(T)>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    template <class T2, int M, int N, StorageType S2, IndexStyle I2> 
    inline const MatrixView<T,FortranStyle>& operator=(
        const SmallMatrix<T2,M,N,S2,I2>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    template <int M, int N>
    inline const MatrixView<T,FortranStyle>& operator=(
        const SmallMatrixComposite<RealType(T),M,N>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    template <int M, int N>
    inline const MatrixView<T,FortranStyle>& operator=(
        const SmallMatrixComposite<ComplexType(T),M,N>& m2) const
    { MatrixView<T,CStyle>::operator=(m2); return *this; }

    ListAssigner<T,VIter<T> > inline operator=(ListInitClass li)
    { return MatrixView<T,CStyle>::operator=(li); }


    //
    // Access
    //

    inline RefType(T) operator()(int i,int j) const 
    { 
      TMVAssert(i > 0 && i <= int(this->colsize()));
      TMVAssert(j > 0 && j <= int(this->rowsize()));
      return ref(i-1,j-1); 
    }

    inline VectorView<T,FortranStyle> operator[](int i) const 
    { 
      TMVAssert(i>0 && i<=int(this->colsize()));
      return row(i); 
    }

    inline VectorView<T,FortranStyle> row(int i) const
    {
      TMVAssert(i>0 && i<=int(this->colsize()));
      return MatrixView<T,CStyle>::row(i-1);
    }

    inline VectorView<T,FortranStyle> col(int j) const
    {
      TMVAssert(j>0 && j<=int(this->rowsize()));
      return MatrixView<T,CStyle>::col(j-1);
    }

    inline VectorView<T,FortranStyle> diag() const
    { return MatrixView<T,CStyle>::diag(); }

    inline VectorView<T,FortranStyle> diag(int i) const
    { return MatrixView<T,CStyle>::diag(i); }

    inline VectorView<T,FortranStyle> row(
        int i, int j1, int j2) const 
    { 
      TMVAssert(i>0 && i<=int(this->colsize()));
      TMVAssert(j1>0 && j1-j2<=0 && j2<=int(this->rowsize()));
      return MatrixView<T,CStyle>::row(i-1,j1-1,j2);
    }

    inline VectorView<T,FortranStyle> col(
        int j, int i1, int i2) const
    {
      TMVAssert(j>0 && j<=int(this->rowsize()));
      TMVAssert(i1>0 && i1-i2<=0 && i2<=int(this->colsize()));
      return MatrixView<T,CStyle>::col(j-1,i1-1,i2);
    }

    inline VectorView<T,FortranStyle> diag(int i, int j1, int j2) const
    {
      TMVAssert(j1 > 0);
      return MatrixView<T,CStyle>::diag(i,j1-1,j2);
    }

    //
    // Modifying Functions
    //

    inline const MatrixView<T,FortranStyle>& Zero() const 
    { MatrixView<T,CStyle>::Zero(); return *this; }

    inline const MatrixView<T,FortranStyle>& Clip(RealType(T) thresh) const
    { MatrixView<T,CStyle>::Clip(thresh); return *this; }

    inline const MatrixView<T,FortranStyle>& SetAllTo(T x) const
    { MatrixView<T,CStyle>::SetAllTo(x); return *this; }

    inline const MatrixView<T,FortranStyle>& TransposeSelf() const
    { MatrixView<T,CStyle>::TransposeSelf(); return *this; }

    inline const MatrixView<T,FortranStyle>& ConjugateSelf() const
    { MatrixView<T,CStyle>::ConjugateSelf(); return *this; }

    inline const MatrixView<T,FortranStyle>& SetToIdentity(T x=T(1)) const
    { MatrixView<T,CStyle>::SetToIdentity(x); return *this; }

    inline const MatrixView<T,FortranStyle>& SwapRows(int i1, int i2) const
    { 
      TMVAssert(i1 > 0 && i1 <= int(this->colsize()));
      TMVAssert(i2 > 0 && i2 <= int(this->colsize()));
      if (i1 != i2)
        MatrixView<T,CStyle>::SwapRows(i1-1,i2-1); 
      return *this; 
    }

    inline const MatrixView<T,FortranStyle>& SwapCols(int j1, int j2) const
    { 
      TMVAssert(j1 > 0 && j1 <= int(this->rowsize()));
      TMVAssert(j2 > 0 && j2 <= int(this->rowsize()));
      if (j1 != j2)
        MatrixView<T,CStyle>::SwapCols(j1-1,j2-1); 
      return *this; 
    }

    inline const MatrixView<T,FortranStyle>& PermuteRows(
        const int* p, int i1, int i2) const
    {
      TMVAssert(i1>0);
      MatrixView<T,CStyle>::PermuteRows(p,i1-1,i2);
      return *this; 
    }

    inline const MatrixView<T,FortranStyle>& PermuteRows(
        const int* p) const
    { MatrixView<T,CStyle>::PermuteRows(p); return *this; }

    inline const MatrixView<T,FortranStyle>& PermuteCols(
        const int* p, int j1, int j2) const
    { Transpose().PermuteRows(p,j1,j2); return *this; }

    inline const MatrixView<T,FortranStyle>& PermuteCols(
        const int* p) const
    { Transpose().PermuteRows(p); return *this; }

    inline const MatrixView<T,FortranStyle>& ReversePermuteRows(
        const int* p, int i1, int i2) const
    { 
      TMVAssert(i1>0);
      MatrixView<T,CStyle>::ReversePermuteRows(p,i1-1,i2);  
      return *this;
    }

    inline const MatrixView<T,FortranStyle>& ReversePermuteRows(
        const int* p) const
    { MatrixView<T,CStyle>::ReversePermuteRows(p); return *this; }

    inline const MatrixView<T,FortranStyle>& ReversePermuteCols(
        const int* p, int j1, int j2) const
    { Transpose().ReversePermuteRows(p,j1,j2); return *this; }

    inline const MatrixView<T,FortranStyle>& ReversePermuteCols(
        const int* p) const
    { Transpose().ReversePermuteRows(p); return *this; }


    //
    // SubMatrix
    //

    inline bool OKSubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const
    {
      return ConstMatrixView<T,FortranStyle>(*this).OKSubMatrix(
          i1,i2,j1,j2,istep,jstep); 
    }

    inline bool OKSubVector(
        int i, int j, int istep, int jstep, int s) const
    {
      return ConstMatrixView<T,FortranStyle>(*this).OKSubVector(
          i,j,istep,jstep,s); 
    }

    inline MatrixView<T,FortranStyle> SubMatrix(
        int i1, int i2, int j1, int j2) const
    {
      TMVAssert(OKSubMatrix(i1,i2,j1,j2,1,1));
      return MatrixView<T,CStyle>::SubMatrix(i1-1,i2,j1-1,j2);
    }

    inline MatrixView<T,FortranStyle> SubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const
    {
      TMVAssert(OKSubMatrix(i1,i2,j1,j2,istep,jstep));
      return MatrixView<T,CStyle>::SubMatrix(i1-1,i2-1+istep,j1-1,j2-1+jstep,
          istep,jstep);
    }

    inline VectorView<T,FortranStyle> SubVector(
        int i, int j, int istep, int jstep, int s) const
    {
      TMVAssert(OKSubVector(i,j,istep,jstep,s));
      return MatrixView<T,CStyle>::SubVector(i-1,j-1,istep,jstep,s);
    }

    inline MatrixView<T,FortranStyle> ColPair(int j1, int j2) const
    {
      TMVAssert(j1 > 0 && j1 <= int(this->rowsize()));
      TMVAssert(j2 > 0 && j2 <= int(this->rowsize()));
      return MatrixView<T,CStyle>::ColPair(j1-1,j2-1);
    }

    inline MatrixView<T,FortranStyle> RowPair(int i1, int i2) const
    {
      TMVAssert(i1 > 0 && i1 <= int(this->rowsize()));
      TMVAssert(i2 > 0 && i2 <= int(this->rowsize()));
      return MatrixView<T,CStyle>::RowPair(i1-1,i2-1);
    }

    inline MatrixView<T,FortranStyle> Cols(int j1, int j2) const
    {
      TMVAssert(j1 > 0 && j1 <= j2 && j2 <= int(this->rowsize()));
      return MatrixView<T,CStyle>::Cols(j1-1,j2);
    }

    inline MatrixView<T,FortranStyle> Rows(int i1, int i2) const
    {
      TMVAssert(i1 > 0 && i1 <= i2 && i2 <= int(this->colsize()));
      return MatrixView<T,CStyle>::Rows(i1-1,i2);
    }

    inline MatrixView<RealType(T),FortranStyle> Real() const
    { return MatrixView<T,CStyle>::Real(); }

    inline MatrixView<RealType(T),FortranStyle> Imag() const
    { return MatrixView<T,CStyle>::Imag(); }

    //
    // Views
    //

    inline MatrixView<T,FortranStyle> View() const
    { return MatrixView<T,CStyle>::View(); }

    inline MatrixView<T,FortranStyle> Transpose() const
    { return MatrixView<T,CStyle>::Transpose(); }

    inline MatrixView<T,FortranStyle> Conjugate() const
    { return MatrixView<T,CStyle>::Conjugate(); }

    inline MatrixView<T,FortranStyle> Adjoint() const
    { return MatrixView<T,CStyle>::Adjoint(); }

    inline UpperTriMatrixView<T,FortranStyle> UpperTri(
        DiagType dt=NonUnitDiag) const
    { return MatrixView<T,CStyle>::UpperTri(dt); }

    inline LowerTriMatrixView<T,FortranStyle> LowerTri(
        DiagType dt=NonUnitDiag) const
    { return MatrixView<T,CStyle>::LowerTri(dt); }

    inline VectorView<T,FortranStyle> LinearView() const
    { return MatrixView<T,CStyle>::LinearView(); }

  protected:

    using MatrixView<T,CStyle>::ref;

  }; // FortranStyle MatrixView

#ifdef XTEST
#ifdef TMVDEBUG
#define XTEST_DEBUG
#endif
#endif

  template <class T, StorageType S, IndexStyle I> 
  class Matrix : 
    public GenMatrix<T> 
  {

  public:

    //
    // Constructors
    //

#define NEW_SIZE(cs,rs) \
    linsize((cs)*(rs)), \
    itsm(new T[linsize]), itscs(cs), itsrs(rs) \
    DEFFIRSTLAST(itsm.get(),itsm.get()+linsize)

    inline Matrix(size_t _colsize, size_t _rowsize) :
      NEW_SIZE(_colsize,_rowsize)
    {
      TMVAssert(S==RowMajor || S==ColMajor);
#ifdef TMVDEBUG
      SetAllTo(T(888));
#endif
    }

    inline Matrix(size_t _colsize, size_t _rowsize, T x) :
      NEW_SIZE(_colsize,_rowsize)
    {
      TMVAssert(S==RowMajor || S==ColMajor);
      SetAllTo(x);
    }

    inline Matrix(size_t _colsize,size_t _rowsize, const T* vv) :
      NEW_SIZE(_colsize,_rowsize)
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      std::copy(vv,vv+linsize,itsm.get());
    }

    inline Matrix(size_t _colsize, size_t _rowsize, 
        const std::vector<T>& vv) : 
      NEW_SIZE(_colsize,_rowsize)
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      TMVAssert(vv.size() == linsize);
      std::copy(vv.begin(),vv.end(),itsm.get());
    }

    explicit Matrix(const std::vector<std::vector<T> >& vv);

    inline Matrix(const Matrix<T,S,I>& rhs) :
      NEW_SIZE(rhs.colsize(),rhs.rowsize())
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      std::copy(rhs.cptr(),rhs.cptr()+linsize,itsm.get());
    }

    template <IndexStyle I2> 
    inline Matrix(const Matrix<T,S,I2>& rhs) :
      NEW_SIZE(rhs.colsize(),rhs.rowsize())
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      std::copy(rhs.cptr(),rhs.cptr()+linsize,itsm.get());
    }

    inline Matrix(const GenMatrix<RealType(T)>& rhs) :
      NEW_SIZE(rhs.colsize(),rhs.rowsize())
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      rhs.AssignToM(View());
    }

    inline Matrix(const GenMatrix<ComplexType(T)>& rhs) :
      NEW_SIZE(rhs.colsize(),rhs.rowsize())
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      TMVAssert(IsComplex(T()));
      rhs.AssignToM(View());
    }

    template <class T2> 
    inline Matrix(const GenMatrix<T2>& rhs) :
      NEW_SIZE(rhs.colsize(),rhs.rowsize()) 
    { 
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(IsComplex(T()) || IsReal(T2()));
      TMVAssert(S==RowMajor || S==ColMajor);
      Copy(rhs,View()); 
    }

    inline Matrix(const AssignableToMatrix<RealType(T)>& m2) :
      NEW_SIZE(m2.colsize(),m2.rowsize())
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      m2.AssignToM(View());
    }

    inline Matrix(const AssignableToMatrix<ComplexType(T)>& m2) :
      NEW_SIZE(m2.colsize(),m2.rowsize())
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      TMVAssert(IsComplex(T()));
      m2.AssignToM(View());
    }

    template <class T2, int M, int N, StorageType S2, IndexStyle I2> 
    inline Matrix(const SmallMatrix<T2,M,N,S2,I2>& rhs) :
      NEW_SIZE(rhs.colsize(),rhs.rowsize()) 
    { 
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(IsComplex(T()) || IsReal(T2()));
      TMVAssert(S==RowMajor || S==ColMajor);
      Copy(rhs.View(),View()); 
    }

    template <int M, int N> 
    inline Matrix(const SmallMatrixComposite<RealType(T),M,N>& m2) :
      NEW_SIZE(M,N)
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(S==RowMajor || S==ColMajor);
      m2.AssignToM(View());
    }

    template <int M, int N> 
    inline Matrix(const SmallMatrixComposite<ComplexType(T),M,N>& m2) :
      NEW_SIZE(M,N)
    {
#ifdef XTEST_DEBUG
      SetAllTo(T(888));
#endif
      TMVAssert(IsComplex(T()));
      TMVAssert(S==RowMajor || S==ColMajor);
      m2.AssignToM(View());
    }

#undef NEW_SIZE

    virtual inline ~Matrix() 
    {
#ifdef TMVDEBUG
      SetAllTo(T(999));
#endif
    }

    //
    // Op=
    //

    inline Matrix<T,S,I>& operator=(const Matrix<T,S,I>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      if (&m2 != this)
        std::copy(m2.cptr(),m2.cptr()+linsize,itsm.get());
      return *this; 
    }

    template <IndexStyle I2> 
    inline Matrix<T,S,I>& operator=(const Matrix<T,S,I2>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      std::copy(m2.cptr(),m2.cptr()+linsize,itsm.get());
      return *this; 
    }

    inline Matrix<T,S,I>& operator=(const GenMatrix<RealType(T)>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      m2.AssignToM(View());
      return *this; 
    }

    inline Matrix<T,S,I>& operator=(const GenMatrix<ComplexType(T)>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      TMVAssert(IsComplex(T()));
      m2.AssignToM(View());
      return *this; 
    }

    template <class T2> 
    inline Matrix<T,S,I>& operator=(const GenMatrix<T2>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      TMVAssert(IsComplex(T()) || IsReal(T2()));
      Copy(m2,View()); 
      return *this; 
    }

    inline Matrix<T,S,I>& operator=(T x) { return SetToIdentity(x); }

    inline Matrix<T,S,I>& operator=(const AssignableToMatrix<RealType(T)>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      m2.AssignToM(View()); 
      return *this; 
    }

    inline Matrix<T,S,I>& operator=(
        const AssignableToMatrix<ComplexType(T)>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      TMVAssert(IsComplex(T()));
      m2.AssignToM(View()); 
      return *this; 
    }

    template <class T2, int M, int N, StorageType S2, IndexStyle I2> 
    inline Matrix<T,S,I>& operator=(const SmallMatrix<T2,M,N,S2,I2>& m2)
    { 
      TMVAssert(m2.colsize() == colsize() && m2.rowsize() == rowsize());
      TMVAssert(IsComplex(T()) || IsReal(T2()));
      Copy(m2.View(),View()); 
      return *this; 
    }

    template <int M, int N> 
    inline Matrix<T,S,I>& operator=(
        const SmallMatrixComposite<RealType(T),M,N>& m2)
    { 
      TMVAssert(M == colsize() && N == rowsize());
      m2.AssignToM(View());
      return *this; 
    }

    template <int M, int N> 
    inline Matrix<T,S,I>& operator=(
        const SmallMatrixComposite<ComplexType(T),M,N>& m2)
    { 
      TMVAssert(M == colsize() && N == rowsize());
      TMVAssert(IsComplex(T()));
      m2.AssignToM(View());
      return *this; 
    }

    ListAssigner<T,VIt<T,Unit,NonConj> > inline operator=(ListInitClass)
    { 
      TMVAssert(linsize == colsize() * rowsize());
      return ListAssigner<T,VIt<T,Unit,NonConj> >(
          VIt<T,Unit,NonConj>(ptr(),1),linsize);
    }


    //
    // Access
    //

    typedef T& reference;

    inline T operator()(int i,int j) const
    {
      if (I == CStyle) {
        TMVAssert(i>=0 && i < int(colsize()));
        TMVAssert(j>=0 && j < int(rowsize()));
        return cref(i,j); 
      } else {
        TMVAssert(i > 0 && i <= int(colsize()));
        TMVAssert(j > 0 && j <= int(rowsize()));
        return cref(i-1,j-1); 
      }
    }

    inline T& operator()(int i,int j) 
    {
      if (I == CStyle) {
        TMVAssert(i>=0 && i < int(colsize()));
        TMVAssert(j>=0 && j < int(rowsize()));
        return ref(i,j); 
      } else {
        TMVAssert(i > 0 && i <= int(colsize()));
        TMVAssert(j > 0 && j <= int(rowsize()));
        return ref(i-1,j-1); 
      }
    }

    inline ConstVectorView<T,I> row(int i) const 
    { 
      if (I == FortranStyle) { TMVAssert(i>0 && i<=int(colsize())); --i; }
      else TMVAssert(i>=0 && i<int(colsize()));
      return ConstVectorView<T,I>(itsm.get()+i*stepi(),
          rowsize(),stepj(),NonConj); 
    }

    inline ConstVectorView<T,I> row(int i, int j1, int j2) const 
    { 
      if (I == FortranStyle) {
        TMVAssert(i>0 && i<=int(colsize())); --i; 
        TMVAssert(j1>0 && j1-j2<=0 && j2<=int(rowsize())); --j1; 
      } else {
        TMVAssert(i>=0 && i<int(colsize())); 
        TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      }
      return ConstVectorView<T,I>(itsm.get()+i*stepi()+j1*stepj(),
          j2-j1,stepj(),NonConj); 
    }

    inline ConstVectorView<T,I> operator[](int i) const
    { return row(i); }

    inline ConstVectorView<T,I> col(int j) const
    {
      if (I == FortranStyle) { TMVAssert(j>0 && j<=int(rowsize())); --j; }
      else TMVAssert(j>=0 && j<int(rowsize()));
      return ConstVectorView<T,I>(itsm.get()+j*stepj(),
          colsize(),stepi(),NonConj); 
    }

    inline ConstVectorView<T,I> col(int j, int i1, int i2) const
    {
      if (I == FortranStyle) {
        TMVAssert(j>0 && j<=int(rowsize())); --j; 
        TMVAssert(i1>0 && i1-i2<=0 && i2<=int(colsize())); --i1; 
      } else {
        TMVAssert(j>=0 && j<int(rowsize())); 
        TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      }
      return ConstVectorView<T,I>(itsm.get()+i1*stepi()+j*stepj(),
          i2-i1,stepi(),NonConj); 
    }

    inline ConstVectorView<T,I> diag() const
    {
      return ConstVectorView<T,I>(itsm.get(),MIN(colsize(),rowsize()),
          stepi()+stepj(),NonConj);
    }

    inline ConstVectorView<T,I> diag(int i) const
    {
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        const size_t diagsize = MIN(colsize(),rowsize()-i);
        return ConstVectorView<T,I>(itsm.get()+i*stepj(),
            diagsize,diagstep,NonConj);
      } else {
        const size_t diagsize = MIN(colsize()+i,rowsize());
        return ConstVectorView<T,I>(itsm.get()-i*stepi(),
            diagsize,diagstep,NonConj);
      }
    }

    inline ConstVectorView<T,I> diag(int i, int j1, int j2) const
    {
      if (I == FortranStyle) { TMVAssert(j1 > 0 && j1-j2<=0); --j1; }
      else { TMVAssert( j1>=0 && j1-j2<=0); }
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + 1;
      if (i >= 0) {
        TMVAssert(j2<=MIN(int(colsize()),int(rowsize())-i));
        return ConstVectorView<T,I>(itsm.get()+i*stepj() + j1*diagstep,
            j2-j1, diagstep, NonConj);
      } else {
        TMVAssert(j2<=MIN(int(colsize())+i,int(rowsize())));
        return ConstVectorView<T,I>(itsm.get()-i*stepi() + j1*diagstep,
            j2-j1, diagstep, NonConj);
      }
    }

    inline VectorView<T,I> row(int i)
    { 
      if (I == FortranStyle) { TMVAssert(i>0 && i<=int(colsize())); --i; }
      else TMVAssert(i>=0 && i<int(colsize()));
      return VectorView<T,I>(ptr()+i*stepi(),
          rowsize(),stepj(),NonConj FIRSTLAST); 
    }

    inline VectorView<T,I> row(int i, int j1, int j2)
    { 
      if (I == FortranStyle) {
        TMVAssert(i>0 && i<=int(colsize())); --i; 
        TMVAssert(j1>0 && j1-j2<=0 && j2<=int(rowsize())); --j1; 
      } else {
        TMVAssert(i>=0 && i<int(colsize())); 
        TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      }
      return VectorView<T,I>(ptr()+i*stepi()+j1*stepj(),
          j2-j1,stepj(),NonConj FIRSTLAST); 
    }

    inline VectorView<T,I> operator[](int i)
    { return row(i); }

    inline VectorView<T,I> col(int j)
    {
      if (I == FortranStyle) { TMVAssert(j>0 && j<=int(rowsize())); --j; }
      else TMVAssert(j>=0 && j<int(rowsize()));
      return VectorView<T,I>(ptr()+j*stepj(),
          colsize(),stepi(),NonConj FIRSTLAST); 
    }

    inline VectorView<T,I> col(int j, int i1, int i2)
    {
      if (I == FortranStyle) {
        TMVAssert(j>0 && j<=int(rowsize())); --j; 
        TMVAssert(i1>0 && i1-i2<=0 && i2<=int(colsize())); --i1; 
      } else {
        TMVAssert(j>=0 && j<int(rowsize())); 
        TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      }
      return VectorView<T,I>(ptr()+i1*stepi()+j*stepj(),
          i2-i1,stepi(),NonConj FIRSTLAST); 
    }

    inline VectorView<T,I> diag()
    {
      return VectorView<T,I>(ptr(),
          MIN(colsize(),rowsize()),stepi()+stepj(),NonConj FIRSTLAST);
    }

    inline VectorView<T,I> diag(int i)
    {
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        const size_t diagsize = MIN(colsize(),rowsize()-i);
        return VectorView<T,I>(ptr()+i*stepj(),
            diagsize,diagstep,NonConj FIRSTLAST);
      } else {
        const size_t diagsize = MIN(colsize()+i,rowsize());
        return VectorView<T,I>(ptr()-i*stepi(),
            diagsize,diagstep,NonConj FIRSTLAST);
      }
    }

    inline VectorView<T,I> diag(int i, int j1, int j2) 
    {
      if (I == FortranStyle) { TMVAssert(j1 > 0 && j1-j2<=0); --j1; }
      else { TMVAssert(j1>=0 && j1-j2<=0); }
      TMVAssert(i>=-int(colsize()) && i<=int(rowsize()));
      const int diagstep = stepi() + stepj();
      if (i >= 0) {
        TMVAssert(j2<=MIN(int(colsize()),int(rowsize())-i));
        return VectorView<T,I>(ptr()+i*stepj() + j1*diagstep,
            j2-j1, diagstep, NonConj FIRSTLAST);
      } else {
        TMVAssert(j2<=MIN(int(colsize()),int(rowsize())-i));
        return VectorView<T,I>(ptr()-i*stepi() + j1*diagstep,
            j2-j1, diagstep, NonConj FIRSTLAST);
      }
    }

    //
    // Modifying Functions
    //

    inline Matrix<T,S,I>& Zero() 
    { LinearView().Zero(); return *this; }

    inline Matrix<T,S,I>& Clip(RealType(T) thresh)
    { LinearView().Clip(thresh); return *this; }

    inline Matrix<T,S,I>& SetAllTo(T x) 
    { LinearView().SetAllTo(x); return *this; }

    inline Matrix<T,S,I>& TransposeSelf() 
    { View().TransposeSelf(); return *this; }

    inline Matrix<T,S,I>& ConjugateSelf() 
    { LinearView().ConjugateSelf(); return *this; }

    inline Matrix<T,S,I>& SetToIdentity(T x=T(1)) 
    { 
      TMVAssert(colsize() == rowsize());
      Zero(); diag().SetAllTo(x);
      return *this; 
    }

    inline Matrix<T,S,I>& SwapRows(int i1, int i2)
    { 
      if (I == CStyle) { 
        TMVAssert(i1>=0 && i1<int(colsize()));
        TMVAssert(i2>=0 && i2<int(colsize())); 
      } else { 
        TMVAssert(i1>0 && i1<=int(colsize()));
        TMVAssert(i2>0 && i2<=int(colsize())); 
      }
      if (i1!=i2) Swap(row(i1),row(i2));
      return *this; 
    }

    inline Matrix<T,S,I>& SwapCols(int j1, int j2)
    { 
      if (I == CStyle) { 
        TMVAssert(j1>=0 && j1<int(rowsize()));
        TMVAssert(j2>=0 && j2<int(rowsize())); 
      } else { 
        TMVAssert(j1>0 && j1<=int(rowsize()));
        TMVAssert(j2>0 && j2<=int(rowsize())); 
      }
      if (j1!=j2) Swap(col(j1),col(j2));
      return *this; 
    }

    inline Matrix<T,S,I>& PermuteRows(const int* p, int i1, int i2)
    { View().PermuteRows(p,i1,i2); return *this; }

    inline Matrix<T,S,I>& PermuteRows(const int* p)
    { View().PermuteRows(p); return *this; }

    inline Matrix<T,S,I>& PermuteCols(const int* p, int j1, int j2)
    { View().PermuteCols(p,j1,j2); return *this; }

    inline Matrix<T,S,I>& PermuteCols(const int* p)
    { View().PermuteCols(p); return *this; }

    inline Matrix<T,S,I>& ReversePermuteRows(
        const int* p, int i1, int i2)
    { View().ReversePermuteRows(p,i1,i2); return *this; }

    inline Matrix<T,S,I>& ReversePermuteRows(const int* p)
    { View().ReversePermuteRows(p); return *this; }

    inline Matrix<T,S,I>& ReversePermuteCols(
        const int* p, int j1, int j2)
    { View().ReversePermuteCols(p,j1,j2); return *this; }

    inline Matrix<T,S,I>& ReversePermuteCols(const int* p)
    { View().ReversePermuteCols(p); return *this; }

    //
    // SubMatrix
    //

    inline ConstMatrixView<T,I> SubMatrix(
        int i1, int i2, int j1, int j2) const
    {
      TMVAssert(View().OKSubMatrix(i1,i2,j1,j2,1,1));
      if (I == FortranStyle) { --i1; --j1; }
      return ConstMatrixView<T,I>(itsm.get()+i1*stepi()+j1*stepj(),
          i2-i1,j2-j1,stepi(),stepj(),S,NonConj);
    }

    inline ConstMatrixView<T,I> SubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) const
    {
      TMVAssert(View().OKSubMatrix(i1,i2,j1,j2,istep,jstep));
      StorageType newstor = S == RowMajor ?
      jstep == 1 ? RowMajor : NoMajor :
      istep == 1 ? ColMajor : NoMajor;
      if (I == FortranStyle) { --i1; --j1; i2 += istep-1; j2 += jstep-1; }
      return ConstMatrixView<T,I>(itsm.get()+i1*stepi()+j1*stepj(),
          (i2-i1)/istep,(j2-j1)/jstep,istep*stepi(),jstep*stepj(), 
          newstor,NonConj);
    }

    inline ConstVectorView<T,I> SubVector(
        int i, int j, int istep, int jstep, int s) const
    {
      TMVAssert(View().OKSubVector(i,j,istep,jstep,s));
      if (I==FortranStyle) { --i; --j; }
      return ConstVectorView<T,I>(itsm.get()+i*stepi()+j*stepj(),s,
          istep*stepi()+jstep*stepj(),NonConj);
    }

    inline ConstMatrixView<T,I> ColPair(int j1, int j2) const
    {
      if (I == CStyle) 
        TMVAssert(j1>=0 && j1<int(rowsize()) && j2>=0 && j2<int(rowsize())); 
      else  {
        TMVAssert(j1>0 && j1<=int(rowsize()) && j2>0 && j2<=int(rowsize())); 
        --j1; --j2;
      }
      StorageType newstor = S == RowMajor ?
      j2==j1+1 ? RowMajor : NoMajor : ColMajor;
      return ConstMatrixView<T,I>(itsm.get()+j1*stepj(),colsize(),2,
          stepi(),(j2-j1)*stepj(),newstor,NonConj);
    }

    inline ConstMatrixView<T,I> RowPair(int i1, int i2) const
    {
      if (I == CStyle)  
        TMVAssert(i1>=0 && i1<int(colsize()) && i2>=0 && i2<int(colsize()));
      else  {
        TMVAssert(i1>0 && i1<=int(colsize()) && i2>0 && i2<=int(colsize())); 
        --i1; --i2;
      }
      StorageType newstor = S == RowMajor ?
      RowMajor : i2==i1+1 ? ColMajor : NoMajor;
      return ConstMatrixView<T,I>(itsm.get()+i1*stepi(),2,rowsize(),
          (i2-i1)*stepi(),stepj(),newstor,NonConj);
    }

    inline ConstMatrixView<T,I> Cols(int j1, int j2) const
    {
      if (I==FortranStyle) { 
        TMVAssert(j1>0 && j1-j2<=0 && j2<=int(rowsize())); 
        --j1;
      } else {
        TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize())); 
      }
      return ConstMatrixView<T,I>(itsm.get()+j1*stepj(),colsize(),j2-j1,
          stepi(),stepj(),S,NonConj,iscm()?1:0);
    }

    inline ConstMatrixView<T,I> Rows(int i1, int i2) const
    {
      if (I==FortranStyle) {
        TMVAssert(i1>0 && i1-i2<=0 && i2<=int(colsize())); 
        --i1;
      } else {
        TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize())); 
      }
      return ConstMatrixView<T,I>(itsm.get()+i1*stepi(),i2-i1,rowsize(),
          stepi(),stepj(),S,NonConj,isrm()?1:0);
    }

    inline ConstMatrixView<RealType(T)> Real() const
    {
      return ConstMatrixView<RealType(T)>(
          reinterpret_cast<const RealType(T)*>(itsm.get()),colsize(),rowsize(),
          IsReal(T()) ? stepi() : 2*stepi(),
          IsReal(T()) ? stepj() : 2*stepj(),
          IsReal(T()) ? S : NoMajor,NonConj);
    }

    inline ConstMatrixView<RealType(T)> Imag() const
    {
      TMVAssert(IsComplex(T()));
      return ConstMatrixView<RealType(T)>(
          reinterpret_cast<const RealType(T)*>(itsm.get())+1,
          colsize(),rowsize(),2*stepi(),2*stepj(),NoMajor,NonConj);
    }
    inline MatrixView<T,I> SubMatrix(int i1, int i2, int j1, int j2)
    {
      TMVAssert(View().OKSubMatrix(i1,i2,j1,j2,1,1));
      if (I==FortranStyle) { --i1; --j1; }
      return MatrixView<T,I>(ptr()+i1*stepi()+j1*stepj(),
          i2-i1,j2-j1,stepi(),stepj(),S,NonConj FIRSTLAST);
    }

    inline MatrixView<T,I> SubMatrix(
        int i1, int i2, int j1, int j2, int istep, int jstep) 
    {
      TMVAssert(View().OKSubMatrix(i1,i2,j1,j2,istep,jstep));
      StorageType newstor = S == RowMajor ?
      jstep == 1 ? RowMajor : NoMajor :
      istep == 1 ? ColMajor : NoMajor;
      if (I == FortranStyle) { --i1; --j1; i2 += istep-1; j2 += jstep-1; }
      return MatrixView<T,I>(ptr()+i1*stepi()+j1*stepj(),
          (i2-i1)/istep,(j2-j1)/jstep,istep*stepi(),jstep*stepj(), 
          newstor,NonConj FIRSTLAST);
    }

    inline VectorView<T,I> SubVector(
        int i, int j, int istep, int jstep, int s) 
    {
      TMVAssert(View().OKSubVector(i,j,istep,jstep,s));
      if (I == FortranStyle) { --i; --j; }
      return VectorView<T,I>(ptr()+i*stepi()+j*stepj(),s,
          istep*stepi()+jstep*stepj(),NonConj FIRSTLAST);
    }

    inline MatrixView<T,I> ColPair(int j1, int j2) 
    {
      if (I == CStyle) 
        TMVAssert(j1>=0 && j1<int(rowsize()) && j2>=0 && j2<int(rowsize()));
      else {
        TMVAssert(j1>0 && j1<=int(rowsize()) && j2>0 && j2<=int(rowsize()));
        --j1; --j2;
      }
      StorageType newstor = S == RowMajor ?
      j2==j1+1 ? RowMajor : NoMajor : ColMajor;
      return MatrixView<T,I>(ptr()+j1*stepj(),colsize(),2,
          stepi(),(j2-j1)*stepj(),newstor,NonConj FIRSTLAST);
    }

    inline MatrixView<T,I> RowPair(int i1, int i2) 
    {
      if (I == CStyle)
        TMVAssert(i1>=0 && i1<int(colsize()) && i2>=0 && i2<int(colsize()));
      else {
        TMVAssert(i1>0 && i1<=int(colsize()) && i2>0 && i2<=int(colsize()));
        --i1; --i2;
      }
      StorageType newstor = S == RowMajor ?
      RowMajor : i2==i1+1 ? ColMajor : NoMajor;
      return MatrixView<T,I>(ptr()+i1*stepi(),2,rowsize(),
          (i2-i1)*stepi(),stepj(),newstor,NonConj FIRSTLAST);
    }

    inline MatrixView<T,I> Cols(int j1, int j2) 
    {
      if (I==FortranStyle) { 
        TMVAssert(j1>0 && j1-j2<=0 && j2<=int(rowsize()));
        --j1;
      } else {
        TMVAssert(j1>=0 && j1-j2<=0 && j2<=int(rowsize()));
      }
      return MatrixView<T,I>(ptr()+j1*stepj(),colsize(),j2-j1,
          stepi(),stepj(),S,NonConj,iscm()?1:0 FIRSTLAST);
    }

    inline MatrixView<T,I> Rows(int i1, int i2) 
    {
      if (I==FortranStyle) {
        TMVAssert(i1>0 && i1-i2<=0 && i2<=int(colsize()));
        --i1;
      } else {
        TMVAssert(i1>=0 && i1-i2<=0 && i2<=int(colsize()));
      }
      return MatrixView<T,I>(ptr()+i1*stepi(),i2-i1,rowsize(),
          stepi(),stepj(),S,NonConj,isrm()?1:0 FIRSTLAST);
    }

    inline MatrixView<RealType(T)> Real() 
    {
      return MatrixView<RealType(T)>(
          reinterpret_cast<RealType(T)*>(ptr()),colsize(),rowsize(),
          IsReal(T()) ? stepi() : 2*stepi(),
          IsReal(T()) ? stepj() : 2*stepj(),
          IsReal(T()) ? S : NoMajor,NonConj
#ifdef TMVFLDEBUG
          ,reinterpret_cast<const RealType(T)*>(first)+1
          ,reinterpret_cast<const RealType(T)*>(last)+1
#endif
          );
    }

    inline MatrixView<RealType(T)> Imag() 
    {
      TMVAssert(IsComplex(T()));
      return MatrixView<RealType(T)>(
          reinterpret_cast<RealType(T)*>(ptr())+1,
          colsize(),rowsize(),2*stepi(),2*stepj(),NoMajor,NonConj
#ifdef TMVFLDEBUG
          ,reinterpret_cast<const RealType(T)*>(first)+1
          ,reinterpret_cast<const RealType(T)*>(last)+1
#endif
          );
    }

    //
    // Views
    //

    inline ConstMatrixView<T,I> View() const
    { 
      return ConstMatrixView<T,I>(itsm.get(),colsize(),rowsize(),
          stepi(),stepj(),S,NonConj,linsize);
    }

    inline ConstMatrixView<T,I> Transpose() const
    { 
      return ConstMatrixView<T,I>(itsm.get(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(S),NonConj,linsize);
    }

    inline ConstMatrixView<T,I> Conjugate() const
    { 
      return ConstMatrixView<T,I>(itsm.get(),colsize(),rowsize(),
          stepi(),stepj(),S,ConjOf(T,NonConj),linsize);
    }

    inline ConstMatrixView<T,I> Adjoint() const
    { 
      return ConstMatrixView<T,I>(itsm.get(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(S),ConjOf(T,NonConj),linsize);
    }

    inline ConstUpperTriMatrixView<T,I> UpperTri(
        DiagType dt=NonUnitDiag) const
    { 
      return ConstUpperTriMatrixView<T,I>(cptr(),rowsize(),
          stepi(),stepj(),dt,stor(),ct() FIRSTLAST);
    }

    inline ConstLowerTriMatrixView<T,I> LowerTri(
        DiagType dt=NonUnitDiag) const
    { 
      return ConstLowerTriMatrixView<T,I>(cptr(),colsize(),
          stepi(),stepj(),dt,stor(),ct() FIRSTLAST);
    }

    inline ConstVectorView<T,I> ConstLinearView() const
    {
      TMVAssert(linsize == colsize()*rowsize());
      return ConstVectorView<T,I>(itsm.get(),linsize,1,NonConj); 
    }

    inline MatrixView<T,I> View()
    { 
      return MatrixView<T,I>(ptr(),colsize(),rowsize(),
          stepi(),stepj(),S,NonConj,linsize FIRSTLAST);
    }

    inline MatrixView<T,I> Transpose()
    { 
      return MatrixView<T,I>(ptr(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(S),NonConj,linsize FIRSTLAST);
    }

    inline MatrixView<T,I> Conjugate()
    { 
      return MatrixView<T,I>(ptr(),colsize(),rowsize(),
          stepi(),stepj(),S,ConjOf(T,NonConj),linsize FIRSTLAST);
    }

    inline MatrixView<T,I> Adjoint()
    { 
      return MatrixView<T,I>(ptr(),rowsize(),colsize(),
          stepj(),stepi(),TransOf(S),ConjOf(T,NonConj),linsize FIRSTLAST);
    }

    inline UpperTriMatrixView<T,I> UpperTri(
        DiagType dt=NonUnitDiag)
    { 
      return UpperTriMatrixView<T,I>(ptr(),rowsize(),
          stepi(),stepj(),dt,stor(),ct() FIRSTLAST);
    }

    inline LowerTriMatrixView<T,I> LowerTri(
        DiagType dt=NonUnitDiag)
    { 
      return LowerTriMatrixView<T,I>(ptr(),colsize(),
          stepi(),stepj(),dt,stor(),ct() FIRSTLAST);
    }

    inline VectorView<T,I> LinearView()
    {
      TMVAssert(linsize == colsize()*rowsize());
      return VectorView<T,I>(ptr(),linsize,1,NonConj FIRSTLAST); 
    }

    virtual inline size_t ls() const { return linsize; }
    virtual inline size_t colsize() const { return itscs; }
    virtual inline size_t rowsize() const { return itsrs; }
    virtual inline const T* cptr() const { return itsm.get(); }
    inline T* ptr() { return itsm.get(); }
    virtual inline int stepi() const 
    { return S == RowMajor ? itsrs : 1; }
    virtual inline int stepj() const 
    { return S == RowMajor ? 1 : itscs; }
    inline bool isrm() const { return S==RowMajor; }
    inline bool iscm() const { return S==ColMajor; }
    inline bool isconj() const { return false; }
    virtual inline StorageType stor() const { return S; }
    virtual inline ConjType ct() const { return NonConj; }

    virtual inline bool CanLinearize() const { return true; }

    virtual inline T cref(int i, int j) const
    { return itsm.get()[S==RowMajor ? i*stepi()+j : i+j*stepj()]; }

    inline T& ref(int i, int j)
    { return itsm.get()[S==RowMajor ? i*stepi()+j : i+j*stepj()]; }

  protected :

    const size_t linsize;
    auto_array<T> itsm;
    const size_t itscs;
    const size_t itsrs;

#ifdef TMVFLDEBUG
  public:
    const T*const first;
    const T*const last;
#endif

  }; // Matrix

  //---------------------------------------------------------------------------

  //
  // Special Creators: 
  //   RowVectorViewOf(v) = 1xn Matrix with v in only row - Same Storage
  //   ColVectorViewOf(v) = nx1 Matrix with v in only col - Same Storage
  //   MatrixViewOf(m,colsize,rowsize,storage) = MatrixView of m 
  //

  template <class T> 
  inline ConstMatrixView<T> RowVectorViewOf(const GenVector<T>& v)
  {
    return ConstMatrixView<T>(v.cptr(),1,v.size(),v.size(),v.step(),
        v.step()==1?RowMajor:NoMajor,v.ct(),v.step()==1?v.size():0);
  }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> RowVectorViewOf(const ConstVectorView<T,I>& v)
  {
    return ConstMatrixView<T,I>(v.cptr(),1,v.size(),v.size(),v.step(),
        v.step()==1?RowMajor:NoMajor,v.ct(),v.step()==1?v.size():0);
  }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> RowVectorViewOf(const Vector<T,I>& v)
  {
    return ConstMatrixView<T,I>(v.cptr(),1,v.size(),v.size(),1,
        RowMajor,v.ct(),v.size());
  }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> RowVectorViewOf(const VectorView<T,I>& v)
  {
    return MatrixView<T,I>(v.ptr(),1,v.size(),v.size(),v.step(),
        v.step()==1?RowMajor:NoMajor,v.ct(),v.step()==1?v.size():0
        FIRSTLAST1(v.ptr(),v.ptr()+v.size()) );
  }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> RowVectorViewOf(Vector<T,I>& v)
  {
    return MatrixView<T,I>(v.ptr(),1,v.size(),v.size(),1,
        RowMajor,v.ct(),v.size()
        FIRSTLAST1(v.ptr(),v.ptr()+v.size()) );
  }

  template <class T> 
  inline ConstMatrixView<T> ColVectorViewOf(const GenVector<T>& v)
  { 
    return ConstMatrixView<T>(v.cptr(),v.size(),1,v.step(),v.size(),
        v.step()==1?ColMajor:NoMajor,v.ct(),v.step()==1?v.size():0);
  }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> ColVectorViewOf(const ConstVectorView<T,I>& v)
  { 
    return ConstMatrixView<T,I>(v.cptr(),v.size(),1,v.step(),v.size(),
        v.step()==1?ColMajor:NoMajor,v.ct(),v.step()==1?v.size():0);
  }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> ColVectorViewOf(const Vector<T,I>& v)
  { 
    return ConstMatrixView<T,I>(v.cptr(),v.size(),1,1,v.size(),
        ColMajor,v.ct(),v.size());
  }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> ColVectorViewOf(const VectorView<T,I>& v)
  { 
    return MatrixView<T,I>(v.ptr(),v.size(),1,v.step(),v.size(),
        v.step()==1?ColMajor:NoMajor,v.ct(),v.step()==1?v.size():0
        FIRSTLAST1(v.ptr(),v.ptr()+v.size()) );
  }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> ColVectorViewOf(Vector<T,I>& v)
  { 
    return MatrixView<T,I>(v.ptr(),v.size(),1,1,v.size(),
        ColMajor,v.ct(),v.size() 
        FIRSTLAST1(v.ptr(),v.ptr()+v.size()) );
  }

  template <class T> 
  inline MatrixView<T> MatrixViewOf(
      T* m, size_t colsize, size_t rowsize, StorageType stor)
  {
    TMVAssert(stor == RowMajor || stor == ColMajor);
    size_t linsize = colsize * rowsize;
    if (stor == RowMajor) 
      return MatrixView<T>(m,colsize,rowsize,rowsize,1,RowMajor,NonConj,
          linsize FIRSTLAST1(m,m+linsize));
    else 
      return MatrixView<T>(m,colsize,rowsize,1,colsize,ColMajor,NonConj,
          linsize FIRSTLAST1(m,m+linsize));
  }

  template <class T> 
  inline ConstMatrixView<T> MatrixViewOf(
      const T* m, size_t colsize, size_t rowsize, StorageType stor)
  {
    TMVAssert(stor == RowMajor || stor == ColMajor);
    size_t linsize = colsize*rowsize;
    if (stor == RowMajor)
      return ConstMatrixView<T>(m,colsize,rowsize,rowsize,1,RowMajor,NonConj,
          linsize);
    else 
      return ConstMatrixView<T>(m,colsize,rowsize,1,colsize,ColMajor,NonConj,
          linsize);
  }

  //
  // Copy Matrices
  //

  template <class T> 
  void DoCopySameType(const GenMatrix<T>& m1, const MatrixView<T>& m2);

  template <class T> 
  inline void DoCopy(const GenMatrix<T>& m1, const MatrixView<T>& m2)
  { DoCopySameType(m1,m2); }

  template <class T, class T1> 
  inline void DoCopyDiffType(const GenMatrix<T1>& m1, const MatrixView<T>& m2)
  {
    TMVAssert(m2.rowsize() == m1.rowsize());
    TMVAssert(m2.colsize() == m1.colsize());
    TMVAssert(m2.rowsize() > 0);
    TMVAssert(m2.colsize() > 0);
    TMVAssert(m1.ct()==NonConj);
    TMVAssert(m2.ct()==NonConj);
    TMVAssert(!m2.isrm());
    TMVAssert(!m2.SameAs(m1));
    TMVAssert(IsComplex(T()) || IsReal(T1()));

    if (m1.iscm() && m2.iscm() && m1.colsize() > 1) 
      for(int j=0;j<int(m2.rowsize());++j)
        DoCopyDiffType(m1.col(j),m2.col(j));
    else if (m2.colsize() < m2.rowsize())
      if (ShouldReverse(m1.stepj(),m2.stepj()))
        for(int i=0;i<int(m2.colsize());++i) 
          DoCopyDiffType(m1.row(i).Reverse(),m2.row(i).Reverse());
      else
        for(int i=0;i<int(m2.colsize());++i) 
          DoCopyDiffType(m1.row(i),m2.row(i));
    else
      if (ShouldReverse(m1.stepi(),m2.stepi()))
        for(int j=0;j<int(m2.rowsize());++j) 
          DoCopyDiffType(m1.col(j).Reverse(),m2.col(j).Reverse());
      else
        for(int j=0;j<int(m2.rowsize());++j) 
          DoCopyDiffType(m1.col(j),m2.col(j));
  }

  template <class T, class T1> 
  inline void DoCopy(const GenMatrix<T1>& m1, const MatrixView<T>& m2)
  { DoCopyDiffType(m1,m2); }

  template <class T> 
  inline void DoCopy(const GenMatrix<std::complex<T> >&, const MatrixView<T>&)
  { TMVAssert(FALSE); }

  template <class T1, class T2> 
  inline void CopyNonConj(const GenMatrix<T1>& m1, const MatrixView<T2>& m2)
  {
    TMVAssert(IsComplex(T2()) || IsReal(T1()));
    TMVAssert(m2.rowsize() == m1.rowsize());
    TMVAssert(m2.colsize() == m1.colsize());
    TMVAssert(m1.ct() == NonConj);
    TMVAssert(m2.ct() == NonConj);

    if (m2.isrm() || (m1.isrm() && !m2.iscm()))
      DoCopy(m1.Transpose(),m2.Transpose());
    else
      DoCopy(m1,m2);
  }

  template <class T1, class T2> 
  inline void Copy(const GenMatrix<T1>& m1, const MatrixView<T2>& m2)
  {
    TMVAssert(IsComplex(T2()) || IsReal(T1()));
    TMVAssert(m2.rowsize() == m1.rowsize());
    TMVAssert(m2.colsize() == m1.colsize());
    if (m2.rowsize() > 0 && m2.colsize() > 0) {
      if (SameStorage(m1,m2)) {
        if (m2.SameAs(m1)) {} // Do Nothing
        else if (m2.Transpose().SameAs(m1)) m2.TransposeSelf(); 
        else if (m1.isrm()) {
          Matrix<T1,RowMajor> m1x = m1;
          m2 = m1x;
        } else {
          Matrix<T1,ColMajor> m1x = m1;
          m2 = m1x;
        }
      }
      else if (m1.stor()==m2.stor() && m1.CanLinearize() && m2.CanLinearize()) {
        TMVAssert(m1.ConstLinearView().size() == m2.LinearView().size());
        TMVAssert( (m1.stepi()==m2.stepi() && m1.stepj()==m2.stepj()) );
        m2.LinearView() = m1.ConstLinearView();
      }
      else {
        if (m1.isconj())
          if (m2.isconj())
            CopyNonConj(m1.Conjugate(),m2.Conjugate());
          else {
            CopyNonConj(m1.Conjugate(),m2);
            m2.ConjugateSelf();
          }
        else
          if (m2.isconj()) {
            CopyNonConj(m1,m2.Conjugate());
            m2.ConjugateSelf();
          }
          else 
            CopyNonConj(m1,m2);
      }
    }
  }

  //
  // Swap Matrices
  //

  template <class T> 
  void Swap(const MatrixView<T>& m1, const MatrixView<T>& m2);

  template <class T, StorageType S, IndexStyle I> 
  inline void Swap(const MatrixView<T>& m1, Matrix<T,S,I>& m2)
  { Swap(m1,m2.View()); }

  template <class T, StorageType S, IndexStyle I> 
  inline void Swap(Matrix<T,S,I>& m1, const MatrixView<T>& m2)
  { Swap(m1.View(),m2); }

  template <class T, StorageType S1, IndexStyle I1, StorageType S2, IndexStyle I2> 
  inline void Swap(Matrix<T,S1,I1>& m1, Matrix<T,S2,I2>& m2)
  { Swap(m1.View(),m2.View()); }

  //
  // Views of a Matrix:
  //

  template <class T> 
  inline ConstMatrixView<T> Transpose(const GenMatrix<T>& m)
  { return m.Transpose(); }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> Transpose(const ConstMatrixView<T,I>& m)
  { return m.Transpose(); }

  template <class T, StorageType S, IndexStyle I> 
  inline ConstMatrixView<T> Transpose(const Matrix<T,S,I>& m)
  { return m.Transpose(); }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> Transpose(const MatrixView<T,I>& m)
  { return m.Transpose(); }

  template <class T, StorageType S, IndexStyle I> 
  inline MatrixView<T,I> Transpose(Matrix<T,S,I>& m)
  { return m.Transpose(); }

  template <class T> 
  inline ConstMatrixView<T> Conjugate(const GenMatrix<T>& m)
  { return m.Conjugate(); }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> Conjugate(const ConstMatrixView<T,I>& m)
  { return m.Conjugate(); }

  template <class T, StorageType S, IndexStyle I> 
  inline ConstMatrixView<T,I> Conjugate(const Matrix<T,S,I>& m)
  { return m.Conjugate(); }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> Conjugate(const MatrixView<T,I>& m)
  { return m.Conjugate(); }

  template <class T, StorageType S, IndexStyle I> 
  inline MatrixView<T,I> Conjugate(Matrix<T,S,I>& m)
  { return m.Conjugate(); }

  template <class T> 
  inline ConstMatrixView<T> Adjoint(const GenMatrix<T>& m)
  { return m.Adjoint(); }

  template <class T, IndexStyle I> 
  inline ConstMatrixView<T,I> Adjoint(const ConstMatrixView<T,I>& m)
  { return m.Adjoint(); }

  template <class T, StorageType S, IndexStyle I> 
  inline ConstMatrixView<T,I> Adjoint(const Matrix<T,S,I>& m)
  { return m.Adjoint(); }

  template <class T, IndexStyle I> 
  inline MatrixView<T,I> Adjoint(const MatrixView<T,I>& m)
  { return m.Adjoint(); }

  template <class T, StorageType S, IndexStyle I> 
  inline MatrixView<T,I> Adjoint(Matrix<T,S,I>& m)
  { return m.Adjoint(); }

  template <class T> 
  inline QuotXM<T,T> Inverse(const GenMatrix<T>& m)
  { return m.Inverse(); }

  //
  // Matrix ==, != Matrix
  //

  template <class T1, class T2> 
  bool operator==(const GenMatrix<T1>& m1,
      const GenMatrix<T2>& m2);
  template <class T1, class T2> 
  inline bool operator!=(const GenMatrix<T1>& m1,
      const GenMatrix<T2>& m2)
  { return !(m1 == m2); }


  //
  // I/O
  //

  template <class T, StorageType S, IndexStyle I> 
  std::istream& operator>>(
      std::istream& is, auto_ptr<Matrix<T,S,I> >& m);

  template <class T> 
  std::istream& operator>>(std::istream& is, 
      const MatrixView<T>& m);

  template <class T, StorageType S, IndexStyle I> 
  inline std::istream& operator>>(std::istream& is, Matrix<T,S,I>& m)
  { return is>>m.View(); }

} // namespace tmv

#endif
