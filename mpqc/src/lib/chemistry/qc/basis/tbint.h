
#ifndef _chemistry_qc_basis_tbint_h
#define _chemistry_qc_basis_tbint_h

#ifdef __GNUC__
#pragma interface
#endif

#include <util/ref/ref.h>
#include <util/group/message.h>
#include <chemistry/qc/basis/gaussbas.h>
#include <chemistry/qc/basis/dercent.h>

////////////////////////////////////////////////////////////////////////////

class TwoBodyInt : public VRefCount {
  protected:
    RefGaussianBasisSet bs1_;
    RefGaussianBasisSet bs2_;
    RefGaussianBasisSet bs3_;
    RefGaussianBasisSet bs4_;

    double *buffer_;

    int redundant_;
    
    TwoBodyInt(const RefGaussianBasisSet&bs1,
               const RefGaussianBasisSet&bs2,
               const RefGaussianBasisSet&bs3,
               const RefGaussianBasisSet&bs4);
  public:
    virtual ~TwoBodyInt();
  
    int nbasis() const;
    int nbasis1() const;
    int nbasis2() const;
    int nbasis3() const;
    int nbasis4() const;

    int nshell() const;
    int nshell1() const;
    int nshell2() const;
    int nshell3() const;
    int nshell4() const;

    RefGaussianBasisSet basis();
    RefGaussianBasisSet basis1();
    RefGaussianBasisSet basis2();
    RefGaussianBasisSet basis3();
    RefGaussianBasisSet basis4();

    const double * buffer() const;
    
    virtual void compute_shell(int,int,int,int) = 0;

    // an index of -1 for any shell indicates any shell
    virtual int log2_shell_bound(int,int,int,int) = 0;

    // if redundant is true, then keep redundant integrals in buffer_.  The
    // default is true.
    int redundant() const { return redundant_; }
    void set_redundant(int i) { redundant_ = i; }
};

REF_dec(TwoBodyInt);

////////////////////////////////////////////////////////////////////////////

class ShellQuartetIter {
  protected:
    const double * buf;
    double scale_;

    int redund_;
    
    int e12;
    int e34;
    int e13e24;

    int index;
    
    int istart;
    int jstart;
    int kstart;
    int lstart;

    int iend;
    int jend;
    int kend;
    int lend;

    int icur;
    int jcur;
    int kcur;
    int lcur;

    int i_;
    int j_;
    int k_;
    int l_;
    
  public:
    ShellQuartetIter();
    virtual ~ShellQuartetIter();

    virtual void init(const double *,
                      int, int, int, int,
                      int, int, int, int,
                      int, int, int, int,
                      double, int);

    virtual void start();
    virtual void next();

    int ready() const { return icur < iend; }

    int i() const { return i_; }
    int j() const { return j_; }
    int k() const { return k_; }
    int l() const { return l_; }

    int nint() const { return iend*jend*kend*lend; }
    
    double val() const { return buf[index]*scale_; }
};

class TwoBodyIntIter {
  protected:
    RefTwoBodyInt tbi;
    ShellQuartetIter sqi;
    
    int iend;
    
    int icur;
    int jcur;
    int kcur;
    int lcur;
    
  public:
    TwoBodyIntIter();
    TwoBodyIntIter(const RefTwoBodyInt&);

    virtual ~TwoBodyIntIter();
    
    virtual void start();
    virtual void next();

    int ready() const { return (icur < iend); }

    int ishell() const { return icur; }
    int jshell() const { return jcur; }
    int kshell() const { return kcur; }
    int lshell() const { return lcur; }

    virtual double scale() const;

    ShellQuartetIter& current_quartet();
};

////////////////////////////////////////////////////////////////////////////

class TwoBodyDerivInt : public VRefCount {
  protected:
    RefGaussianBasisSet bs1_;
    RefGaussianBasisSet bs2_;
    RefGaussianBasisSet bs3_;
    RefGaussianBasisSet bs4_;

    double *buffer_;

    TwoBodyDerivInt(const RefGaussianBasisSet&b1,
                    const RefGaussianBasisSet&b2,
                    const RefGaussianBasisSet&b3,
                    const RefGaussianBasisSet&b4);
  public:
    virtual ~TwoBodyDerivInt();
  
    int nbasis() const;
    int nbasis1() const;
    int nbasis2() const;
    int nbasis3() const;
    int nbasis4() const;

    int nshell() const;
    int nshell1() const;
    int nshell2() const;
    int nshell3() const;
    int nshell4() const;

    RefGaussianBasisSet basis();
    RefGaussianBasisSet basis1();
    RefGaussianBasisSet basis2();
    RefGaussianBasisSet basis3();
    RefGaussianBasisSet basis4();

    const double * buffer() const;
    
    virtual void compute_shell(int,int,int,int,DerivCenters&) = 0;

    // an index of -1 for any shell indicates any shell
    virtual int log2_shell_bound(int,int,int,int) = 0;
};

REF_dec(TwoBodyDerivInt);

#endif
