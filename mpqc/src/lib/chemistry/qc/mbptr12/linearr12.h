//
// linearr12.h
//
// Copyright (C) 2003 Edward Valeev
//
// Author: Edward Valeev <edward.valeev@chemistry.gatech.edu>
// Maintainer: EV
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

#ifdef __GNUG__
#pragma interface
#endif

#include <string>
#include <chemistry/qc/basis/integral.h>
#include <chemistry/qc/basis/intdescr.h>
#include <chemistry/qc/basis/tbint.h>

#ifndef _chemistry_qc_mbptr12_linearr12_h
#define _chemistry_qc_mbptr12_linearr12_h

namespace sc {
  namespace LinearR12 {

#if 0
    /// Describes all known correlation factors
    enum CorrelationFactorID {
        // null correlation factor
        NullCorrFactor = -1,
        // Linear R12 correlation factor
        R12CorrFactor = 0,
        // exp(-gamma r12^2) correlation factor -- Gaussian geminal
        G12CorrFactor = 1
    };
#endif
    
    /** CorrelationFactor is a set of one or more two-particle functions
        of the interparticle distance. Each function may be a primitive function
        or a contraction of several functions.
    */
    class CorrelationFactor : public RefCount {
      public:
        /// Definitions of primitive and contracted Geminals
        typedef IntParamsG12::PrimitiveGeminal PrimitiveGeminal;
        typedef IntParamsG12::ContractedGeminal ContractedGeminal;
        /// Vector of contracted 2 particle functions
        typedef std::vector<ContractedGeminal> CorrelationParameters;

        CorrelationFactor(const std::string& label,
                          const CorrelationParameters& params = CorrelationParameters());
        virtual ~CorrelationFactor();

        /// Returns label
        const std::string& label() const;
        /// Returns the number of contracted two-particle functions in the set
        unsigned int nfunctions() const;
        /// Returns contracted function c
        const ContractedGeminal& function(unsigned int c) const;
        /// Returns the number of primitive functions in contraction c
        unsigned int nprimitives(unsigned int c) const;
        /// Returns std::pair<primitive_parameter,coefficient> in primitive p of contraction c
        const PrimitiveGeminal& primitive(unsigned int c, unsigned int p) const;

        /** Returns TwoBodyIntDescr needed to compute matrix elements where correlation
            function f appears in either bra or ket only.
        */
        virtual Ref<TwoBodyIntDescr> tbintdescr(const Ref<Integral>& IF, unsigned int f) const;
        /** Returns TwoBodyIntDescr needed to compute matrix elements where correlation
            functions fbra and fket appear in bra or ket, respectively.
        */
        virtual Ref<TwoBodyIntDescr> tbintdescr(const Ref<Integral>& IF, unsigned int fbra, unsigned int fket) const;
        /// Returns the maximum number of two-body integral types produced by the appropriate integral evaluator
        virtual unsigned int max_num_tbint_types() const =0;
        
        //
        // These functions are used to map the logical type of integrals ([T1,F12], etc.) to concrete types as produced by TwoBodyInt
        //
        
        /// Returns (int)TwoBodyInt::tbint_type corresponding to electron repulsion integrals
        virtual int tbint_type_eri() const;
        /// Returns (int)TwoBodyInt::tbint_type corresponding to integrals over correlation operator
        virtual int tbint_type_f12() const;
        /// Returns (int)TwoBodyInt::tbint_type corresponding to integrals over [T1,f12]
        virtual int tbint_type_t1f12() const;
        /// Returns (int)TwoBodyInt::tbint_type corresponding to integrals over [T2,f12]
        virtual int tbint_type_t2f12() const;
        /// Returns (int)TwoBodyInt::tbint_type corresponding to integrals over f12/r12
        virtual int tbint_type_f12eri() const;
        /// Returns (int)TwoBodyInt::tbint_type corresponding to integrals over f12^2
        virtual int tbint_type_f12f12() const;
        /// Returns (int)TwoBodyInt::tbint_type corresponding to integrals over [f12,[T1,f12]]
        virtual int tbint_type_f12t1f12() const;
        
        /// print the correlation factor
        void print(std::ostream& os = ExEnv::out0()) const;
        
      private:
        CorrelationParameters params_;
        std::string label_;
    };
    
    /** NullCorrelationFactor stands for no correlation factor */
    class NullCorrelationFactor : public CorrelationFactor {
      public:
      NullCorrelationFactor();
      
      /// Implementation of CorrelationFactor::max_num_tbint_types()
      unsigned int max_num_tbint_types() const { return 1; }
      /// Implementation of CorrelationFactor::tbint_type_eri()
      int tbint_type_eri() const;
    };
    
    /** R12CorrelationFactor stands for no correlation factor */
    class R12CorrelationFactor : public CorrelationFactor {
      public:
      R12CorrelationFactor();
      
      /// Implementation of CorrelationFactor::max_num_tbint_types()
      unsigned int max_num_tbint_types() const { return 4; }
      /// Implementation of CorrelationFactor::tbint_type_eri()
      int tbint_type_eri() const;
      /// Implementation of CorrelationFactor::tbint_type_f12()
      int tbint_type_f12() const;
      /// Implementation of CorrelationFactor::tbint_type_t1f12()
      int tbint_type_t1f12() const;
      /// Implementation of CorrelationFactor::tbint_type_t2f12()
      int tbint_type_t2f12() const;
      /// Overload of CorrelationFactor::tbintdescr(f)
      Ref<TwoBodyIntDescr> tbintdescr(const Ref<Integral>& IF, unsigned int f) const;
    };
    
    /** G12CorrelationFactor stands for no correlation factor */
    class G12CorrelationFactor : public CorrelationFactor {
      public:
      G12CorrelationFactor(const CorrelationParameters& params);
      
      /// Implementation of CorrelationFactor::max_num_tbint_types()
      unsigned int max_num_tbint_types() const { return 6; }
      /// Implementation of CorrelationFactor::tbint_type_eri()
      int tbint_type_eri() const;
      /// Implementation of CorrelationFactor::tbint_type_f12()
      int tbint_type_f12() const;
      /// Implementation of CorrelationFactor::tbint_type_t1f12()
      int tbint_type_t1f12() const;
      /// Implementation of CorrelationFactor::tbint_type_t2f12()
      int tbint_type_t2f12() const;
      /// Implementation of CorrelationFactor::tbint_type_f12eri()
      int tbint_type_f12eri() const;
      /// Implementation of CorrelationFactor::tbint_type_f12f12()
      int tbint_type_f12f12() const;
      /// Implementation of CorrelationFactor::tbint_type_f12t1f12()
      int tbint_type_f12t1f12() const;
      /// Overload of CorrelationFactor::tbintdescr(f)
      Ref<TwoBodyIntDescr> tbintdescr(const Ref<Integral>& IF, unsigned int f) const;
      /// Overload of CorrelationFactor::tbintdescr(fbra,fket)
      Ref<TwoBodyIntDescr> tbintdescr(const Ref<Integral>& IF, unsigned int fbra, unsigned int fket) const;
    };
    
    enum StandardApproximation {StdApprox_A = 0,
				StdApprox_Ap = 1,
				StdApprox_B = 2};
    enum ABSMethod {ABS_ABS = 0,
		    ABS_ABSPlus = 1,
		    ABS_CABS = 2,
		    ABS_CABSPlus = 3};
  }

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:


