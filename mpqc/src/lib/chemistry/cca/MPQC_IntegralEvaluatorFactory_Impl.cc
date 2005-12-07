// 
// File:          MPQC_IntegralEvaluatorFactory_Impl.cc
// Symbol:        MPQC.IntegralEvaluatorFactory-v0.2
// Symbol Type:   class
// Babel Version: 0.10.2
// Description:   Server-side implementation for MPQC.IntegralEvaluatorFactory
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.2
// 
#include "MPQC_IntegralEvaluatorFactory_Impl.hh"

// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._includes)
using namespace std;
using namespace sc;
using namespace Chemistry::QC;
string get_Integral_keyval(string);
#include <iostream>
#include <chemistry/qc/intv3/intv3.h>
#ifdef HAVE_CINTS
  #include <chemistry/qc/cints/cints.h>
#endif
#include "MPQC_IntegralEvaluator2.hh"
#include "MPQC_IntegralEvaluator3.hh"
#include "MPQC_IntegralEvaluator4.hh"
sc::Ref<sc::GaussianBasisSet> basis_cca_to_sc(GaussianBasis::Molecular&);
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._includes)

// user-defined constructor.
void MPQC::IntegralEvaluatorFactory_impl::_ctor() {
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._ctor)
  // add construction details here
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._ctor)
}

// user-defined destructor.
void MPQC::IntegralEvaluatorFactory_impl::_dtor() {
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._dtor)
  // add destruction details here
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._dtor)
}

// static class initializer.
void MPQC::IntegralEvaluatorFactory_impl::_load() {
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._load)
  // guaranteed to be called at most once before any other method in this class
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._load)
}

// user-defined static methods: (none)

// user-defined non-static methods:
/**
 * Starts up a component presence in the calling framework.
 * @param Svc the component instance's handle on the framework world.
 * Contracts concerning Svc and setServices:
 * 
 * The component interaction with the CCA framework
 * and Ports begins on the call to setServices by the framework.
 * 
 * This function is called exactly once for each instance created
 * by the framework.
 * 
 * The argument Svc will never be nil/null.
 * 
 * Those uses ports which are automatically connected by the framework
 * (so-called service-ports) may be obtained via getPort during
 * setServices.
 */
void
MPQC::IntegralEvaluatorFactory_impl::setServices (
  /* in */ ::gov::cca::Services services ) 
throw ( 
  ::gov::cca::CCAException
){
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.setServices)
  
  services_ = services;
  if (services_._is_nil()) return;

  try {
      services_.addProvidesPort(self, "IntegralEvaluatorFactory",
                       "Chemistry.QC.GaussianBasis.IntegralEvaluatorFactory", 0);
  }
  catch (gov::cca::CCAException e) {
    std::cout << "Error using services: "
	      << e.getNote() << std::endl;
  }

  //setup parameters
  try {

    if (services_._not_nil()) {
      gov::cca::TypeMap tm = services_.createTypeMap();
      services_.registerUsesPort("classicParam",
                                 "gov.cca.ParameterPortFactoryService",tm);
      gov::cca::Port p = services_.getPort("classicParam");
      ccaffeine::ports::PortTranslator portX = p;
      if(portX._not_nil()) {
        classic::gov::cca::Port *cp
          =static_cast<classic::gov::cca::Port*>(portX.getClassicPort());
        if(!cp) {
          std::cout << "Couldn't get classic port" << std::endl;
          return;
        }
        ConfigurableParameterFactory *cpf
          = dynamic_cast<ConfigurableParameterFactory *>(cp);
        ConfigurableParameterPort *pp = setup_parameters(cpf);
        classic::gov::cca::Port *clscp
          = dynamic_cast<classic::gov::cca::Port*>(pp);
        if (!clscp) {
          std::cout << "Couldn't cast to classic::gov::cca::Port"
                    << std::endl;
        }
        void *vp = static_cast<void*>(clscp);
        ccaffeine::ports::PortTranslator provideX
          = ccaffeine::ports::PortTranslator::createFromClassic(vp);

        services_.addProvidesPort(provideX,
                                  "configure", "ParameterPort", tm);

        services_.releasePort("classicParam");
        services_.unregisterUsesPort("classicParam");
      }
    }
  }
  catch(std::exception& e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }
  
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.setServices)
}

/**
 * Set the EvaluatorConfig
 * @param config EvaluatorConfig 
 */
void
MPQC::IntegralEvaluatorFactory_impl::set_config (
  /* in */ ::Chemistry::QC::GaussianBasis::EvaluatorConfig config ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.set_config)
  eval_config_ = config;
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.set_config)
}

/**
 * Set the molecular basis 
 * @param molbasis The molecular basis 
 */
void
MPQC::IntegralEvaluatorFactory_impl::set_molecular (
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular molbasis ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.set_molecular)
  // insert implementation here
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.set_molecular)
}

/**
 * Get the molecular basis
 * @return The molecular basis 
 */
::Chemistry::QC::GaussianBasis::Molecular
MPQC::IntegralEvaluatorFactory_impl::get_molecular ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_molecular)
  // insert implementation here
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_molecular)
}

/**
 * Set the molecule
 * @param The molecule 
 */
void
MPQC::IntegralEvaluatorFactory_impl::set_molecule (
  /* in */ ::Chemistry::Molecule mol ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.set_molecule)
  molecule_ = mol;
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.set_molecule)
}

/**
 * Get the molecule
 * @return The molecule 
 */
::Chemistry::Molecule
MPQC::IntegralEvaluatorFactory_impl::get_molecule ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_molecule)
  return molecule_;
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_molecule)
}

/**
 * Set the integral package
 * @param The integral package 
 */
void
MPQC::IntegralEvaluatorFactory_impl::set_integral_package (
  /* in */ const ::std::string& label ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.set_integral_package)
  package_ = label;
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.set_integral_package)
}

/**
 * Set available storage
 * @param storage Available storage in bytes 
 */
void
MPQC::IntegralEvaluatorFactory_impl::set_storage (
  /* in */ int64_t storage ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.set_storage)
  storage_ = storage;
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.set_storage)
}

/**
 * Get a 2-center integral evaluator
 * @param label String specifying integral type
 * @param max_deriv Maximum derivative that will be computed
 * @param bs1 Molecular basis set on center 1
 * @param bs2 Molecular basis set on center 2
 * @return 2-center integral evaluator 
 */
::Chemistry::QC::GaussianBasis::IntegralEvaluator2
MPQC::IntegralEvaluatorFactory_impl::get_integral_evaluator2 (
  /* in */ const ::std::string& label,
  /* in */ int64_t max_deriv,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
  /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters deriv_ctr ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_integral_evaluator2)

  // create the evaluator
  MPQC::IntegralEvaluator2 eval = MPQC::IntegralEvaluator2::_create();
 
  // determine proper integrals package
  bool pkg_set = false;
  for( int i=0; i<eval_config_.get_n_pkg_config(); ++i)
    if( eval_config_.get_pkg_config_type(i) == label ) {
      eval.set_integral_package( eval_config_.get_pkg_config_pkg(i) );
      pkg_set = true;
    }
  if( !pkg_set ) {
    package_ =  package_param_->getValueString();
    if( package_ == "intv3" || package_ == "cints" )
      eval.set_integral_package( package_ );
    else {
      package_ = eval_config_.get_default_pkg();
      if( package_ == "intv3" || package_ == "cints" )
        eval.set_integral_package( package_ );
      else
        eval.set_integral_package( "intv3" );
    }
  }
  
  // initialize
  eval.initialize( bs1, bs2, label, max_deriv, storage_, deriv_ctr );
  return eval;

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_integral_evaluator2)
}

/**
 * Get a nuclear repulsion specialized  2-center integral 
 * evaluator.  Returns derivative integrals taken with 
 * respect to DerivCenters.
 * @param max_deriv Maximum derivative that will be computed
 * @param bs1 Molecular basis set on center 1
 * @param bs2 Molecular basis set on center 2
 * @return nuclear repulsion integral evaluator 
 */
::Chemistry::QC::GaussianBasis::IntegralEvaluator2
MPQC::IntegralEvaluatorFactory_impl::get_nuclear_evaluator (
  /* in */ int64_t max_deriv,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
  /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters dc ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_nuclear_evaluator)
  // Insert-Code-Here {MPQC.IntegralEvaluatorFactory.get_nuclear_evaluator} (get_nuclear_evaluator method)
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_nuclear_evaluator)
}

/**
 * Get a hcore specialized  2-center integral
 * evaluator.  Returns derivative integrals taken with
 * respect to DerivCenters.
 * @param max_deriv Maximum derivative that will be computed
 * @param bs1 Molecular basis set on center 1
 * @param bs2 Molecular basis set on center 2
 * @return hcore repulsion integral evaluator 
 */
::Chemistry::QC::GaussianBasis::IntegralEvaluator2
MPQC::IntegralEvaluatorFactory_impl::get_hcore_evaluator (
  /* in */ int64_t max_deriv,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
  /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters dc ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_hcore_evaluator)
  // Insert-Code-Here {MPQC.IntegralEvaluatorFactory.get_hcore_evaluator} (get_hcore_evaluator method)
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_hcore_evaluator)
}

/**
 * Get a 3-center integral evaluator
 * @param label String specifying integral type
 * @param max_deriv Maximum derivative that will be computed
 * @param bs1 Molecular basis set on center 1
 * @param bs2 Molecular basis set on center 2
 * @param bs3 Molecular basis set on center 3
 * @return 3-center integral evaluator 
 */
::Chemistry::QC::GaussianBasis::IntegralEvaluator3
MPQC::IntegralEvaluatorFactory_impl::get_integral_evaluator3 (
  /* in */ const ::std::string& label,
  /* in */ int64_t max_deriv,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs3,
  /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters deriv_ctr ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_integral_evaluator3)

  // create the evaluator
  MPQC::IntegralEvaluator3 eval = MPQC::IntegralEvaluator3::_create();

 // determine proper integrals package
  bool pkg_set = false;
  for( int i=0; i<eval_config_.get_n_pkg_config(); ++i)
    if( eval_config_.get_pkg_config_type(i) == label ) {
      eval.set_integral_package( eval_config_.get_pkg_config_pkg(i) );
      pkg_set = true;
    }
  if( !pkg_set ) {
    package_ =  package_param_->getValueString();
    if( package_ == "intv3" || package_ == "cints" )
      eval.set_integral_package( package_ );
    else {
      package_ = eval_config_.get_default_pkg();
      if( package_ == "intv3" || package_ == "cints" )
        eval.set_integral_package( package_ );
      else
        eval.set_integral_package( "intv3" );
    }
  }

  // initialize
  eval.initialize( bs1, bs2, bs3, label, max_deriv, storage_, deriv_ctr );
  return eval;

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_integral_evaluator3)
}

/**
 * Get a 4-center integral evaluator
 * @param label String defining integral type
 * @param max_deriv Maximum derivative that will be computed
 * @param bs1 Molecular basis set on center 1
 * @param bs2 Molecular basis set on center 2
 * @param bs3 Molecular basis set on center 3
 * @param bs4 Molecular basis set on center 4
 * @return 4-center integral evaluator 
 */
::Chemistry::QC::GaussianBasis::IntegralEvaluator4
MPQC::IntegralEvaluatorFactory_impl::get_integral_evaluator4 (
  /* in */ const ::std::string& label,
  /* in */ int64_t max_deriv,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs3,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs4,
  /* in */ ::Chemistry::QC::GaussianBasis::DerivCenters deriv_ctr ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory.get_integral_evaluator4)

  // create the evaluator
  MPQC::IntegralEvaluator4 eval = MPQC::IntegralEvaluator4::_create();

 // determine proper integrals package
  bool pkg_set = false;
  for( int i=0; i<eval_config_.get_n_pkg_config(); ++i)
    if( eval_config_.get_pkg_config_type(i) == label ) {
      eval.set_integral_package( eval_config_.get_pkg_config_pkg(i) );
      pkg_set = true;
    }
  if( !pkg_set ) {
    package_ =  package_param_->getValueString();
    if( package_ == "intv3" || package_ == "cints" )
      eval.set_integral_package( package_ );
    else {
      package_ = eval_config_.get_default_pkg();
      if( package_ == "intv3" || package_ == "cints" )
        eval.set_integral_package( package_ );
      else
        eval.set_integral_package( "intv3" );
    }
  }

  // initialize
  eval.initialize( bs1, bs2, bs3, bs4, label, max_deriv, storage_, deriv_ctr );
  return eval;

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory.get_integral_evaluator4)
}


// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._misc)

ConfigurableParameterPort *
MPQC::IntegralEvaluatorFactory_impl::setup_parameters(
                                              ConfigurableParameterFactory *cpf)
{
  ConfigurableParameterPort * pp = cpf->createConfigurableParameterPort();

  pp->setBatchTitle("PortTranslatorStarter Configuration");
  pp->setGroupName("Model Factory Input");

  package_param_ = new StringParameter("package", "Integral package",
                                      "package", "default");
  pp->addRequest(package_param_);

  return pp;
}

// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._misc)

