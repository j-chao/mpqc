//
// keyvalagg.cc
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@ca.sandia.gov>
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

extern "C" {
#include <ctype.h>
#include <stdlib.h>
}
#include <util/keyval/keyval.h>

/////////////////////////////////////////////////////////////////////
// AggregateKeyVal

AggregateKeyVal::AggregateKeyVal(const RefKeyVal&kv0)
{
  kv[0] = kv0;
  kv[1] = 0;
  kv[2] = 0;
  kv[3] = 0;
}

AggregateKeyVal::AggregateKeyVal(const RefKeyVal&kv0,const RefKeyVal&kv1)
{
  kv[0] = kv0;
  kv[1] = kv1;
  kv[2] = 0;
  kv[3] = 0;
}

AggregateKeyVal::AggregateKeyVal(const RefKeyVal&kv0,const RefKeyVal&kv1,
                                 const RefKeyVal&kv2)
{
  kv[0] = kv0;
  kv[1] = kv1;
  kv[2] = kv2;
  kv[3] = 0;
}

AggregateKeyVal::AggregateKeyVal(const RefKeyVal&kv0,const RefKeyVal&kv1,
                                 const RefKeyVal&kv2,const RefKeyVal&kv3)
{
  kv[0] = kv0;
  kv[1] = kv1;
  kv[2] = kv2;
  kv[3] = kv3;
}

AggregateKeyVal::~AggregateKeyVal()
{
}

RefKeyVal
AggregateKeyVal::getkeyval(const char* keyword)
{
  for (int i=0; i<MaxKeyVal && kv[i].nonnull(); i++) {
      kv[i]->exists(keyword);
      seterror(kv[i]->error());
      if (error() != KeyVal::UnknownKeyword) return kv[i];
    }
  return 0;
}

RefKeyValValue
AggregateKeyVal::key_value(const char*arg)
{
  KeyVal* kval = getkeyval(arg).pointer();
  if (kval) return kval->value(arg);
  else return 0;
}

int
AggregateKeyVal::key_exists(const char* key)
{
  KeyVal* kval = getkeyval(key).pointer();
  if (kval) return kval->exists(key);
  else return 0;
}

void
AggregateKeyVal::errortrace(ostream&fp,int n)
{
  offset(fp,n);
  fp << "AggregateKeyVal: error: \"" << errormsg() << "\"" << endl;
  for (int i = 0; i<4; i++) {
      if (kv[i].nonnull()) {
          offset(fp,n); fp << "  KeyVal #" << i << ":" << endl;
          kv[i]->errortrace(fp,n+OffsetDelta);
        }
    }
}

void
AggregateKeyVal::dump(ostream&fp,int n)
{
  offset(fp,n);
  fp << "AggregateKeyVal: error: \"" << errormsg() << "\"" << endl;
  for (int i = 0; i<4; i++) {
      if (kv[i].nonnull()) {
          offset(fp,n); fp << "  KeyVal #" << i << ":" << endl;
          kv[i]->dump(fp,n+OffsetDelta);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// eval: (c-set-style "CLJ")
// End:
