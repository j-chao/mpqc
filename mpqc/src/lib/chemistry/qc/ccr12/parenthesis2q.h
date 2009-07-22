// hand written code by Toru Shiozaki
#ifndef _chemistry_qc_ccr12_ccsd_parenthesis2q_h
#define _chemistry_qc_ccr12_ccsd_parenthesis2q_h

#ifdef __GNUC__
#pragma interface
#endif

#include <chemistry/qc/ccr12/ccr12_info.h>
#include <chemistry/qc/ccr12/parenthesis2tnum.h>

namespace sc {

class Parenthesis2q : virtual public RefCount {

  protected:
   CCR12_Info* z;

  public:
   Parenthesis2q(CCR12_Info* );
   ~Parenthesis2q();
   
   double compute(Parenthesis2tNum*, Parenthesis2tNum*);

};



}

#endif


