//
// statei.h
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

#ifndef CLASSNAME
#define CLASSNAME you_forgot_to_define_CLASSNAME
#endif

#define stringize_(arg) # arg
#define stringize(arg) stringize_(arg)

void
CLASSNAME::save_object_state(StateOut&so)
{
  if (class_desc() != static_class_desc()) {
      cerr <<  "Warning:"
           << stringize(CLASSNAME)
           << "::save_object_state: "
           << "exact type not known -- object not saved" << endl;
      return;
    }
  // save the version info
  //so.put_version(static_class_desc());
  save_vbase_state(so);
  save_data_state(so);
}

CLASSNAME*
CLASSNAME::restore_state(StateIn&si)
{
#ifdef __GNUC__
  return CLASSNAME::castdown(SavableState::restore_state(si));
#else
  return CLASSNAME::castdown(att_hack_job(si));
#endif
}

#undef stringize
#undef stringize_
