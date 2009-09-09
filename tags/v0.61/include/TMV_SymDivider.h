///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// The Template Matrix/Vector Library for C++ was created by Mike Jarvis     //
// Copyright (C) 2008                                                        //
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
// This file defines the TMV SymDivider class.
//
// It adds Inverse(SymMatrix) capability to the regular Divier class.
//

#ifndef TMV_SymDivider_H
#define TMV_SymDivider_H

#include "TMV_Divider.h"
#include "TMV_BaseSymMatrix.h"

namespace tmv {

#define RT RealType(T)
#define CT ComplexType(T)

  template <class T> class SymDivider : public Divider<T>
  {

    public :

      SymDivider() {}
      virtual ~SymDivider() {}

      using Divider<T>::Inverse;
      virtual void Inverse(const SymMatrixView<RT>& sinv) const =0;
      virtual void Inverse(const SymMatrixView<CT>& sinv) const =0;
  };

#ifdef XDEBUG
  template <class T> inline std::string Type(const SymDivider<T>& d)
  { return std::string("SymDivider<")+tmv::Type(T())+">{"+d.Type()+"}"; }
#endif

#undef RT
#undef CT

} // namespace tmv

#endif
