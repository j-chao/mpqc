//
// memmid.cc
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

#ifndef _util_group_memmid_cc
#define _util_group_memmid_cc

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdio.h> // for sprintf
#include <unistd.h>

#include <util/misc/formio.h>
#include <util/group/memmid.h>

using namespace std;

#define dbufsize 8192

///////////////////////////////////////////////////////////////////////
// The handler function and its data

static int ack_serial_number = 0;

void
MIDMemoryGrp::handler(long *mid)
{
  handler(data_request_buffer_, mid);
}

static int handler_depth = 0;

void
MIDMemoryGrp::handler(MemoryDataRequest& buffer, long *msgid_arg)
{
  int i;
  int mid;
  int dsize;
  double chunk[dbufsize];
  int remain;
  int dremain;
  int junk;
  double *source_data;

  handler_depth++;
  if (handler_depth != 1) {
      ExEnv::err() << "MIDMemoryGrp::handler: bad depth: " << handler_depth << endl;
      abort();
    }

  const char *handlerstr;
  if (msgid_arg) handlerstr = "====:";
  else handlerstr = "----:";

  if (debug_) {
      char m[80];
      sprintf(m,"%d: %s", me_, handlerstr);
      buffer.print(m);
    }

  if (msgid_arg) {
      wait(*msgid_arg);
      got_data_request_mid();
    }

  MemoryDataRequest::Request request = buffer.request();
  int offset = buffer.offset();
  int size = buffer.size();
  int node = buffer.node();
  int from_type = data_type_from_handler_;
  int to_type = data_type_to_handler_;
  int reactivate = buffer.reactivate();

  switch (request) {
  case MemoryDataRequest::Deactivate:
      handler_depth--;
      break;
  case MemoryDataRequest::Sync:
      nsync_++;
      handler_depth--;
      if (msgid_arg) {
          if ((me() == 0 && nsync_ < n() - 1) || reactivate) activate();
        }
      break;
  case MemoryDataRequest::Retrieve:
      mid = send(&data_[offset], size, node, from_type);
      wait(mid);
      if (debug_) {
          ExEnv::out() << scprintf(
              "%d: %s sent %d bytes at byte offset %d (mid = %d)\n",
              me_, handlerstr, size, offset, mid);
        }
      n_ret_recv_++;
      handler_depth--;
      if (msgid_arg) activate();
      break;
  case MemoryDataRequest::Replace:
      junk = (me() & 0xff) + (ack_serial_number++ << 8);
      mid = send(&junk, sizeof(junk), node, from_type);
      wait(mid);
      if (debug_) {
          ExEnv::out() << scprintf("%d: %s sent go-ahead %d:%d to %d\n",
                           me_, handlerstr, junk&0xff, junk>>8, node);
        }
      mid = recv(&data_[offset], size, node, to_type);
      wait(mid);
      if (debug_) {
          ExEnv::out() << scprintf(
              "%d: %s replaced %d bytes at byte offset %d (mid = %d)\n",
              me_, handlerstr, size, offset, mid);
        }
      if (use_acknowledgments_) {
          junk = (me() & 0xff) + (ack_serial_number++ << 8);
          mid = send(&junk, sizeof(junk), node, from_type);
          wait(mid);
          if (debug_) {
              ExEnv::out() << scprintf("%d: %s sent ack %d:%d to %d\n",
                               me_, handlerstr, junk&0xff, junk>>8, node);
            }
        }
      n_rep_recv_++;
      handler_depth--;
      if (msgid_arg) activate();
      break;
  case MemoryDataRequest::DoubleSum:
      dsize = size/sizeof(double);
      remain = size;
      dremain = dsize;
      if (offset < 0
          || offset+size > distsize_to_size(offsets_[me()+1]-offsets_[me()])) {
          ExEnv::err() << "MIDMemoryGrp::handler(): bad offset/size for DoubleSum:"
               << " offset = " << offset
               << ", size = " << size
               << ", fence = " << offsets_[me()+1] << endl;
          abort();
        }
      junk = (me() & 0xff) + (ack_serial_number++ << 8);
      mid = send(&junk, sizeof(junk), node, from_type);
      wait(mid);
      if (debug_) {
          ExEnv::out() << scprintf("%d: %s sent go-ahead %d:%d to %d",
                           me_, handlerstr, junk&0xff, junk>>8, node)
               << endl;
        }
      while (remain > 0) {
          int dchunksize = dbufsize;
          if (dremain < dchunksize) dchunksize = dremain;
          int chunksize = dchunksize*sizeof(double);
          mid = recv(chunk, chunksize, node, to_type);
          wait(mid);
          if (debug_) {
              ExEnv::out() << scprintf("%d: %s summing %d bytes (%d doubles) (mid=%d)",
                               me_, handlerstr, size, size/sizeof(double), mid)
                   << endl;
            }
          source_data = (double*) &data_[offset];
          for (i=0; i<dchunksize; i++) {
              source_data[i] += chunk[i];
            }
          dremain -= dchunksize;
          remain -= chunksize;
          offset += chunksize;
        }
      if (use_acknowledgments_) {
          junk = (me() & 0xff) + (ack_serial_number++ << 8);
          mid = send(&junk, sizeof(junk), node, from_type);
          wait(mid);
          if (debug_) {
              ExEnv::out() << scprintf("%d: %s sent ack %d:%d to %d",
                               me_, handlerstr, junk&0xff, junk>>8, node)
                   << endl;
            }
        }
      n_sum_recv_++;
      handler_depth--;
      if (msgid_arg) activate();
      break;
  default:
      ExEnv::err() << scprintf("%d: mid_memory_handler: bad request id\n", me_);
      sleep(1);
      abort();
    }
}

///////////////////////////////////////////////////////////////////////
// The MIDMemoryGrp class

static ClassDesc MIDMemoryGrp_cd(
  typeid(MIDMemoryGrp),"MIDMemoryGrp",1,"public ActiveMsgMemoryGrp",
  0, 0, 0);

MIDMemoryGrp::MIDMemoryGrp(const Ref<MessageGrp>& msg):
  ActiveMsgMemoryGrp(msg)
{
  if (debug_) ExEnv::out() << scprintf("%d: MIDMemoryGrp CTOR\n", me());

  ctl_mask_ = 0x10000;
  intMessageGrp *img = dynamic_cast<intMessageGrp*>(msg_.pointer());
  if (img) {
      ctl_mask_ = img->leftover_ctl_bits();
    }
  data_request_type_ = ctl_mask_ | 113;
  data_type_to_handler_ = ctl_mask_ | 114;
  data_type_from_handler_ = ctl_mask_ | 115;
  nsync_ = 0;
  use_acknowledgments_ = 0;
  use_active_messages_ = 1;
  active_ = 0;

  if (debug_) {
      ExEnv::out() << node0 << indent << "data_request_type = "
           << data_request_type_ << endl;
      ExEnv::out() << node0 << indent << "data_type_to_handler = "
           << data_type_to_handler_ << endl;
      ExEnv::out() << node0 << indent << "data_type_from_handler = "
           << data_type_from_handler_ << endl;
    }

  n_sum_send_ = new int[msg_->n()];
  n_ret_send_ = new int[msg_->n()];
  n_rep_send_ = new int[msg_->n()];
  memset(n_sum_send_,0,msg_->n() * sizeof(int));
  memset(n_ret_send_,0,msg_->n() * sizeof(int));
  memset(n_rep_send_,0,msg_->n() * sizeof(int));

  n_sum_recv_ = 0;
  n_ret_recv_ = 0;
  n_rep_recv_ = 0;
}

MIDMemoryGrp::MIDMemoryGrp(const Ref<KeyVal>& keyval):
  ActiveMsgMemoryGrp(keyval)
{
  if (debug_) ExEnv::out() << scprintf("%d: MIDMemoryGrp KeyVal CTOR\n", me());
  
  nsync_ = 0;

  int default_ctl_mask = 0x10000;
  intMessageGrp *img = dynamic_cast<intMessageGrp*>(msg_.pointer());
  if (img) {
      default_ctl_mask = img->leftover_ctl_bits();
    }

  ctl_mask_ = keyval->intvalue("ctl_mask");
  if (keyval->error() != KeyVal::OK) ctl_mask_ = default_ctl_mask;

  data_request_type_ = keyval->intvalue("request_type");
  if (keyval->error() != KeyVal::OK) data_request_type_ = ctl_mask_ | 113;
  data_type_to_handler_ = keyval->intvalue("to_type");
  if (keyval->error() != KeyVal::OK) data_type_to_handler_ = ctl_mask_ | 114;
  data_type_from_handler_ = keyval->intvalue("from_type");
  if (keyval->error() != KeyVal::OK) data_type_from_handler_ = ctl_mask_ | 115;

  use_acknowledgments_ = keyval->booleanvalue("ack");
  if (keyval->error() != KeyVal::OK) use_acknowledgments_ = 0;
  use_active_messages_ = keyval->booleanvalue("active");
  if (keyval->error() != KeyVal::OK) use_active_messages_ = 1;
  active_ = 0;

  n_sum_send_ = new int[msg_->n()];
  n_ret_send_ = new int[msg_->n()];
  n_rep_send_ = new int[msg_->n()];
  memset(n_sum_send_,0,msg_->n() * sizeof(int));
  memset(n_ret_send_,0,msg_->n() * sizeof(int));
  memset(n_rep_send_,0,msg_->n() * sizeof(int));

  n_sum_recv_ = 0;
  n_ret_recv_ = 0;
  n_rep_recv_ = 0;
}

MIDMemoryGrp::~MIDMemoryGrp()
{
  delete[] n_sum_send_;
  delete[] n_ret_send_;
  delete[] n_rep_send_;

  if (debug_) ExEnv::out() << scprintf("%d: ~MIDMemoryGrp\n", me());
}

void
MIDMemoryGrp::print_memreq(MemoryDataRequest &req,
                           const char *msg, int target)
{
  if (msg == 0) msg = "";

  char newmsg[80];
  if (target == -1) {
      sprintf(newmsg, "%d: %s", me(), msg);
    }
  else {
      sprintf(newmsg, "%d->%d: %s", me(), target, msg);
    }
  req.print(newmsg);
}

void
MIDMemoryGrp::activate()
{
  if (!active_) {
      active_ = 1;
      if (use_active_messages_) {
          postrecv(data_request_buffer_.data(), data_request_buffer_.nbytes(),
                   data_request_type_);
        }
      else {
          data_request_mid_ = recv(data_request_buffer_.data(),
                                   data_request_buffer_.nbytes(),
                                   -1,
                                   data_request_type_);
          if (data_request_mid_ == -1) {
              ExEnv::err() << scprintf("MIDMemoryGrp::activate(): bad mid\n");
              abort();
            }
        }
      if (debug_) {
          ExEnv::out() << scprintf("%d: activated memory request handler mid = %d\n",
                           me_, data_request_mid_);
        }
    }
}

void
MIDMemoryGrp::deactivate()
{
  // active_ might be false because we've already begun sync_act(0)
  // on other nodes.
  if (active_ || nsync_) {
      if (debug_) ExEnv::out() << scprintf("%d: MIDMemoryGrp::deactivate()\n", me_);
      sync_act(0);
      if (active_) {
          ExEnv::err() << "MIDMemoryGrp::deactivate(): still active" << endl;
          abort();
        }
    }
}

void
MIDMemoryGrp::retrieve_data(void *data, int node, int offset, int size)
{
  n_ret_send_[node]++;

  MemoryDataRequestQueue q;

  long oldlock = lockcomm();

  MemoryDataRequest buf(MemoryDataRequest::Retrieve,
                         me(), offset, size);
  if (debug_) print_memreq(buf, "retrieve:", node);
  int mid = send(buf.data(), buf.nbytes(), node, data_request_type_);
  do_wait("retrieve: send req", mid, q, buf.nbytes(), node);

  mid = recv(data, size, node, data_type_from_handler_);
  do_wait("retrieve: recv dat", mid, q, size, node);

  flush_queue(q);

  unlockcomm(oldlock);
}

void
MIDMemoryGrp::replace_data(void *data, int node, int offset, int size)
{
  n_rep_send_[node]++;

  MemoryDataRequestQueue q;

  long oldlock = lockcomm();

  MemoryDataRequest buf(MemoryDataRequest::Replace,
                        me(), offset, size);
  if (debug_) print_memreq(buf, "replace:", node);
  int mid = send(buf.data(), buf.nbytes(), node, data_request_type_);
  do_wait("replace: send req", mid, q, buf.nbytes(), node);

  int junk;
  mid = recv(&junk, sizeof(junk), node, data_type_from_handler_);
  do_wait("replace: recv go-ahead", mid, q, sizeof(junk), node);
  if (debug_)
      ExEnv::out() << scprintf("%d: replace: got go-ahead %d:%d\n",
                       me_,junk&0xff,junk>>8);

  mid = send(data, size, node, data_type_to_handler_);
  do_wait("replace: send dat", mid, q, size, node);

  if (use_acknowledgments_) {
      mid = recv(&junk, sizeof(junk), node, data_type_from_handler_);
      do_wait("replace: recv ack", mid, q, sizeof(junk), node);
      if (debug_)
          ExEnv::out() << scprintf("%d: replace: got ack %d:%d\n",me_,junk&0xff,junk>>8);
    }

  flush_queue(q);

  unlockcomm(oldlock);
}

void
MIDMemoryGrp::sum_data(double *data, int node, int offset, int size)
{
  n_sum_send_[node]++;

  MemoryDataRequestQueue q;

  long oldlock = lockcomm();

  MemoryDataRequest buf(MemoryDataRequest::DoubleSum,
                        me(), offset, size);
  if (debug_) print_memreq(buf, "sum:", node);
  int mid = send(buf.data(), buf.nbytes(), node, data_request_type_);
  do_wait("sum: send req", mid, q, buf.nbytes(), node);

  int junk;
  recv(&junk, sizeof(junk), node, data_type_from_handler_);
  do_wait("sum: recv go-ahead", mid, q, sizeof(junk), node);
  if (debug_)
      ExEnv::out() << scprintf("%d: sum: got go-ahead %d:%d\n",me_,junk&0xff,junk>>8);

  int remain = size;
  int dremain = size/sizeof(double);
  int dataoffset = 0;
  while (remain > 0) {
      int dchunksize = dbufsize;
      if (dremain < dchunksize) dchunksize = dremain;
      int chunksize = dchunksize*sizeof(double);
      mid = send(&data[dataoffset], chunksize, node, data_type_to_handler_);
      do_wait("sum: send dat", mid, q, size, node);
      dremain -= dchunksize;
      remain -= chunksize;
      dataoffset += dchunksize;
    }

  if (use_acknowledgments_) {
      recv(&junk, sizeof(junk), node, data_type_from_handler_);
      do_wait("sum: recv ack", mid, q, sizeof(junk), node);
      if (debug_)
          ExEnv::out() << scprintf("%d: sum: got ack %d:%d\n",me_, junk&0xff, junk>>8);
    }

  flush_queue(q);

  unlockcomm(oldlock);
}

void
MIDMemoryGrp::sync()
{
  sync_act(1);
}

void
MIDMemoryGrp::sync_act(int reactivate)
{
  int i;
  int mid;

  long oldlock = lockcomm();

  if (debug_) {
      ExEnv::out() << scprintf(
          "%d: MIDMemoryGrp::sync() entered, active = %d, reactivate = %d",
          me_, active_, reactivate)
           << endl;
    }

  if (me() == 0) {
      // keep processing requests until all nodes have sent a
      // sync request
      if (debug_) {
          ExEnv::out() << scprintf("%d: outside while loop nsync_ = %d, n() = %d",
                           me_, nsync_, n())
               << endl;
        }
      while (nsync_ < n() - 1) {
          if (debug_) {
              ExEnv::out() << scprintf(
                  "%d: waiting for sync or other msg data_req_mid = %d",
                  me_, data_request_mid_)
                   << endl;
            }
          if (!active_) {
              ExEnv::err() << me_ << ": MIDMemoryGrp::sync(): not active (1)" << endl;
            }
          mid = wait(data_request_mid_);
          if (debug_) {
              ExEnv::out() << scprintf("%d: got mid = %d (data_request_mid_ = %d)",
                     me_, mid, data_request_mid_)
                   << endl;
            }
          if (mid == data_request_mid_) {
              got_data_request_mid();
              handler();
              if (nsync_ < n() - 1 || reactivate) activate();
            }
          else {
              ExEnv::err() << scprintf(
                  "%d: WARNING: MIDMemoryGrp::sync(1): stray message id = %d",
                  me_, mid) << endl;
            }
        }
      nsync_ = 0;

      if (debug_)
          ExEnv::out() << scprintf("%d: notifying nodes that sync is complete", me_)
               << endl;
      // tell all nodes that they can proceed
      MemoryDataRequest buf(MemoryDataRequest::Sync, me(), reactivate);
      for (i=1; i<n(); i++) {
          if (debug_) print_memreq(buf, "sync:", i);
          mid = send(buf.data(), buf.nbytes(), i, data_request_type_);
          if (debug_) 
              ExEnv::out() << scprintf("%d: node %d can proceed (mid = %d)",
                               me_,i,mid)
                   << endl;
          mid = wait(mid);
          if (debug_) 
              ExEnv::out() << scprintf("%d: node %d got proceed message",
                               me_, i, mid)
                   << endl;
        }
    }
  else {
      // let node 0 know that i'm done
      MemoryDataRequest buf(MemoryDataRequest::Sync, me(), reactivate);
      if (debug_) print_memreq(buf, "sync:", 0);
      mid = send(buf.data(), buf.nbytes(), 0, data_request_type_);
      if (debug_) 
          ExEnv::out() << scprintf("%d: sending sync (mid = %d)", me_, mid)
               << endl;
      int tmpmid;
      do {
          tmpmid = wait(mid, data_request_mid_);
          if (tmpmid == data_request_mid_) {
              if (debug_) 
                  ExEnv::out() << scprintf("%d: sync: wait: handling request %d-%d",
                                   me_, data_request_buffer_.node(),
                                   data_request_buffer_.serial_number())
                       << endl;
              got_data_request_mid();
              handler();
              if (reactivate || !nsync_) activate();
            }
          else if (tmpmid != mid) {
              ExEnv::err() << scprintf(
                  "%d: MIDMemoryGrp::sync(2): stray message id = %d",
                  me_, tmpmid)
                   << endl;
              sleep(1);
              abort();
            }
        } while (tmpmid != mid);
      // watch for the done message from 0 or request messages
      while (!nsync_) {
          if (debug_)
              ExEnv::out() << scprintf("%d: waiting for sync", me_) << endl;
          if (!active_) {
              ExEnv::err() << me_ << ": MIDMemoryGrp::sync(): not active (3)" << endl;
            }
          mid = wait(data_request_mid_);
          if (debug_) 
              ExEnv::out() << scprintf("%d: in sync got mid = %d", me_, mid) << endl;
          if (mid == data_request_mid_) {
              got_data_request_mid();
              handler();
              if (reactivate || !nsync_) activate();
            }
          else {
              ExEnv::err() << scprintf(
                  "%d: WARNING: MIDMemoryGrp::sync(3): stray message"
                  "id = %d", me_, mid) << endl;
            }
        }
      nsync_ = 0;
    }

  if (debug_)
      ExEnv::out() << scprintf("%d: MIDMemoryGrp::sync() done", me_) << endl;

  // after a sync, the sums must agree
  msg_->sum(n_sum_send_, n_);
  msg_->sum(n_ret_send_, n_);
  msg_->sum(n_rep_send_, n_);
  int doabort = 0;
  if (n_sum_send_[me_] != n_sum_recv_
      || n_ret_send_[me_] != n_ret_recv_
      || n_rep_send_[me_] != n_rep_recv_) {
      ExEnv::out() << scprintf("ERROR: %d: sum:s=%d r=%d ret:s=%d r=%d rep:s=%d r=%d",
                       me_,
                       n_sum_send_[me_], n_sum_recv_,
                       n_ret_send_[me_], n_ret_recv_,
                       n_rep_send_[me_], n_rep_recv_) << endl;
      doabort = 1;
    }
  msg_->max(doabort);
  if (doabort) {
      ExEnv::out() << node0 << "MIDMemoryGrp: counts do not not agree" << endl;
      abort();
    }

  // reset the counters
  memset(n_sum_send_, 0, n_*sizeof(int));
  memset(n_ret_send_, 0, n_*sizeof(int));
  memset(n_rep_send_, 0, n_*sizeof(int));
  n_sum_recv_ = 0;
  n_ret_recv_ = 0;
  n_rep_recv_ = 0;

  unlockcomm(oldlock);
}

void
MIDMemoryGrp::do_wait(const char *msg, int mid,
                      MemoryDataRequestQueue &q, size_t expectedsize,
                      int node)
{
  long oldlock = lockcomm();

  int tmpmid;
  do {
      if (debug_) 
          ExEnv::out() << scprintf("%d: %s: wait: waiting\n", me_, msg);
      if (!active_) {
          ExEnv::err() << me_ << ": MIDMemoryGrp::do_wait(): not active" << endl;
        }
      tmpmid = wait(data_request_mid_, mid);
      if (tmpmid == data_request_mid_) {
          got_data_request_mid();
          // Arbitrarily order the nodes to prevent deadlock.
          // Node 0 always executes the else block so it
          // can skip reactivation.
          if (me() > data_request_buffer_.node()) {
              if (data_request_buffer_.request() == MemoryDataRequest::Sync) {
                  ExEnv::err() << me() << ": MIDMessageGrp: "
                       << "how did I get a Sync in do_wait???"
                       << endl;
                  abort();
                }
              if (debug_) 
                  ExEnv::out() << scprintf("%d: %s: wait: queueing request %d-%d\n",
                                   me_, msg,
                                   data_request_buffer_.node(),
                                   data_request_buffer_.serial_number());
              q.push(data_request_buffer_);
              if (q.n() == MemoryDataRequestQueue::MaxDepth/2) {
                  ExEnv::out() << me_ << ": " << msg
                       << " wait: queue half full while waiting for node "
                       << node << endl;
                }
              activate();
            }
          else {
              if (debug_) 
                  ExEnv::out() << scprintf("%d: %s: wait: handling request %d-%d\n",
                                   me_, msg,
                                   data_request_buffer_.node(),
                                   data_request_buffer_.serial_number());
              handler();
              if (data_request_buffer_.request() == MemoryDataRequest::Sync) {
                  if (data_request_buffer_.reactivate()
                      || (me() == 0 && nsync_ < n() - 1)) activate();
                }
              else {
                  activate();
                }
            }
        }
      else if (tmpmid != mid) {
          ExEnv::err() << scprintf("%d: MIDMemoryGrp: %s: stray message id = %d\n",
                           me_, msg, tmpmid);
          sleep(1);
          abort();
        }
      else {
          if (debug_)
              ExEnv::out() << scprintf("%d: %s: wait: wait complete\n", me_, msg);
        }
    } while (tmpmid != mid);

  unlockcomm(oldlock);
}

void
MIDMemoryGrp::flush_queue(MemoryDataRequestQueue &q)
{
  long oldlock = lockcomm();

  for (int i=0; i<q.n(); i++) {
      if (debug_) 
          ExEnv::out() << scprintf("%d: processing queued request %d-%d\n",
                           me_,
                           data_request_buffer_.node(),
                           data_request_buffer_.serial_number());
      handler(q[i]);
    }
  q.clear();

  unlockcomm(oldlock);
}
void
MIDMemoryGrp::got_data_request_mid()
{
  data_request_mid_ = -1;
  active_ = 0;
}

void
MIDMemoryGrp::catchup()
{
  while (!use_active_messages_ && probe(data_request_mid_)) {
      got_data_request_mid();
      if (debug_)
          ExEnv::out()
              << scprintf("%d: catchup: handling request %d-%d",
                          me_,
                          data_request_buffer_.node(),
                          data_request_buffer_.serial_number())
              << endl;
      handler();
      activate();
    }
}

int
MIDMemoryGrp::probe(long)
{
    ExEnv::err() << "MIDMemoryGrp:: probe not implemeneted for " << class_name()
         << endl;
    abort();
    return 0;
}

#endif

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
