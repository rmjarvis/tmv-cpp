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


#ifndef TMV_DIVIMPL_H
#define TMV_DIVIMPL_H

#include "tmv/TMV_BaseMatrix.h"

namespace tmv {

  template <class T> 
  struct DivHelper<T>::DivImpl 
  {
  public :

    DivImpl(const BaseMatrix<T>& _m) : 
      m(_m), itsdiv(0), itsdt(_m.IsSquare() ? LU : QR),
      inplace(false), cache(false) {}
    ~DivImpl() { if (itsdiv) delete itsdiv; itsdiv=0; }

    const BaseMatrix<T>& m;
    const Divider<T>* itsdiv;
    DivType itsdt;
    bool inplace;
    bool cache;

  private :

    DivImpl(const DivImpl&)
    { TMVAssert(FALSE); }
    DivImpl& operator=(const DivImpl&)
    { TMVAssert(FALSE); return *this; }
  };

}

#endif

