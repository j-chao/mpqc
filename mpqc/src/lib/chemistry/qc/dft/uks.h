//
// uks.h --- definition of the unrestricted Kohn-Sham class
//
// Copyright (C) 1997 Limit Point Systems, Inc.
//
// Author: Edward Seidl <seidl@janed.com>
// Maintainer: LPS
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#ifndef _chemistry_qc_scf_uks_h
#define _chemistry_qc_scf_uks_h

#ifdef __GNUC__
#pragma interface
#endif

#include <chemistry/qc/scf/uscf.h>
#include <chemistry/qc/dft/integrator.h>
#include <chemistry/qc/dft/functional.h>

// //////////////////////////////////////////////////////////////////////////

class UKS: public UnrestrictedSCF {
#   define CLASSNAME UKS
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
  protected:
    RefDenIntegrator integrator_;
    RefDenFunctional functional_;
    RefSymmSCMatrix vaxc_;
    RefSymmSCMatrix vbxc_;

  public:
    UKS(StateIn&);
    UKS(const RefKeyVal&);
    ~UKS();

    void save_data_state(StateOut&);

    void print(ostream&o=cout) const;

    void two_body_energy(double &ec, double &ex);

    int value_implemented() const;
    int gradient_implemented() const;

  protected:
    double exc_;
    
    void ao_fock();
    double scf_energy();
    RefSCExtrapData extrap_data();
    void two_body_deriv(double*);
};

#endif

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
