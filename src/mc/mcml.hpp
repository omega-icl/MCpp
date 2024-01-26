#ifndef MC__MCML_HPP
#define MC__MCML_HPP

//#define MC__MCANN_DEBUG
#define MC__MCANN_CHECK

#include "mccormick.hpp"
#include "ismodel.hpp"
#include "asmodel.hpp"
#include "mcfunc.hpp"
#include "ffunc.hpp"
#include "polimage.hpp"
#include "slift.hpp"

namespace mc
{

//! @brief C++ class for evaluation and relaxation of neural networks
////////////////////////////////////////////////////////////////////////
//! mc::ANN is a C++ class for evaluation and relaxation of neural
//! networks leveraging expression trees and arithmetics available
//! through MC++
////////////////////////////////////////////////////////////////////////
template <typename T> 
class ANN
////////////////////////////////////////////////////////////////////////
{
public:

  //! @brief Number of inputs
  size_t                               nin;
  //! @brief Number of outputs
  size_t                               nout;
  //! @brief ANN data
  std::vector<std::vector<std::vector<double>>> data;

  //! @brief Storage for Polhedral relaxation
  std::pair<FFGraph<>*,void*>          DAG;
  FFSubgraph                           DAGOps;
  std::vector<FFVar>                   DAGVar;
  std::vector<FFVar>                   DAGRes;
  PolImg<T>*                           POLEnv;
  std::vector<PolVar<T>>               POLVar;
  std::vector<PolVar<T>>               POLRes;
  std::map<PolVar<T> const*,PolVar<T>,lt_PolVar<T>> POLMap;

  //! @brief Storage for Interval bounds
  std::vector<T>                       IVar;
  std::vector<T>                       IRes;

  //! @brief Storage for McCormick relaxation
  std::vector<McCormick<T>>            MCVar;
  std::vector<McCormick<T>>            MCRes;

  //! @brief Storage for interval superposition models
  ISModel<T>*                          ISMEnv;
  std::vector<ISVar<T>>                ISMVar;
  std::vector<ISVar<T>>                ISMRes;
  std::vector<std::vector<PolVar<T>>>  POLISMAux;
  std::vector<double>                  DLISMAux;
  std::vector<double>                  DUISMAux;
  std::vector<McCormick<ISVar<T>>>     MCISMVar;
  std::vector<McCormick<ISVar<T>>>     MCISMRes;

  //! @brief Storage for affine superposition models
  ASModel<T>*                          ASMEnv;
  std::vector<ASVar<T>>                ASMVar;
  std::vector<ASVar<T>>                ASMRes;
  std::vector<PolVar<T>>               POLLASMAux;
  std::vector<PolVar<T>>               POLUASMAux;
  std::vector<double>                  DXASMAux;
  std::vector<double>                  DYASMAux;

  //! @brief ReLUANNOp options
  struct Options
  {
    //! @brief Activation type
    enum ACTIVTYPE{
      LINEAR=0, //!< Linear activation function
      RELU,     //!< ReLU activation function
      TANH,     //!< tanh activation function
      SIGMOID   //!< Sigmoid activation function
    };

    //! @brief Relaxation type
    enum RELAXTYPE{
      POL=0, //!< Polyhedral relaxation with auxiliary variables
      INT,   //!< Interval bounds
      MC,    //!< McCormick relaxation with interval bounds
      ISM,   //!< Interval superposition model
      MCISM, //!< McCormick relaxation with interval superposition bounds
      ASM    //!< Affine superposition model
    };

    //! @brief Default constructor
    Options():
      ACTIV(RELU), RELAX(POL), ISMDIV(64), ASMBPS(8), ISMCONT(true), ISMSLOPE(true), ISMSHADOW(true), CUTSHADOW(false), 
      ZEROTOL(machprec()), RELU2ABS(false), SIG2EXP(false)
      {}

    //! @brief Assignment operator
    Options& operator=
      ( Options const& opt ){
        ACTIV     = opt.ACTIV;
        RELAX     = opt.RELAX;
        ISMDIV    = opt.ISMDIV;
        ASMBPS    = opt.ASMBPS;
        ISMCONT   = opt.ISMCONT;
        ISMSLOPE  = opt.ISMSLOPE;
        ISMSHADOW = opt.ISMSHADOW;
        CUTSHADOW = opt.CUTSHADOW;
        ZEROTOL   = opt.ZEROTOL;
        RELU2ABS  = opt.RELU2ABS;
        SIG2EXP   = opt.SIG2EXP;
        return *this;
      }

    //! @brief Type of activation function
    ACTIVTYPE ACTIV;
    //! @brief Type of relaxation
    RELAXTYPE RELAX;
    //! @brief Number of subdivisions in superposition model
    unsigned ISMDIV;
    //! @brief Number of ??? in superposition model   
    unsigned ASMBPS;
    //! @brief Whether to construct continuous or binary relaxation of superposition model
    bool     ISMCONT;
    //! @brief Whether to propagate slopes in superposition model
    bool     ISMSLOPE;
    //! @brief Whether to propagate shadow remainders in superposition model
    bool     ISMSHADOW;
    //! @brief Whether to append cuts from shadow remainders in superposition model relaxation
    bool     CUTSHADOW;
    //! @brief Threshold for zero coefficient in neural network
    double   ZEROTOL;
    //! @brief Whether to convert ReLU to abs (true) or max (false) function
    bool     RELU2ABS;
    //! @brief Whether to convert sigmoid to exp (true) or tanh (false)
    bool     SIG2EXP;
  } options;

  //! @brief Default constructor
  ANN
  ()
  : nin(0), nout(0), DAG(nullptr,nullptr), POLEnv(nullptr), ISMEnv(nullptr), ASMEnv(nullptr)
  {}

  ~ANN() 
  {
    delete POLEnv;
    delete DAG.first;
    delete ISMEnv;
    delete ASMEnv;
  }

  //! @brief Set neural network
  void set
    ( std::vector<std::vector<std::vector<double>>> const& data );

  //! @brief Resize relaxation containers
  void resize
    ();

  //! @brief Evaluate neural network
  template <typename U>
  void evaluate
    ( U* y, U const* x )
    const
    { static thread_local std::vector<std::vector<U>> Uhid;
      evaluate( y, x, Uhid ); }
  void evaluate
    ( double* y, double const* x )
    { evaluate( y, x, Dhid ); }
  void evaluate
    ( fadbad::F<double>* y, fadbad::F<double> const* x )
    { evaluate( y, x, FDhid ); }
  void evaluate
    ( T* y, T const* x )
    { evaluate( y, x, Ihid ); }
  void evaluate
    ( McCormick<T>* y, McCormick<T> const* x )
    { evaluate( y, x, MCIhid ); }
  void evaluate
    ( ISVar<T>* y, ISVar<T> const* x )
    { evaluate( y, x, ISMhid ); }
  void evaluate
    ( McCormick<ISVar<T>>* y, McCormick<ISVar<T>> const* x )
    { evaluate( y, x, MCISMhid ); }
  void evaluate
    ( ASVar<T>* y, ASVar<T> const* x )
    { evaluate( y, x, ASMhid ); }
  void evaluate
    ( PolVar<T>* y, PolVar<T> const* x )
    { evaluate( y, x, POLhid ); }

  //! @brief Propagate polyhedral image through neural network
  void propagate
    ( PolBase<T>* img, FFVar** pRes, PolVar<T>* vRes, PolVar<T> const* vVar );

  //! @brief Append polyhedral image cuts for neural network
  void back_propagate
    ( PolBase<T>* img, FFOp* pOp, PolVar<T> const* vRes, PolVar<T>* vVar );

private:

  template <typename U>
  void evaluate
    ( U* y, U const* x, std::vector<std::vector<U>>& vhid )
    const;

  void append_ASMcuts
    ( PolBase<T>* img, FFOp* pop, PolVar<T> const& vRes, PolVar<T>* vVar,
      std::vector<UnivarPWL<T>> const& pwlEst );

  void append_ASMcuts
    ( PolBase<T>* img, FFOp* pOp, PolVar<T> const& vRes, PolVar<T>* vVar,
      double const& rhsEst, std::vector<double> const& lnrEst=std::vector<double>() );
  
  template <typename U>
  U ReLU
    ( U const& x )
    const
    {
      return options.RELU2ABS?
             ( x + Op<U>::fabs(x) ) * 0.5:
             Op<U>::max( x, U(0.) );
    }
    
  template <typename U>
  ISVar<U> ReLU
    ( ISVar<U> const& x )
    const
    {
      return relu( x );
    }

  template <typename U>
  ASVar<U> ReLU
    ( ASVar<U> const& x )
    const
    {
      return relu( x );
    }
    
  template <typename U>
  fadbad::F<U> ReLU
    ( fadbad::F<U> const& x )
    const
    {
      fadbad::F<U> z = ReLU( x.val() );
      z.setDepend( x );
      for( unsigned j=0; j<z.size(); ++j )
        z[j] = Op<U>::fstep( x.val() ) * x[j];
      return z;
    }

  //! @brief Intermediate storage for MLP evaluation
  static thread_local std::vector<std::vector<double>>              Dhid;
  static thread_local std::vector<std::vector<fadbad::F<double>>>   FDhid;
  static thread_local std::vector<std::vector<T>>                   Ihid;
  static thread_local std::vector<std::vector<McCormick<T>>>        MCIhid;
  static thread_local std::vector<std::vector<ISVar<T>>>            ISMhid;
  static thread_local std::vector<std::vector<McCormick<ISVar<T>>>> MCISMhid;
  static thread_local std::vector<std::vector<ASVar<T>>>            ASMhid;
  static thread_local std::vector<std::vector<PolVar<T>>>           POLhid;
};

template <typename T> inline thread_local std::vector<std::vector<double>>              ANN<T>::Dhid     = std::vector<std::vector<double>>();
template <typename T> inline thread_local std::vector<std::vector<fadbad::F<double>>>   ANN<T>::FDhid    = std::vector<std::vector<fadbad::F<double>>>();
template <typename T> inline thread_local std::vector<std::vector<T>>                   ANN<T>::Ihid     = std::vector<std::vector<T>>();
template <typename T> inline thread_local std::vector<std::vector<McCormick<T>>>        ANN<T>::MCIhid   = std::vector<std::vector<McCormick<T>>>();
template <typename T> inline thread_local std::vector<std::vector<ISVar<T>>>            ANN<T>::ISMhid   = std::vector<std::vector<ISVar<T>>>();
template <typename T> inline thread_local std::vector<std::vector<McCormick<ISVar<T>>>> ANN<T>::MCISMhid = std::vector<std::vector<McCormick<ISVar<T>>>>();
template <typename T> inline thread_local std::vector<std::vector<ASVar<T>>>            ANN<T>::ASMhid   = std::vector<std::vector<ASVar<T>>>();
template <typename T> inline thread_local std::vector<std::vector<PolVar<T>>>           ANN<T>::POLhid   = std::vector<std::vector<PolVar<T>>>();

template<typename T>
inline void
ANN<T>::set
( std::vector<std::vector<std::vector<double>>> const& data )
{
  // Set data
#ifdef MC__MCANN_CHECK
  assert( data.size() && data[0].size() && data[0][0].size() > 1 );
#endif
  nin  = data.front().front().size() - 1;
  nout = data.back().size();
  this->data = data;
}

template<typename T>
template<typename U>
inline void
ANN<T>::evaluate
( U* y, U const* x, std::vector<std::vector<U>>& vhid )
const
{
  // Propagate through hidden layers
  //std::vector<std::vector<U>> vhid;
  //static std::vector<std::vector<U>> vhid;
  //static thread_local std::vector<std::vector<U>> vhid;
  size_t const nhid = data.size()-1;
  vhid.resize( nhid );
#ifdef MC__MCANN_DEBUG
  std::cerr << "No hidden layers: " << nhid << std::endl;
#endif
  for( unsigned l=0; l<nhid; ++l ){
#ifdef MC__MCANN_CHECK
    assert( data[l].size() ); // number of neurons in layer l+1
#endif
    size_t const nneu = data[l].size();
    vhid[l].resize( nneu );
#ifdef MC__MCANN_DEBUG
    std::cerr << "No neurons in layer " << l << ": " << nneu << std::endl;
#endif
    for( unsigned i=0; i<nneu; ++i ){
      vhid[l][i] = data[l][i][0]; // bias term
#ifdef MC__MCANN_DEBUG
      std::cerr << "No inputs to neuron " << i << " in layer " << l << ": " << data[l][i].size()-1 << std::endl;
#endif
      for( unsigned j=0; j<data[l][i].size()-1; ++j ){
#ifdef MC__MCANN_DEBUG
        std::cout << "layer:" << l << " neuron:" << i << " input:" << j << std::endl;
#endif
        if( std::fabs(data[l][i][1+j]) < options.ZEROTOL ) continue;
        vhid[l][i] += (l? vhid[l-1][j]: x[j]) * data[l][i][1+j];
      }
      switch( options.ACTIV ){
        case Options::LINEAR:  default:                                                break;
        case Options::RELU:    vhid[l][i] = ReLU( vhid[l][i] );                        break;
        case Options::TANH:    vhid[l][i] = Op<U>::tanh( vhid[l][i] );                 break;
        case Options::SIGMOID: vhid[l][i] = options.SIG2EXP?
                                            1. / ( Op<U>::exp( -vhid[l][i] ) + 1. ):
                                            Op<U>::tanh( vhid[l][i]*0.5 ) * 0.5 + 0.5; break;
      }
    }
  }

  // Propagate through output layers
#ifdef MC__MCANN_CHECK
  assert( data.back().size() ); // number of neurons in layer l+1
#endif
  size_t const nneu = data.back().size();
#ifdef MC__MCANN_DEBUG
  std::cerr << "No neurons in layer " << nhid << ": " << nneu << std::endl;
#endif
  for( unsigned i=0; i<nneu; ++i ){
    y[i] = data.back()[i][0]; // bias term
#ifdef MC__MCANN_DEBUG
    std::cerr << "No inputs to neuron " << i << " in layer " << nhid << ": " << data.back()[i].size()-1 << std::endl;
#endif
    for( unsigned j=0; j<data.back()[i].size()-1; ++j ){
#ifdef MC__MCANN_DEBUG
      std::cout << "layer:" << nhid << " neuron:" << i << " input:" << j << std::endl;
#endif
      if( std::fabs(data.back()[i][1+j]) < options.ZEROTOL ) continue;
      y[i] += (nhid? vhid[nhid-1][j]: x[j]) * data.back()[i][1+j];
    }
  }
}

template<typename T>
inline void
ANN<T>::resize
()
{
  // Set relaxation environment and containers
  switch( options.RELAX ){
    case Options::POL:
    default:
      if( !DAG.first || DAG.second != &data ){
        delete DAG.first;
        DAG = std::make_pair( new FFGraph<>, &data );
        DAGVar.resize( nin );
        DAGRes.resize( nout );
        for( unsigned i=0; i<nin; ++i )
          DAGVar[i].set( DAG.first );
        evaluate( DAGRes.data(), DAGVar.data() );
        DAGOps = DAG.first->subgraph( nout, DAGRes.data() );
#ifdef MC__MCANN_DEBUG
        DAG.first->output( DAGOps, " ANN", std::cerr );
        std::ofstream oFile( "ANN.dot", std::ios_base::out );
        DAG.first->dot_script( nout, DAGRes.data(), oFile );
        oFile.close();
#endif
      }
      if( !POLEnv ) POLEnv = new PolImg<T>;
      POLVar.resize( nin );
      POLRes.resize( nout );
      break;

    case Options::INT:
      IVar.resize( nin );
      IRes.resize( nout );
      return;

    case Options::MC:
      MCVar.resize( nin );
      MCRes.resize( nout );
      return;

    case Options::MCISM:
      MCISMVar.resize( nin );
      MCISMRes.resize( nout );
      // no break

    case Options::ISM:
      if( !ISMEnv || ISMEnv->nvar() != nin || ISMEnv->ndiv() != options.ISMDIV ){
        delete ISMEnv;
        ISMEnv = new ISModel<T>( nin, options.ISMDIV );
      }
      ISMEnv->options.SLOPE_USE  = options.ISMSLOPE;  
      ISMEnv->options.SHADOW_USE = options.ISMSHADOW;
      ISMVar.resize( nin );
      ISMRes.resize( nout );
      POLISMAux.resize( nin );
      DLISMAux.resize( options.ISMDIV );
      DUISMAux.resize( options.ISMDIV );
      return;

    case Options::ASM:
      if( !ASMEnv || ASMEnv->nvar() != nin || ASMEnv->ndiv() != options.ISMDIV ){
        delete ASMEnv;
        ASMEnv = new ASModel<T>( nin, options.ISMDIV );
      }
      ASMEnv->options.SLOPE_USE  = options.ISMSLOPE;
      ASMEnv->options.SHADOW_USE = options.ISMSHADOW;
      ASMVar.resize( nin );
      ASMRes.resize( nout );
      POLLASMAux.resize( nin );
      POLUASMAux.resize( nin );
      return;
  }
}

template<typename T>
inline void
ANN<T>::propagate
( PolBase<T>* img, FFVar ** pRes, PolVar<T>* vRes, PolVar<T> const* vVar )
{
  switch( options.RELAX ){
    // Polyhedral relaxation with auxiliary variables
    case Options::POL:
    default:
      POLEnv->options = img->options;
      POLEnv->reset();
      POLMap.clear();
      for( unsigned i=0; i<nin; ++i ){
        POLVar[i].set( POLEnv, DAGVar[i], vVar[i].range() );
        POLMap[&POLVar[i]] = vVar[i];
      }
      DAG.first->eval( DAGOps, nout, DAGRes.data(), POLRes.data(), nin, DAGVar.data(), POLVar.data() );   
      // COULD DO CONSTRAINT PROPAGATION BASED ON vRes.range()
      for( unsigned j=0; j<nout; ++j ){
        vRes[j].set( img, *pRes[j], POLRes[j].range() );
#ifdef MC__MCANN_DEBUG
        std::cerr << "vRes[" << j << "] in " << vRes[j] << std::endl;
#endif
        POLMap[&POLRes[j]] = vRes[j];
      }
      break;

    // Interval bounds
    case ANN<T>::Options::RELAXTYPE::INT:
      for( unsigned i=0; i<nin; ++i )
        IVar[i] = vVar[i].range();
      evaluate( IRes.data(), IVar.data() );
      for( unsigned j=0; j<nout; ++j ){
        vRes[j].set( img, *pRes[j], IRes[j] );
#ifdef MC__MCANN_DEBUG
        std::cerr << "vRes[" << j << "] in " << vRes[j] << std::endl;
#endif
      }
      return;
      
    // McCormick relaxations at mid-point with subgradient in each direction
    case ANN<T>::Options::RELAXTYPE::MC:
      for( unsigned i=0; i<nin; ++i )
      MCVar[i] = McCormick<T>( vVar[i].range(), Op<T>::mid( vVar[i].range() ) ).sub( nin, i );
      evaluate( MCRes.data(), MCVar.data() );
      for( unsigned j=0; j<nout; ++j ){
        vRes[j].set( img, *pRes[j], MCRes[j].I() );
#ifdef MC__MCANN_DEBUG
        std::cerr << "vRes[" << j << "] in " << vRes[j].range() << std::endl;
#endif
      }
      return;

    // Interval superposition models
    case ANN<T>::Options::RELAXTYPE::ISM:
      for( unsigned i=0; i<nin; ++i )
        ISMVar[i].set( ISMEnv, i, vVar[i].range() );
      evaluate( ISMRes.data(), ISMVar.data() );
      for( unsigned j=0; j<nout; ++j ){
        vRes[j].set( img, *pRes[j], ISMRes[j].B() );
//#ifdef MC__MCANN_DEBUG
        std::cerr << "ISMRes[" << j << "] in " << ISMRes[j] << std::endl;
        std::cerr << "vRes[" << j << "] in " << vRes[j].range() << std::endl;
//#endif
      }
      return;

 
    // McCormick relaxation with ISM bounds at mid-point with subgradient in each direction
    case ANN<T>::Options::RELAXTYPE::MCISM:
      for( unsigned i=0; i<nin; ++i )
        MCISMVar[i] = McCormick<ISVar<T>>( ISVar<T>( ISMEnv, i, vVar[i].range() ), Op<T>::mid( vVar[i].range() ) ).sub( nin, i );
      evaluate( MCISMRes.data(), MCISMVar.data() );
      for( unsigned j=0; j<nout; ++j ){
        vRes[j].set( img, *pRes[j], MCISMRes[j].I().B() );
//#ifdef MC__MCANN_DEBUG
        std::cerr << "ISMRes[" << j << "] in " << MCISMRes[j].I() << std::endl;
        std::cerr << "MCISMRes[" << j << "] in " << MCISMRes[j] << std::endl;
        std::cerr << "vRes[" << j << "] in " << vRes[j].range() << std::endl;
//#endif
      }
      return;

    // Affine superposition models
    case ANN<T>::Options::RELAXTYPE::ASM:
      UnivarPWLE<double>::nbpsMax = options.ASMBPS;       
      for( unsigned i=0; i<nin; ++i )
        ASMVar[i].set( ASMEnv, i, vVar[i].range() );
      evaluate( ASMRes.data(), ASMVar.data() );
      for( unsigned j=0; j<nout; ++j ){
        vRes[j].set( img, *pRes[j], ASMRes[j].B() );
#ifdef MC__MCANN_DEBUG
        std::cerr << "ASMRes[" << j << "] in " << ASMRes[j] << std::endl;
        std::cerr << "vRes[" << j << "] in " << vRes[j].range() << std::endl;
#endif
      }
      return;
  }
}

template<typename T>
inline void
ANN<T>::back_propagate
( PolBase<T>* img, FFOp* pOp, PolVar<T> const* vRes, PolVar<T>* vVar )
{
  switch( options.RELAX ){
    // Polyhedral relaxation with auxiliary variables
    case Options::POL:
    default:
      POLEnv->generate_cuts( nout, POLRes.data() );
#ifdef MC__MCANN_DEBUG
      std::cerr << "POLEnv:" << *POLEnv << std::endl;
#endif
      img->insert_cuts( POLEnv, POLMap );
      break;

    // Interval bounds
    case ANN<T>::Options::RELAXTYPE::INT:
      return;
      
    // McCormick relaxations at mid-point with subgradient in each direction
    case ANN<T>::Options::RELAXTYPE::MC:
      // polyhedral cut generation
      for( unsigned j=0; j<nout; ++j ){
#ifdef MC__MCANN_DEBUG
        std::cerr << "MCRes[" << j << "] in " << MCRes[j] << std::endl;
#endif
        double rhs1 = -MCRes[j].cv(),
               rhs2 = -MCRes[j].cc();
        for( unsigned i=0; i<nin; ++i ){
          rhs1 += MCRes[j].cvsub(i)*MCVar[i].cv();
          rhs2 += MCRes[j].ccsub(i)*MCVar[i].cc();
        }
        img->add_cut( pOp, PolCut<T>::LE, rhs1, nin, vVar, MCRes[j].cvsub(), vRes[j], -1. );
        img->add_cut( pOp, PolCut<T>::GE, rhs2, nin, vVar, MCRes[j].ccsub(), vRes[j], -1. );
      }
      return;

    // Interval superposition models
    case ANN<T>::Options::RELAXTYPE::ISM:
#ifdef MC__MCANN_CHECK
      assert( ISMEnv->ndiv() == options.ISMDIV );
#endif
      // define auxiliary variables 
      for( unsigned i=0; i<nin; ++i ){
        POLISMAux[i].resize( ISMEnv->ndiv() );
        for( unsigned k=0; k<ISMEnv->ndiv(); ++k )
          POLISMAux[i][k].set( img, Op<T>::zeroone(), options.ISMCONT );
      }

      // polyhedral cut generation
      for( unsigned j=0; j<nout; ++j ){
#ifdef MC__MCANN_DEBUG
        std::cerr << "MCRes[" << j << "] in " << MCRes[j] << std::endl;
#endif
        auto cutF1 = *img->add_cut( pOp, PolCut<T>::LE, 0., vRes[j], -1. );
        auto cutF2 = *img->add_cut( pOp, PolCut<T>::GE, 0., vRes[j], -1. );
        for( unsigned i=0; i<nin; ++i ){
          auto const& ISMi = ISMRes[j].C()[i];
          if( ISMi.empty() ) continue;
          for( unsigned k=0; k<ISMEnv->ndiv(); ++k ){
            DLISMAux[k] = Op<T>::l( ISMi[k] );
            DUISMAux[k] = Op<T>::u( ISMi[k] );
          }
          cutF1->append( ISMEnv->ndiv(), POLISMAux[i].data(), DLISMAux.data() );
          cutF2->append( ISMEnv->ndiv(), POLISMAux[i].data(), DUISMAux.data() );
        }

        // add polyhedral cuts for ISM-participating variables
        for( unsigned i=0; i<nin; i++ ){
          if( POLISMAux[i].empty() ) continue;
          // auxiliaries add up to 1
          img->add_cut( pOp, PolCut<T>::EQ, 1., ISMEnv->ndiv(), POLISMAux[i].data(), 1. );

          // link auxiliaries to model variables
          PolVar<T> POLvarL( 0. ), POLvarU( 0. );
          auto&& ISMi = ISMVar[i].C()[i];
          assert( !ISMi.empty() );
          for( unsigned k=0; k<ISMEnv->ndiv(); k++ ){
            DLISMAux[k] = Op<T>::l(ISMi[k]);
            DUISMAux[k] = Op<T>::u(ISMi[k]);
          } 
          img->add_cut( pOp, PolCut<T>::LE, 0., ISMEnv->ndiv(), POLISMAux[i].data(), DLISMAux.data(), vVar[i], -1. );
          img->add_cut( pOp, PolCut<T>::GE, 0., ISMEnv->ndiv(), POLISMAux[i].data(), DUISMAux.data(), vVar[i], -1. );
        }
      }
      return;

    // McCormick relaxation with ISM bounds at mid-point with subgradient in each direction    
    case ANN<T>::Options::RELAXTYPE::MCISM:
#ifdef MC__MCANN_CHECK
      assert( ISMEnv->ndiv() == options.ISMDIV );
#endif
      // define auxiliary variables 
      for( unsigned i=0; i<nin; ++i ){
        POLISMAux[i].resize( ISMEnv->ndiv() );
        for( unsigned k=0; k<ISMEnv->ndiv(); ++k )
          POLISMAux[i][k].set( img, Op<T>::zeroone(), options.ISMCONT );  
      }
  
      // polyhedral cut generation
      for( unsigned j=0; j<nout; ++j ){
#ifdef MC__MCANN_DEBUG
        std::cerr << "MCISMRes[" << j << "] in " << MCISMRes[j] << std::endl;
#endif
        auto cutF1 = *img->add_cut( pOp, PolCut<T>::LE, 0., vRes[j], -1. );
        auto cutF2 = *img->add_cut( pOp, PolCut<T>::GE, 0., vRes[j], -1. );
        for( unsigned i=0; i<nin; ++i ){
          auto const& ISMi = MCISMRes[j].I().C()[i];
          if( ISMi.empty() ) continue;
          for( unsigned k=0; k<ISMEnv->ndiv(); ++k ){
            DLISMAux[k] = Op<T>::l( ISMi[k] );
            DUISMAux[k] = Op<T>::u( ISMi[k] );
          }
          cutF1->append( ISMEnv->ndiv(), POLISMAux[i].data(), DLISMAux.data() );
          cutF2->append( ISMEnv->ndiv(), POLISMAux[i].data(), DUISMAux.data() );
        }

        // add polyhedral cuts for ISM-participating variables
        for( unsigned i=0; i<nin; i++ ){
          if( POLISMAux[i].empty() ) continue;
          // auxiliaries add up to 1
          img->add_cut( pOp, PolCut<T>::EQ, 1., ISMEnv->ndiv(), POLISMAux[i].data(), 1. );

          // link ISM auxiliaries to model variables
          PolVar<T> POLvarL( 0. ), POLvarU( 0. );
          auto&& ISMi = MCISMVar[i].I().C()[i];
          assert( !ISMi.empty() );
          for( unsigned k=0; k<ISMEnv->ndiv(); k++ ){
            DLISMAux[k] = Op<T>::l(ISMi[k]);
            DUISMAux[k] = Op<T>::u(ISMi[k]);
          }
          img->add_cut( pOp, PolCut<T>::LE, 0., ISMEnv->ndiv(), POLISMAux[i].data(), DLISMAux.data(), vVar[i], -1. );
          img->add_cut( pOp, PolCut<T>::GE, 0., ISMEnv->ndiv(), POLISMAux[i].data(), DUISMAux.data(), vVar[i], -1. );
        }

        // polyhedral cut generation for MC  
        double rhs1 = -MCISMRes[j].cv(),  
               rhs2 = -MCISMRes[j].cc();
        for( unsigned i=0; i<nin; ++i ){ 
          rhs1 += MCISMRes[j].cvsub(i) * MCISMVar[i].cv();
          rhs2 += MCISMRes[j].ccsub(i) * MCISMVar[i].cc();
        }
        img->add_cut( pOp, PolCut<T>::LE, rhs1, nin, vVar, MCISMRes[j].cvsub(), vRes[j], -1. );
        img->add_cut( pOp, PolCut<T>::GE, rhs2, nin, vVar, MCISMRes[j].ccsub(), vRes[j], -1. );
      }
      return;
      
    // Affine superposition models
    case ANN<T>::Options::RELAXTYPE::ASM:
#ifdef MC__MCANN_CHECK
      assert( ASMEnv->ndiv() == options.ISMDIV );
#endif
      // polyhedral cut generation
      for( unsigned j=0; j<nout; ++j ){
#ifdef MC__MCANN_DEBUG
        std::cerr << "ASMRes[" << j << "] in " << ASMRes[j] << std::endl;
#endif
        switch(ASMRes[j].get_ASVar()){
          case 1:
            append_ASMcuts( img, pOp, vRes[j], vVar, ASMRes[j].get_cst() );
            break;    
                    
          case 2:
            append_ASMcuts( img, pOp, vRes[j], vVar, ASMRes[j].get_cst(), ASMRes[j].get_lnr() );
            break;

          case 3:
            append_ASMcuts( img, pOp, vRes[j], vVar, ASMRes[j].get_lst() );          
            break;

          case 4:
            append_ASMcuts( img, pOp, vRes[j], vVar, ASMRes[j].get_lst() );
            if( options.CUTSHADOW && options.ISMSHADOW )
              append_ASMcuts( img, pOp, vRes[j], vVar, ASMRes[j].get_shadow() );
            break;

          default:
           throw std::runtime_error("ANN::relax: **ERROR** invalid flag from get_ASVar()\n");
        }
      }
      return;
  }

#ifdef MC__MCANN_DEBUG
  std::cerr << *img;
  {int dum; std::cout << "PAUSED, ENTER 1"; std::cin >> dum;}
#endif
}

template<typename T>
inline void
ANN<T>::append_ASMcuts
( PolBase<T>* img, FFOp* pOp, PolVar<T> const& vRes, PolVar<T>* vVar,
  double const& rhsEst, std::vector<double> const& lnrEst )
{
  double rhs = rhsEst / (double)nin;
  for( unsigned i=0; i<nin; ++i ){
    double DX = mc::Op<T>::diam(vVar[i].range()),
           DY = !lnrEst.empty()? lnrEst[i] * DX: 0.,
           XL = mc::Op<T>::l(vVar[i].range()),
           YL = !lnrEst.empty()? lnrEst[i] * DX + rhs: rhs;
    T IY = !lnrEst.empty()? lnrEst[i] * vVar[i].range() + rhs: T(rhs);
    POLLASMAux[i].set( img, IY, true );
    img->add_cut( pOp, mc::PolCut<T>::EQ, DX*YL-DY*XL, POLLASMAux[i], DX, vVar[i], -DY );
  }
  img->add_cut( pOp, PolCut<T>::EQ, 0., nin, POLLASMAux.data(), 1., vRes, -1. );
}

template<typename T>
inline void
ANN<T>::append_ASMcuts
( PolBase<T>* img, FFOp* pOp, PolVar<T> const& vRes, PolVar<T>* vVar,
  std::vector<UnivarPWL<T>> const& pwlEst )
{
  for( unsigned i=0; i<nin; ++i ){
    UnivarPWLE<double> const& uest = pwlEst[i].undEst;
    if( uest.empty() )
      POLLASMAux[i].set( img, T(0.), true );
    else{ 
      POLLASMAux[i].set( img, T(uest.get_lb(),uest.get_ub()), true );
      auto const [ucst,isuCst] = uest.get_cst();
      if( isuCst )
        img->add_cut( pOp, PolCut<T>::EQ, ucst, POLLASMAux[i], 1. );
      else{
        unsigned NK = uest.first.size()-1;
#ifdef MC__MCANN_CHECK
        assert( uest.second.size() == uest.first.size() );
#endif
        if(NK==1){
          NK += 1;
          DXASMAux.assign( NK, 0. );
          DYASMAux.assign( NK, 0. );
          for( unsigned j=0; j<NK; ++j ){
            DXASMAux[j] = uest.first[j];
            DYASMAux[j] = uest.second[j];
          }            
        }
        else{
          DXASMAux.assign( NK, uest.first[0] );
          DYASMAux.assign( NK, uest.second[0] );
          for( unsigned j=0; j<NK; ++j ){
            DXASMAux[j] += uest.first[j+1];
            DYASMAux[j] += uest.second[j+1];
          }
        }
        img->add_semilinear_cuts( pOp, NK, vVar[i], DXASMAux.data(), POLLASMAux[i], DYASMAux.data(), mc::PolCut<T>::EQ );
      }
    }

    UnivarPWLE<double> const& oest = pwlEst[i].oveEst;
    if( oest.empty() )
      POLUASMAux[i].set( img, T(0.), true );
    else{
      POLUASMAux[i].set( img, T(oest.get_lb(),oest.get_ub()), true );
      auto const [ocst,isoCst] = oest.get_cst();
      if( isoCst )
        img->add_cut( pOp, PolCut<T>::EQ, ocst, POLUASMAux[i], 1. );
      else{
        unsigned NK = oest.first.size()-1;
#ifdef MC__MCANN_CHECK
        assert( oest.second.size() == oest.first.size() );
#endif
        if(NK == 1){
          NK += 1;
          DXASMAux.assign( NK, 0. );
          DYASMAux.assign( NK, 0. );
          for( unsigned j=0; j<NK; ++j ){
            DXASMAux[j] = oest.first[j];
            DYASMAux[j] = oest.second[j];
          }
        }  
        else{  
          DXASMAux.assign( NK, oest.first[0] );
          DYASMAux.assign( NK, oest.second[0] );
          for( unsigned j=0; j<NK; ++j ){
            DXASMAux[j] += oest.first[j+1];
            DYASMAux[j] += oest.second[j+1];
          }
        }	      
        img->add_semilinear_cuts( pOp, NK, vVar[i], DXASMAux.data(), POLUASMAux[i], DYASMAux.data(), mc::PolCut<T>::EQ );
      }
    }
  }
  img->add_cut( pOp, PolCut<T>::LE, 0., nin, POLLASMAux.data(), 1., vRes, -1. );
  img->add_cut( pOp, PolCut<T>::GE, 0., nin, POLUASMAux.data(), 1., vRes, -1. );
} 

//! @brief C++ class defining neural networks as external DAG operations in MC++.
////////////////////////////////////////////////////////////////////////
//! mc::FFANN is a C++ class for defining neural networks as external
//! DAG operations in MC++.
////////////////////////////////////////////////////////////////////////
template<typename T, unsigned int ID>
class FFANN
////////////////////////////////////////////////////////////////////////
: public FFOp
{
public:

  //! @brief Constructors
  FFANN
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( unsigned const idep, unsigned const nVar, FFVar const* pVar, ANN<T>* pANN )
    const
    {
      data = pANN;
      info = ID;
#ifdef MC__MCANN_CHECK
      assert( nVar == pANN->nin && idep < pANN->nout );
#endif
      auto dep = FFDep();
      for( unsigned i=0; i<nVar; ++i ) dep += pVar[i].dep();
      dep.update( FFDep::TYPE::N );
      return *(insert_external_operation( *this, pANN->nout, dep, nVar, pVar )[idep]);
    }

  FFVar** operator()
    ( unsigned const nVar, FFVar const* pVar, ANN<T>* pANN )
    const
    {
      data = pANN;
      info = ID;
#ifdef MC__MCANN_CHECK
      assert( nVar == pANN->nin );
#endif
      auto dep = FFDep();
      for( unsigned i=0; i<nVar; ++i ) dep += pVar[i].dep();
      dep.update( FFDep::TYPE::N );
      return insert_external_operation( *this, pANN->nout, dep, nVar, pVar );
    }

  // Forward evaluation overloads
  template<typename U>
  void eval
    ( unsigned const nRes, U* vRes, unsigned const nVar, U const* vVar, unsigned const* mVar )
    const;

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const;

  void eval
    ( unsigned const nRes, SLiftVar* vRes, unsigned const nVar, SLiftVar const* vVar, unsigned const* mVar )
    const;

  void eval
    ( unsigned const nRes, PolVar<T>* vRes, unsigned const nVar, PolVar<T> const* vVar, unsigned const* mVar )
    const;

  // Backward evaluation overloads
  template<typename U>
  bool reval
    ( unsigned const nRes, U const* vRes, unsigned const nVar, U* vVar )
    const
    {
      throw std::runtime_error("FFANN::reval: **ERROR** no generic implementation\n");
    }

  bool reval
    ( unsigned const nRes, PolVar<T> const* vRes, unsigned const nVar, PolVar<T>* vVar )
    const;

  // Properties
  std::string name
    ()
    const
    { std::ostringstream oss; oss << data; return "ANN[" + oss.str() + "]"; }

  //! @brief Return whether or not operation is commutative
  bool commutative
    ()
    const
    { return false; }
};


template<typename T, unsigned int ID>
template<typename U>
inline void
FFANN<T,ID>::eval
( unsigned const nRes, U* vRes, unsigned const nVar, U const* vVar,
  unsigned const* mVar )
const
{
#ifdef MC__ANNOP_TRACE
  std::cout << "FFANN::eval: " << typeid( vRes[0] ).name() << " (generic)\n";
#endif
  ANN<T>* pANN = static_cast<ANN<T>*>( data );
#ifdef MC__MCANN_CHECK
  assert( pANN && nRes == pANN->nout && nVar == pANN->nin );
#endif

  pANN->evaluate( vRes, vVar );
}

template<typename T, unsigned int ID>
inline void
FFANN<T,ID>::eval
( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar,
  unsigned const* mVar )
const
{
#ifdef MC__FFANN_TRACE
  std::cout << "FFANN::eval: FFVar\n";
#endif
  ANN<T>* pANN = static_cast<ANN<T>*>( data );
#ifdef MC__MCANN_CHECK
  assert( pANN && nRes == pANN->nout && nVar == pANN->nin );
#endif

  FFVar** pRes = operator()( nVar, vVar, pANN );
  for( unsigned j=0; j<nRes; ++j ) vRes[j] = *(pRes[j]);
}

template<typename T, unsigned int ID>
inline void
FFANN<T,ID>::eval
( unsigned const nRes, SLiftVar* vRes, unsigned const nVar, SLiftVar const* vVar,
  unsigned const* mVar )
const
{
#ifdef MC__FFANN_TRACE
  std::cout << "FFANN::eval: SLiftVar\n";
#endif
  ANN<T>* pANN = static_cast<ANN<T>*>( data );
#ifdef MC__MCANN_CHECK
  assert( pANN && nRes == pANN->nout && nVar == pANN->nin );
#endif

  vVar->env()->lift( nRes, vRes, nVar, vVar );
}

template<typename T, unsigned int ID>
inline void
FFANN<T,ID>::eval
( unsigned const nRes, PolVar<T>* vRes, unsigned const nVar, PolVar<T> const* vVar,
  unsigned const* mVar )
const
{
#ifdef MC__FFANN_TRACE
  std::cout << "FFANN::eval: PolVar\n";
#endif
  ANN<T>* pANN = static_cast<ANN<T>*>( data );
#ifdef MC__MCANN_CHECK
  assert( pANN && nVar == pANN->nin && nRes == pANN->nout );
#endif

  PolBase<T>* img = vVar[0].image();
  FFBase* dag = vVar[0].var().dag();
#ifdef MC__MCANN_CHECK
  assert( img && dag );
#endif
  FFVar** pRes = dag->curOp()->varout.data(); // ACCOUNT FOR MULTIPLE OUTPUTS
#ifdef MC__MCANN_CHECK
  assert( nRes == dag->curOp()->varout.size() );
#endif

  pANN->resize();
  pANN->propagate( img, pRes, vRes, vVar );
}

template<typename T, unsigned int ID>
inline bool
FFANN<T,ID>::reval
( unsigned const nRes, PolVar<T> const* vRes, unsigned const nVar, PolVar<T>* vVar )
const
{
#ifdef MC__FFANN_TRACE
  std::cout << "FFANN::reval: PolVar\n";
#endif
  ANN<T>* pANN = static_cast<ANN<T>*>( data );
#ifdef MC__MCANN_CHECK
  assert( pANN && nVar == pANN->nin && nRes == pANN->nout );
#endif

  PolBase<T>* img = vVar[0].image();
  FFOp* pop = vVar[0].var().opdef().first;
#ifdef MC__MCANN_CHECK
  assert( img && pop );
#endif

  pANN->back_propagate( img, pop, vRes, vVar );
  return true;
}

} // end namespace mc

#endif
