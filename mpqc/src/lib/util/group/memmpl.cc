//
// memmpl.cc
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@limitpt.com>
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

#ifndef _util_group_memmpl_cc
#define _util_group_memmpl_cc

#ifdef __GNUC__
#pragma implementation
#endif

#include <unistd.h>
#include <util/misc/formio.h>
#include <util/group/memmpl.h>

#include <mpi.h>
#include <mpproto.h>

using namespace std;

///////////////////////////////////////////////////////////////////////
// The handler function and its data

static volatile int global_source, global_type, global_mid;
static MPLMemoryGrp *global_mpl_mem = 0;

void
MPLMemoryGrp::static_handler(int*msgid_arg)
{
  if (!global_mpl_mem) {
      ExEnv::err() << scprintf("WARNING: Tried to call mpl_memory_handler"
              " without global_mpl_mem\n");
    }
  else {
      long grpmid = global_mpl_mem->grp_mid(*msgid_arg);
      global_mpl_mem->handler(&grpmid);
    }
}

///////////////////////////////////////////////////////////////////////
// The MPLMemoryGrp class

static ClassDesc MPLMemoryGrp_cd(
  typeid(MPLMemoryGrp),"MPLMemoryGrp",1,"public MIDMemoryGrp",
  0, create<MPLMemoryGrp>, 0);


void
MPLMemoryGrp::init_mid()
{
  for (int i=0; i<max_mid; i++) mid_ready_[i] = 1;
}

long
MPLMemoryGrp::get_mid(char info)
{
  for (int i=0; i<max_mid; i++) {
      if (mid_ready_[i]) {
          mid_ready_[i] = 0;
          info_[i] = info;
          if (debug_) {
              ExEnv::out() << "MPLMemoryGrp::get_mid(): got " << i
                   << ", current mids:";
              for (int ii=0; ii<max_mid; ii++) {
                  if (!mid_ready_[ii]) {
                      ExEnv::out() << " " << ii << info_[ii]
                           << "(" << handles_[ii] << ")";
                    }
                }
              ExEnv::out() << endl;
            }
          if (info == 'P') {
              for (int ii=0; ii<max_mid; ii++) {
                  if (info_[ii] == 'P' && ii != i && !mid_ready_[i]) {
                      ExEnv::err() << "MPLMemoryGrp: double post" << endl;
                      abort();
                    }
                }
            }
          return i;
        }
    }

  ExEnv::err() << "MPLMemoryGrp::get_mid(): ran out of mid's" << endl;
  abort();
  return 0;
}

long
MPLMemoryGrp::grp_mid(int mpc_mid)
{
  int i;
  for (i=0; i<max_mid; i++) {
      if (!mid_ready_[i] && handles_[i] == mpc_mid) return i;
    }

  ExEnv::err() << "MPLMemoryGrp::grp_mid(): invalid mid: " << mpc_mid << endl;
  for (i=0; i<max_mid; i++) {
      if (mid_ready_[i]) ExEnv::out() << "mid " << i << " is unused" << endl;
      else ExEnv::out() << "mid " << i << " has handle " << handles_[i] << endl;
    }
  abort();
  return 0;
}

void
MPLMemoryGrp::free_mid(long mid)
{
  if (debug_) ExEnv::out() << "MPLMemoryGrp::free_mid(): freeing " << mid << endl;
  if (mid_ready_[mid]) {
      ExEnv::err() << "MPLMemoryGrp::free_mid(): mid not in use" << endl;
      abort();
    }
  mid_ready_[mid] = 1;
}

int &
MPLMemoryGrp::mpc_mid(long mid)
{
  if (mid_ready_[mid]) {
      ExEnv::err() << "MPLMemoryGrp::mpc_mid(" << mid << "): not in use" << endl;
    }
  return handles_[mid];
}

long
MPLMemoryGrp::lockcomm()
{
  int oldvalue;
  mpc_lockrnc(1, &oldvalue);
  if (debug_) ExEnv::out() << ">>>> mpc_lockrnc(1," << oldvalue << ") (lock)" << endl;
  return oldvalue;
}

void
MPLMemoryGrp::unlockcomm(long oldvalue)
{
  int old = oldvalue;
  mpc_lockrnc(old, &old);
  if (debug_) ExEnv::out() << ">>>> mpc_lockrnc(" << oldvalue
                   << "," << old << ") (unlock)" << endl;
}

long
MPLMemoryGrp::send(void* data, int nbytes, int node, int type)
{
  int mid = get_mid('S');
  mpc_send(data, nbytes, node, type, &mpc_mid(mid));
  if (debug_) ExEnv::out() << ">>>> mpc_send(,"
                   << nbytes << ","
                   << node << ","
                   << type << ","
                   << mpc_mid(mid) << ")" << endl;
  return mid;
}

long
MPLMemoryGrp::recv(void* data, int nbytes, int node, int type)
{
  int n;
  if (node == -1) n = DONTCARE;
  else n = node;
  int t = type;
  int mid = get_mid('R');
  mpc_recv(data, nbytes, &n, &t, &mpc_mid(mid));
  if (debug_) ExEnv::out() << ">>>> mpc_recv(,"
                   << nbytes << ","
                   << n << ","
                   << t << ","
                   << mpc_mid(mid) << ")" << endl;
  return mid;
}

void
MPLMemoryGrp::postrecv(void *data, int nbytes, int type)
{
  global_type = type;
  global_source = DONTCARE; 
  global_mid = get_mid('P');
  data_request_mid_ = global_mid;
  if (data_request_mid_ == -1) {
      ExEnv::err() << scprintf("MPLMemoryGrp::activate(): bad mid\n");
      abort();
    }
  mpc_rcvncall(data, nbytes,
               (int*)&global_source, (int*)&global_type, &mpc_mid(global_mid),
               static_handler);
  if (debug_) ExEnv::out() << ">>>> mpc_rcvncall(,"
                   << nbytes << ","
                   << ","
                   << global_type << ","
                   << global_mid << ","
                   << ")" << endl;
}

long
MPLMemoryGrp::wait(long mid1, long mid2)
{
  int imid;
  if (mid2 == -1) imid = mpc_mid(mid1);
  else imid = DONTCARE;
  size_t count;
  if (debug_) {
      ExEnv::out() << "MPLMemoryGrp::wait(" << mid1 << "," << mid2
           << "): waiting on " << imid << ": current mids:";
      for (int i=0; i<max_mid; i++) {
          if (!mid_ready_[i]) {
              ExEnv::out() << " " << i << info_[i] << "(" << handles_[i] << ")";
            }
        }
      ExEnv::out() << endl;
    }
  if (mpc_wait(&imid,&count)) {
      ExEnv::err() << scprintf("MPLMemoryGrp: mpc_wait failed\n");
      sleep(1);
      abort();
    }
  if (debug_) ExEnv::out() << ">>>> (after call) mpc_wait("
                   << imid << ","
                   << count << ")" << endl;
  // mpc_wait might clobber imid, so avoid its use
  if (mid2 == -1) {
      free_mid(mid1);
      return mid1;
    }
  long grpmid = grp_mid(imid);
  free_mid(grpmid);
  return grpmid;
}

MPLMemoryGrp::MPLMemoryGrp(const Ref<MessageGrp>& msg):
  MIDMemoryGrp(msg)
{
  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp entered\n");
  if (global_mpl_mem) {
      ExEnv::err() << scprintf("MPLMemoryGrp: only one allowed at a time\n");
      sleep(1);
      abort();
    }

  global_mpl_mem = this;

  use_acknowledgments_ = 0;
  use_active_messages_ = 1;

  init_mid();

  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp activating\n");
  activate();
  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp done\n");
}

MPLMemoryGrp::MPLMemoryGrp(const Ref<KeyVal>& keyval):
  MIDMemoryGrp(keyval)
{
  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp KeyVal entered\n");
  if (global_mpl_mem) {
      ExEnv::err() << scprintf("MPLMemoryGrp: only one allowed at a time\n");
      sleep(1);
      abort();
    }

  global_mpl_mem = this;

  init_mid();

  if (n() == 1) {
      ExEnv::err() << "MPLMemoryGrp: nproc = 1 not allowed" << endl;
      abort();
    }

  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp activating\n");
  activate();
  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp done\n");
}

MPLMemoryGrp::~MPLMemoryGrp()
{
  if (debug_) 
      ExEnv::out() << scprintf("MPLMemoryGrp: in DTOR\n");
  deactivate();

  int oldlock = lockcomm();
  global_mpl_mem = 0;
  unlockcomm(oldlock);
}

void
MPLMemoryGrp::deactivate()
{
  if (!global_mpl_mem) return;
  MIDMemoryGrp::deactivate();
}

#endif

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
