//
// r12ia.cc
//
// Copyright (C) 2002 Edward Valeev
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

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdexcept>
#include <stdlib.h>
#include <util/misc/formio.h>
#include <util/misc/exenv.h>
#include <chemistry/qc/mbptr12/r12ia.h>

using namespace std;
using namespace sc;

/*--------------------------------
  R12IntsAcc
 --------------------------------*/
static ClassDesc R12IntsAcc_cd(
  typeid(R12IntsAcc),"R12IntsAcc",2,"virtual public SavableState",
  0, 0, 0);

R12IntsAcc::R12IntsAcc(int num_te_types, int ni, int nj, int nx, int ny) :
  num_te_types_(num_te_types), ni_(ni), nj_(nj), nx_(nx), ny_(ny),
  nxy_(nx*ny), blksize_(nxy_*sizeof(double)), blocksize_(blksize_*num_te_types_),
  mem_(MemoryGrp::get_default_memorygrp()), active_(false)
{
}

R12IntsAcc::R12IntsAcc(StateIn& si) : SavableState(si),
  mem_(MemoryGrp::get_default_memorygrp()), active_(false)
{
  si.get(num_te_types_);
  si.get(ni_);
  si.get(nj_);
  si.get(nx_);
  si.get(ny_);

  nxy_ = nx_ * ny_;
  blksize_ = nxy_*sizeof(double);
  blocksize_ = blksize_*num_te_types_;
}

R12IntsAcc::~R12IntsAcc()
{
}

void R12IntsAcc::save_data_state(StateOut& so)
{
  so.put(num_te_types_);
  so.put(ni_);
  so.put(nj_);
  so.put(nx_);
  so.put(ny_);
}

int
R12IntsAcc::tasks_with_access(vector<int>& twa_map) const
{
  int nproc = ntasks();
  
  // Compute the number of tasks that have full access to the integrals
  // and split the work among them
  int nproc_with_ints = 0;
  for(int proc=0;proc<nproc;proc++)
    if (has_access(proc)) nproc_with_ints++;
 
  twa_map.resize(nproc);
  int count = 0;
  for(int proc=0;proc<nproc;proc++)
    if (has_access(proc)) {
      twa_map[proc] = count;
      count++;
    }
    else
      twa_map[proc] = -1;

  return nproc_with_ints;
}

namespace sc{ namespace detail {
  
void store_memorygrp(Ref<R12IntsAcc>& acc, Ref<MemoryGrp>& mem, int i_offset,
                     int ni, const size_t blksize_memgrp) {
  const int num_te_types = acc->num_te_types();
  for (int i=0; i<ni; i++) {
    const int ii = i + i_offset;
    for (int j=0; j<acc->nj(); j++) {
      const int ij = acc->ij_index(ii, j);
      const int proc = ij % mem->n();
      const int local_ij_index = ij / mem->n();
      if (proc != mem->me()) {
        distsize_t moffset = (distsize_t)local_ij_index*blksize_memgrp
            *num_te_types + mem->offset(proc);
        const size_t blksize = acc->blksize();
        for (int te_type = 0; te_type < num_te_types; ++te_type) {
          double* data = (double *) mem->obtain_readonly(moffset, blksize);
          acc->store_pair_block(ii, j, te_type, data);
          mem->release_readonly(data, moffset, blksize);
          moffset += blksize_memgrp;
        }
      } else {
        double* data = (double *) ((size_t)mem->localdata() + blksize_memgrp
            *num_te_types*local_ij_index);
        for (int te_type=0; te_type < num_te_types; te_type++) {
          acc->store_pair_block(ii, j, te_type, data);
          data = (double*) ((size_t) data + blksize_memgrp);
        }
      }
    }
  }
}
}} // end of namespace sc::detail

///////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
