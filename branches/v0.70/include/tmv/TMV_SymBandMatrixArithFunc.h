///////////////////////////////////////////////////////////////////////////////
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


#ifndef TMV_SymBandMatrixArithFunc_H
#define TMV_SymBandMatrixArithFunc_H

#include "tmv/TMV_BaseSymBandMatrix.h"
#include "tmv/TMV_BandMatrixArithFunc.h"
#include "tmv/TMV_Array.h"

#define CT std::complex<T>

namespace tmv {

    // y (+)= alpha * A * x
    template <bool add, class T, class Ta, class Tx> 
    void MultMV(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenVector<Tx>& x, VectorView<T> y);

    // A = alpha * A
    template <class T> 
    inline void MultXM(const T alpha, SymBandMatrixView<T> A)
    { MultXM(alpha,A.upperBand()); }

    // B += alpha * A
    template <class T, class Ta> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A,
        SymBandMatrixView<T> B)
    { AddMM(alpha,A.upperBand(),B.upperBand()); }
    template <class T, class Ta> 
    void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A,
        BandMatrixView<T> B);
    template <class T, class Ta> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A,
        MatrixView<T> B)
    { AddMM(alpha,A,BandMatrixViewOf(B,A.nlo(),A.nhi())); }

    // C = alpha * A + beta * B
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B,
        SymBandMatrixView<T> C)
    { AddMM(alpha,A.upperBand(),beta,B.upperBand(),C.upperBand()); }
    template <class T, class Ta, class Tb> 
    void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B,
        BandMatrixView<T> C);
    template <class T, class Ta, class Tb> 
    void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B, MatrixView<T> C);
    template <class T, class Ta, class Tb> 
    void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenBandMatrix<Tb>& B, BandMatrixView<T> C);
    template <class T, class Ta, class Tb> 
    void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenMatrix<Tb>& B, MatrixView<T> C);
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B, BandMatrixView<T> C)
    { AddMM(beta,B,alpha,A,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B, MatrixView<T> C)
    { AddMM(beta,B,alpha,A,C); }


    // C (+)= alpha * A * B
    template <bool add, class T, class Ta, class Tb> 
    void MultMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenMatrix<Tb>& B, MatrixView<T> C);
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenMatrix<Ta>& A, 
        const GenSymBandMatrix<Tb>& B, MatrixView<T> C)
    { MultMM<add>(alpha,B.transpose(),A.transpose(),C.transpose()); }
    template <bool add, class T, class Ta, class Tb> 
    void MultMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenSymBandMatrix<Tb>& B, BandMatrixView<T> C);
    template <bool add, class T, class Ta, class Tb> 
    void MultMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenBandMatrix<Tb>& B, BandMatrixView<T> C);
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenBandMatrix<Ta>& A, 
        const GenSymBandMatrix<Tb>& B, BandMatrixView<T> C)
    { MultMM<add>(alpha,B.transpose(),A.transpose(),C.transpose()); }

    template <class T> 
    class SymBandMatrixComposite : public GenSymBandMatrix<T>
    {
    public :
        inline SymBandMatrixComposite() : itsm(0) {}
        inline SymBandMatrixComposite(const SymBandMatrixComposite<T>&) :
            itsm(0) {}
        virtual inline ~SymBandMatrixComposite() {}

        // Definitions are in TMV_MultsBV.cpp
        const T* cptr() const;
        int stepi() const;
        int stepj() const;
        int diagstep() const;

        inline SymType sym() const { return Sym; }
        inline UpLoType uplo() const { return Lower; }
        inline ConjType ct() const { return NonConj; }

    private:
        mutable AlignedArray<T> itsm1;
        mutable T* itsm;

        SymBandMatrixComposite<T>& operator=(const SymBandMatrixComposite<T>&);
    };


    template <class T> 
    class SymBandMatrixComposite<CT> :
        public BandMatrixComposite<CT>,
        virtual public AssignableToSymBandMatrix<CT>
    {
    public :
        // Need to respecify that size() is pure virtual here, 
        // since GenBandMatrix defines size for compaibility
        // with AssignableToDiagMatrix, etc.
        virtual int size() const = 0;

        using AssignableToSymMatrix<CT>::sym;
        using AssignableToBandMatrix<CT>::nlo;
        inline int nhi() const { return nlo(); }
        inline int colsize() const { return size(); }
        inline int rowsize() const { return size(); }

        inline void assignToS(SymMatrixView<T> m0) const
        {
            TMVAssert(m0.size() == size());
            TMVAssert(m0.sym() == sym());
            this->assignTosB(SymBandMatrixViewOf(m0,nlo()));
            if (m0.size() > nlo()+1)
                BandMatrixViewOf(m0.upperTri()).diagRange(
                    nlo()+1,m0.size()).setZero();
        }
        inline void assignToS(SymMatrixView<CT> m0) const
        {
            TMVAssert(m0.size() == size());
            TMVAssert(m0.sym() == sym());
            this->assignTosB(SymBandMatrixViewOf(m0,nlo()));
            if (m0.size() > nlo()+1)
                BandMatrixViewOf(m0.upperTri()).diagRange(
                    nlo()+1,m0.size()).setZero();
        }

        inline void assignToM(MatrixView<T> m0) const
        { BandMatrixComposite<CT>::assignToM(m0); }
        inline void assignToM(MatrixView<CT> m0) const
        { BandMatrixComposite<CT>::assignToM(m0); }
        inline void assignToB(BandMatrixView<T> m0) const
        { BandMatrixComposite<CT>::assignToB(m0); }
        inline void assignToB(BandMatrixView<CT> m0) const
        { BandMatrixComposite<CT>::assignToB(m0); }
    };

    // Specialize allowed complex combinations:
    template <bool add, class T, class Ta, class Tx> 
    inline void MultMV(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenVector<Tx>& x, VectorView<CT> y)
    { MultMV<add>(CT(alpha),A,x,y); }

    template <class T> 
    inline void MultXM(const T alpha, SymBandMatrixView<CT> A)
    { MultXM(CT(alpha),A); }

    template <class T, class Ta> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A,
        SymBandMatrixView<CT> B)
    { AddMM(CT(alpha),A,B); }
    template <class T, class Ta> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        BandMatrixView<CT> B)
    { AddMM(CT(alpha),A,B); }
    template <class T, class Ta> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, MatrixView<CT> B)
    { AddMM(CT(alpha),A,B); }

    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B,
        SymBandMatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B,
        BandMatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B, MatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenBandMatrix<Tb>& B, BandMatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const T beta, const GenMatrix<Tb>& B, MatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenBandMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B,
        BandMatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const T alpha, const GenMatrix<Ta>& A, 
        const T beta, const GenSymBandMatrix<Tb>& B, MatrixView<CT> C)
    { AddMM(CT(alpha),A,CT(beta),B,C); }
    template <class T> 
    inline void AddMM(
        const CT alpha, const GenSymBandMatrix<CT>& A, 
        const CT beta, const GenSymBandMatrix<T>& B, MatrixView<CT> C)
    { AddMM(beta,B,alpha,A,C); }
    template <class T> 
    inline void AddMM(
        const CT alpha, const GenSymBandMatrix<CT>& A,
        const CT beta, const GenSymBandMatrix<T>& B,
        BandMatrixView<CT> C)
    { AddMM(beta,B,alpha,A,C); }

    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenMatrix<Tb>& B, MatrixView<CT> C)
    { MultMM<add>(CT(alpha),A,B,C); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenMatrix<Ta>& A, 
        const GenSymBandMatrix<Tb>& B, MatrixView<CT> C)
    { MultMM<add>(CT(alpha),A,B,C); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenSymBandMatrix<Tb>& B, BandMatrixView<CT> C)
    { MultMM<add>(CT(alpha),A,B,C); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenSymBandMatrix<Ta>& A, 
        const GenBandMatrix<Tb>& B, BandMatrixView<CT> C)
    { MultMM<add>(CT(alpha),A,B,C); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const T alpha, const GenBandMatrix<Ta>& A, 
        const GenSymBandMatrix<Tb>& B, BandMatrixView<CT> C)
    { MultMM<add>(CT(alpha),A,B,C); }

    // Specialize disallowed complex combinations:
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMV(
        const CT , const GenSymBandMatrix<Ta>& , 
        const GenVector<Tb>& , VectorView<T> )
    { TMVAssert(TMV_FALSE); }

    template <class T> 
    inline void MultXM(
        const CT , SymBandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }

    template <class T, class Ta> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , SymBandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , MatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const CT , const GenSymBandMatrix<Tb>& , SymBandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const CT , const GenSymBandMatrix<Tb>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const CT , const GenSymBandMatrix<Tb>& , MatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const CT , const GenBandMatrix<Tb>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const CT , const GenMatrix<Tb>& , MatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenBandMatrix<Ta>& , 
        const CT , const GenSymBandMatrix<Tb>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <class T, class Ta, class Tb> 
    inline void AddMM(
        const CT , const GenMatrix<Ta>& , 
        const CT , const GenSymBandMatrix<Tb>& , MatrixView<T> )
    { TMVAssert(TMV_FALSE); }

    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const GenMatrix<Tb>& , MatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const CT , const GenMatrix<Ta>& , 
        const GenSymBandMatrix<Tb>& , MatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const GenSymBandMatrix<Tb>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const CT , const GenSymBandMatrix<Ta>& , 
        const GenBandMatrix<Tb>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }
    template <bool add, class T, class Ta, class Tb> 
    inline void MultMM(
        const CT , const GenBandMatrix<Ta>& , 
        const GenSymBandMatrix<Tb>& , BandMatrixView<T> )
    { TMVAssert(TMV_FALSE); }

} // namespace tmv

#undef CT

#endif
