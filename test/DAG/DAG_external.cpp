#undef  MC__FFUNC_CPU_EVAL
////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>

#include "mctime.hpp"
#include "mclapack.hpp"
#include "ffunc.hpp"
#include "slift.hpp"
#include "mcml.hpp"

#ifdef MC__USE_PROFIL
 #include "mcprofil.hpp"
 typedef INTERVAL I;
#else
 #ifdef MC__USE_FILIB
  #include "mcfilib.hpp"
  typedef filib::interval<double,filib::native_switched,filib::i_mode_extended> I;
 #else
  #ifdef MC__USE_BOOST
   #include "mcboost.hpp"
   typedef boost::numeric::interval_lib::save_state<boost::numeric::interval_lib::rounded_transc_opp<double>> T_boost_round;
   typedef boost::numeric::interval_lib::checking_base<double> T_boost_check;
   typedef boost::numeric::interval_lib::policies<T_boost_round,T_boost_check> T_boost_policy;
   typedef boost::numeric::interval<double,T_boost_policy> I;
  #else
   #include "interval.hpp"
   typedef mc::Interval I;
  #endif
 #endif
#endif

#include "mccormick.hpp"
typedef mc::McCormick<I> MC;

#include "cmodel.hpp"
typedef mc::CModel<I> CM;
typedef mc::CVar<I> CV;

#include "scmodel.hpp"
typedef mc::SCModel<I> SCM;
typedef mc::SCVar<I> SCV;

#include "polimage.hpp"
typedef mc::PolVar<I> POLV;


////////////////////////////////////////////////////////////////////////
// EXTERNAL OPERATIONS
////////////////////////////////////////////////////////////////////////
namespace mc
{

template<unsigned int ID>
class FFnorm2
: public FFOp
{
public:
  // Constructors
  FFnorm2
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( unsigned const nVar, FFVar const* pVar )
    const
    {
//      dep.update( FFDep::TYPE::N );
      return **insert_external_operation( *this, 1, nVar, pVar );
    }

  // Evaluation overloads
  template< typename T >
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      std::cout << "NORM2 generic instantiation\n";
      switch( nVar ){
        case 0: vRes[0] = T( 0. ); break;
        case 1: vRes[0] = vVar[0]; break;
        default: vRes[0] = Op<T>::sqr( vVar[0] );
                 for( unsigned i=1; i<nVar; ++i ) vRes[0] += Op<T>::sqr( vVar[i] );
                 vRes[0] = Op<T>::sqrt( vRes[0] ); break;
      }
    }

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      std::cout << "NORM2 FFVar instantiation\n";
      vRes[0] = operator()( nVar, vVar );
    }
    
  void eval
    ( unsigned const nRes, SLiftVar* vRes, unsigned const nVar, SLiftVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar && nRes == 1 );
      std::cout << "NORM2 SLiftVar instantiation\n";
      vVar->env()->lift( nRes, vRes, nVar, vVar );
    }

  void deriv
    ( unsigned const nRes, FFVar const* vRes, unsigned const nVar, FFVar const* vVar, FFVar** vDer )
    const
    {
      assert( nRes == 1 );
      std::cout << "NORM2 FFVar differentiation\n";
      fadbad::F<FFVar> vFVar[nVar], vFRes[nRes];
      for( unsigned i=0; i<nVar; ++i ){
        vFVar[i] = vVar[i];
        vFVar[i].diff( i, nVar );
      }
      eval( nRes, vFRes, nVar, vFVar, nullptr );
      for( unsigned j=0; j<nRes; ++j )
        for( unsigned i=0; i<nVar; ++i )
          vDer[j][i] = vFRes[j].d(i);
    }

  // Properties
  std::string name
    ()
    const
    { return "NORM2"; }
  //! @brief Return whether or not operation is commutative
  bool commutative
    ()
    const
    { return true; }
};

template<unsigned int ID>
class FFnorm12
: public FFOp
{
public:
  // Constructors
  FFnorm12
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( unsigned const idep, unsigned const nVar, FFVar const* pVar )
    const
    {
      return *(insert_external_operation( *this, 2, nVar, pVar )[idep]);
    }
  FFVar** operator()
    ( unsigned const nVar, FFVar const* pVar )
    const
    {
      return insert_external_operation( *this, 2, nVar, pVar );
    }

  // Evaluation overloads
  template< typename T >
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 2 );
      std::cout << "NORM12 generic instantiation\n";
      switch( nVar ){
        case 0: vRes[0] = T( 0. );
                vRes[1] = T( 0. );
                break;
        case 1: vRes[0] = Op<T>::fabs( vVar[0] );
                vRes[1] = Op<T>::fabs( vVar[0] );
                break;
        default: vRes[0] = Op<T>::sqr( vVar[0] );
                 for( unsigned i=1; i<nVar; ++i ) vRes[0] += Op<T>::sqr( vVar[i] );
                 vRes[0] = Op<T>::sqrt( vRes[0] );
                 vRes[1] = Op<T>::fabs( vVar[0] );
                 for( unsigned i=1; i<nVar; ++i ) vRes[1] += Op<T>::fabs( vVar[i] );
                 break;
      }
    }

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      std::cout << "NORM12 FFVar instantiation\n";
      FFVar** ppRes = operator()( nVar, vVar );
      for( unsigned j=0; j<nRes; ++j ) vRes[j] = *(ppRes[j]);
    }
    
  void eval
    ( unsigned const nRes, SLiftVar* vRes, unsigned const nVar, SLiftVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar && nRes == 2 );
      std::cout << "NORM12 SLiftVar instantiation\n";
      vVar->env()->lift( nRes, vRes, nVar, vVar );
    }

  void deriv
    ( unsigned const nRes, FFVar const* vRes, unsigned const nVar, FFVar const* vVar, FFVar** vDer )
    const
    {
      assert( nRes == 2 );
      std::cout << "NORM12 FFVar differentiation\n";
      fadbad::F<FFVar> vFVar[nVar], vFRes[nRes];
      for( unsigned i=0; i<nVar; ++i ){
        vFVar[i] = vVar[i];
        vFVar[i].diff( i, nVar );
      }
      eval( nRes, vFRes, nVar, vFVar, nullptr );
      for( unsigned j=0; j<nRes; ++j )
        for( unsigned i=0; i<nVar; ++i )
          vDer[j][i] = vFRes[j].d(i);
    }

  // Properties
  std::string name
    ()
    const
    { return "NORM12"; }
  //! @brief Return whether or not operation is commutative
  bool commutative
    ()
    const
    { return true; }
};

template<unsigned int ID>
class FFxlog
: public FFOp
{
public:
  // Constructors
  FFxlog
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( FFVar const& Var )
    const
    {
      return *(insert_external_operation( *this, 1, Var )[0]);
    }

  // Evaluation overloads
  template <typename T>
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      std::cout << "xlog generic instantiation\n"; 
      vRes[0] = vVar[0] * Op<T>::log( vVar[0] );
    }

  template <typename T>
  void eval
    ( unsigned const nRes, McCormick<T>* vRes, unsigned const nVar, McCormick<T> const* vVar,
      unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      std::cout << "xlog McCormick instantiation\n"; 
      vRes[0] = xlog( vVar[0] );
    }

  void eval
    ( unsigned const nRes, fadbad::F<FFVar>* vRes, unsigned const nVar, fadbad::F<FFVar> const* vVar,
      unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      std::cout << "xlog fadbad::F<FFVar> instantiation\n"; 
      vRes[0] = operator()( vVar[0].val() );
      if( !vVar[0].depend() ) return;
      FFVar dxlog( log( vVar[0].val()) + 1 );
      vRes[0].setDepend( vVar[0] );
      for( unsigned int i=0; i<vRes[0].size(); ++i )
        vRes[0][i] = dxlog * vVar[0][i];
    }

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      vRes[0] = operator()( vVar[0] );
    }

  template <typename T>
  void eval
    ( unsigned const nRes, PolVar<T>* vRes, unsigned const nVar, PolVar<T> const* vVar,
      unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      std::cout << "xlog Polyhedral image instantiation\n"; 
      PolBase<T>* img = vVar[0].image();
      FFBase* dag = vVar[0].var().dag();
      assert( img && dag );
      FFVar* pRes = dag->curOp()->varout[0];
      T TRes = Op<I>::xlog( vVar[0].range() );
      vRes[0].set( img, *pRes, TRes );
      // vRes[0] = xlog( vVar[0] );
    }

  template <typename T>
  bool reval
    ( unsigned const nRes, PolVar<T> const* vRes, unsigned const nVar, PolVar<T>* vVar )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      std::cout << "xlog Polyhedral image generation\n"; 
      PolBase<T>* img = vVar[0].image();
      FFBase* dag = vVar[0].var().dag();
      FFOp* pop = vVar[0].var().opdef().first;
      assert( img && dag && pop );
      struct loc{ static std::pair<double,double> xlog
        ( const double x, const double*rusr, const int*iusr )
        { return std::make_pair( mc::xlog(x), std::log(x)+1. ); }
      };
      img->add_semilinear_cuts( pop, vVar[0], Op<T>::l(vVar[0].range()), Op<T>::u(vVar[0].range()),
        vRes[0], PolCut<T>::LE, loc::xlog );
      img->add_sandwich_cuts( pop, vVar[0], Op<T>::l(vVar[0].range()), Op<T>::u(vVar[0].range()),
        vRes[0], Op<T>::l(vRes[0].range()), Op<T>::u(vRes[0].range()), PolCut<T>::GE, loc::xlog );
      return true;
    }

  void deriv
    ( unsigned const nRes, FFVar const* vRes, unsigned const nVar, FFVar const* vVar, FFVar** vDer )
    const
    {
      assert( nVar == 1 && nRes == 1 );
      std::cout << "xlog FFVar differentiation\n";
      vDer[0][0] = log( vVar[0] ) + 1;
    }

  // Properties
  std::string name
    ()
    const
    { return "XLOG EXT"; }
};

template<unsigned int ID>
class FFdet
: public FFOp
{
public:
  // Constructors
  FFdet
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( unsigned const nVar, FFVar const* pVar )
    const
    {
      return **insert_external_operation( *this, 1, nVar, pVar );
    }

  // Evaluation overloads
  template< typename T >
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      std::cout << "Det generic instantiation\n"; 
      const unsigned nDim = std::sqrt(nVar);
      switch( nDim ){
        case 0:  vRes[0] = T( 0. ); break;
	case 1:  vRes[0] = vVar[0]; break;
        default: vRes[0] = FFBase::det( nDim, vVar ); break;
      }
    }
  void eval
    ( unsigned const nRes, double* vRes, unsigned const nVar, double const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      std::cout << "Det double instantiation\n"; 
      const unsigned nDim = std::sqrt(nVar);
      CPPL::dgematrix Amat( nDim, nDim );
      for( unsigned i=0; i<nDim; ++i )
        for( unsigned j=0; j<nDim; ++j )
          Amat(i,j) = vVar[i+j*nDim];
      if( dgeqrf( Amat, vRes[0] ) )
        throw FFBase::Exceptions( FFBase::Exceptions::EXTERN );
    }
  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      vRes[0] = operator()( nVar, vVar );
    }
  void eval
    ( unsigned const nRes, FFDep* vRes, unsigned const nVar, FFDep const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      vRes[0] = 0;
      for( unsigned i=0; i<nVar; ++i ) vRes[0] += vVar[i];
      switch( nVar ){
        case 0:
        case 1:  vRes[0].update( FFDep::TYPE::L ); break;
        case 2:  vRes[0].update( FFDep::TYPE::Q ); break;
        default: vRes[0].update( FFDep::TYPE::P ); break;
      }
    }

  // Properties
  std::string name
    ()
    const
    { return "DET"; }
  //! @brief Return whether or not operation is commutative
  bool commutative
    ()
    const
    { return false; }
};

struct FFDOptBase
{
  // Vector of atom matrices
  static std::vector< CPPL::dsymatrix > _A;

  // Read atom matrices from file
  static unsigned read
    ( unsigned const dim, std::string filename, bool const disp=false )
    {
      CPPL::dsymatrix Ai( dim );
      _A.clear();
      std::ifstream file( filename );
      if( !file ) throw std::runtime_error("Error: Could not open input file\n");
      std::string line;
      unsigned i = 0;
      bool empty = false;
      while( std::getline( file, line ) ){
        std::istringstream iss( line );
        for( unsigned j=0; j<dim; j++ ){
          if( !(iss >> Ai(i,j) ) ){
            if( j ) throw std::runtime_error("Error: Could not read input file\n");
            empty = true;
            break;
          }
          std::cout << "reading (" << i << "," << j << "): " << Ai(i,j) << std::endl;
        }
        i++;
        if( empty ){
          if( disp ) std::cout << "Atomic matrix #" << _A.size() << ":" << std::endl << Ai;
          _A.push_back( Ai );
          i = 0;
          empty = false;
        }
        if( i > dim ) throw std::runtime_error("Error: Could not read input file\n");
      }
      if( i ) _A.push_back( Ai );
      return _A.size();
    }
};

inline std::vector< CPPL::dsymatrix > FFDOptBase::_A;

template<unsigned int ID>
class FFDOpt
: public FFOp,
  public FFDOptBase
{
public:
  // Constructors
  FFDOpt
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( unsigned const nVar, FFVar const* pVar )
    const
    {
      info = ID;
      return **insert_external_operation( *this, 1, nVar, pVar );
    }

  // Evaluation overloads
  template< typename T >
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      throw std::runtime_error("Error: No generic implementation for DOpt\n");
    }

  void eval
    ( unsigned const nRes, double* vRes, unsigned const nVar, double const* vVar, unsigned const* mVar )
    const
    {
      std::cout << "FFDOpt::eval: double\n"; 
      assert( nRes == 1 && nVar == _A.size() && _A.begin() != _A.end() );
      CPPL::dsymatrix Amat( _A[0].n );
      Amat.zero();
      for( unsigned i=0; i<nVar; ++i )
        if( !i ) Amat  = vVar[0] * _A[0];
        else     Amat += vVar[i] * _A[i];
      //std::cout << Amat;
      if( dgeqrf( Amat.to_dgematrix(), vRes[0] ) )
        throw FFBase::Exceptions( FFBase::Exceptions::EXTERN );
      vRes[0] = std::log( vRes[0] );
    }

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      std::cout << "FFDOpt::eval: FFVar\n"; 
      vRes[0] = operator()( nVar, vVar );
    }

  void eval
    ( unsigned const nRes, FFDep* vRes, unsigned const nVar, FFDep const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == 1 );
      std::cout << "FFDOpt::eval: FFDep\n"; 
      vRes[0] = 0;
      for( unsigned i=0; i<nVar; ++i ) vRes[0] += vVar[i];
      vRes[0].update( FFDep::TYPE::N );
    }

  void eval
    ( unsigned const nRes, fadbad::F<FFVar>* vRes, unsigned const nVar, fadbad::F<FFVar> const* vVar,
      unsigned const* mVar )
    const;

  void deriv
    ( unsigned const nRes, FFVar const* vRes, unsigned const nVar, FFVar const* vVar, FFVar** vDer )
    const;

  // Properties
  std::string name
    ()
    const
    { return "DOPT"; }
  //! @brief Return whether or not operation is commutative
  bool commutative
    ()
    const
    { return false; }
};

template<unsigned int ID>
class FFDOptGrad
: public FFOp,
  public FFDOptBase
{
public:
  // Constructors
  FFDOptGrad
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( unsigned const idep, unsigned const nVar, FFVar const* pVar )
    const
    {
      info = ID+1;
      return *(insert_external_operation( *this, nVar, nVar, pVar )[idep]);
    }
  FFVar** operator()
    ( unsigned const nVar, FFVar const* pVar )
    const
    {
      info = ID+1;
      return insert_external_operation( *this, nVar, nVar, pVar );
    }

  // Evaluation overloads
  template< typename T >
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      throw std::runtime_error("Error: No generic implementation for DOptGrad\n");
    }

  void eval
    ( unsigned const nRes, double* vRes, unsigned const nVar, double const* vVar, unsigned const* mVar )
    const
    {
      std::cout << "FFDOptGrad::eval: double\n"; 
      assert( nRes == nVar && nVar == _A.size() && _A.begin() != _A.end() );
      CPPL::dsymatrix Amat( _A[0].n );
      Amat.zero();
      for( unsigned i=0; i<nVar; ++i )
        if( !i ) Amat  = vVar[0] * _A[0];
        else     Amat += vVar[i] * _A[i];
      //std::cout << Amat;
      // Perform LDL' decomposition
      CPPL::dgematrix Lmat;
      std::vector<int> IPIV;
      if( dsytrf( Amat, Lmat, IPIV ) )
        throw FFBase::Exceptions( FFBase::Exceptions::EXTERN );
      CPPL::dgematrix Xmat;
      for( unsigned i=0; i<nVar; ++i ){
        if( dsytrs( Lmat, IPIV, _A[i].to_dgematrix(), Xmat ) )
          throw FFBase::Exceptions( FFBase::Exceptions::EXTERN );
        //std::cout << Xmat;
        for( int j=0; j<Xmat.n; ++j )
          if( !j ) vRes[i]  = Xmat(0,0);
          else     vRes[i] += Xmat(j,j);
      }
    }

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == nVar );
      std::cout << "FFDOptGrad::eval: FFVar\n"; 
      FFVar** ppRes = operator()( nVar, vVar );
      for( unsigned j=0; j<nRes; ++j ) vRes[j] = *(ppRes[j]);
    }

  void eval
    ( unsigned const nRes, FFDep* vRes, unsigned const nVar, FFDep const* vVar, unsigned const* mVar )
    const
    {
      assert( nRes == nVar );
      std::cout << "FFDOpt::eval: FFDep\n"; 
      vRes[0] = 0;
      for( unsigned i=0; i<nVar; ++i ) vRes[0] += vVar[i];
      vRes[0].update( FFDep::TYPE::N );
      for( unsigned j=1; j<nRes; ++j ) vRes[j] = vRes[0];
    }

  // Properties
  std::string name
    ()
    const
    { return "DOPTGRAD"; }
  //! @brief Return whether or not operation is commutative
  bool commutative
    ()
    const
    { return false; }
};

template<unsigned int ID>
inline void
FFDOpt<ID>::eval
( unsigned const nRes, fadbad::F<FFVar>* vRes, unsigned const nVar, fadbad::F<FFVar> const* vVar,
  unsigned const* mVar )
const
{
  assert( nRes == 1 && nVar == _A.size() && _A.begin() != _A.end() );
  std::cout << "FFDOpt::eval: fadbad::F<FFVar>\n";
  std::vector<FFVar> vVarVal( nVar );
  for( unsigned i=0; i<nVar; ++i )
    vVarVal[i] = vVar[i].val();
  vRes[0] = operator()( nVar, vVarVal.data() );
  FFDOptGrad<ID> DOptGrad;
  for( unsigned i=0; i<nVar; ++i )
    vRes[0].setDepend( vVar[i] );
  for( unsigned j=0; j<vRes[0].size(); ++j )
    for( unsigned i=0; i<nVar; ++i )
      if( !i ) vRes[0][j]  = DOptGrad( 0, nVar, vVarVal.data() ) * vVar[0][j];
      else     vRes[0][j] += DOptGrad( i, nVar, vVarVal.data() ) * vVar[i][j];
}


template<unsigned int ID>
inline void
FFDOpt<ID>::deriv
( unsigned const nRes, FFVar const* vRes, unsigned const nVar, FFVar const* vVar, FFVar** vDer )
const
{
  assert( nRes == 1 && nVar == _A.size() && _A.begin() != _A.end() );
  std::cout << "FFDOpt::deriv: FFVar\n";
  FFDOptGrad<ID> DOptGrad;
  for( unsigned i=0; i<nVar; ++i )
    vDer[0][i] = DOptGrad( i, nVar, vVar );
}

template<unsigned int ID>
class FFArrh
: public FFOp
{
public:
  // Constructors
  FFArrh
    ()
    : FFOp( (int)EXTERN+ID )
    {}

  // Functor
  FFVar& operator()
    ( FFVar const& Var, double& r )
    const
    {
      data = &r; // this is assuming r isn't going out of scope
      info = ID;
      //std::cout << "data: " << data << std::endl;
      return *(insert_external_operation( *this, 1, Var )[0]);
    }

  // Evaluation overloads
  template <typename T>
  void eval
    ( unsigned const nRes, T* vRes, unsigned const nVar, T const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 && data );
      std::cout << "FFArrh: generic instantiation\n"; 
      vRes[0] = Op<T>::exp( - *static_cast<double*>( data ) / vVar[0] );
    }

  template <typename T>
  void eval
    ( unsigned const nRes, McCormick<T>* vRes, unsigned const nVar, McCormick<T> const* vVar,
      unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 && data );
      std::cout << "FFArrh: McCormick instantiation\n"; 
      vRes[0] = arrh( vVar[0], *static_cast<double*>( data ) );
    }

  void eval
    ( unsigned const nRes, FFVar* vRes, unsigned const nVar, FFVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 && data );
      std::cout << "FFArrh: FFVar instantiation\n"; 
      vRes[0] = operator()( vVar[0], *static_cast<double*>( data ) );
    }

  void eval
    ( unsigned const nRes, FFDep* vRes, unsigned const nVar, FFDep const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 && data );
      std::cout << "FFArrh: FFDep instantiation\n";
      vRes[0] = vVar[0];
      vRes[0].update( FFDep::TYPE::N );
    }

  void eval
    ( unsigned const nRes, SLiftVar* vRes, unsigned const nVar, SLiftVar const* vVar, unsigned const* mVar )
    const
    {
      assert( nVar == 1 && nRes == 1 && data );
      std::cout << "FFArrh: SLiftVar instantiation\n";
      vVar[0].env()->lift( nRes, vRes, nVar, vVar );
    }

  // Properties
  std::string name
    ()
    const
    { //std::cout << "data: " << data << std::endl;
      return "ARRH[" + std::to_string(*static_cast<double*>( data )) + "]"; }

  // Data cleanup
  bool cleanup
    ()
    const
    { std::cout << "FFArrh: cleanup\n"; 
      return false; }
};

}

///////////////////////////////////////////////////////////////////////////////

int test_external0()
{
  std::cout << "\n==============================================\ntest_external0:\n";

  // Create DAG
  mc::FFGraph< mc::FFnorm2<0>, mc::FFnorm12<1> > DAG;
  const unsigned NX = 2, NF = 3;
  mc::FFVar X[NX];
  for( unsigned int i=0; i<NX; i++ ) X[i].set( &DAG );
  mc::FFnorm2<0>  norm2;
  mc::FFnorm12<1> norm12;
  mc::FFVar F[NF] = { norm2( NX, X ), norm12( 0, NX, X ), norm12( 1, NX, X ) };
  std::cout << DAG;
  
  std::ofstream o_F( "external0_F.dot", std::ios_base::out );
  DAG.dot_script( NF, F, o_F );
  o_F.close();

  auto F_op  = DAG.subgraph( NF, F );
  DAG.output( F_op );

  // Evaluation in real arithmetic
  double dX[NX] = { 2., 3. }, dF[NF];
  std::vector<double> dwk;
  DAG.eval( F_op, dwk, NF, F, dF, NX, X, dX );
  for( unsigned i=0; i<NF; i++ ) std::cout << "F[" << i << "] = " << dF[i] << std::endl;

  // Evaluation in McCormick arithmetic
  MC mcX[NX] = { MC(I(1.5,2.5),2.), MC(I(2.5,3.5),3.) }, mcF[NF];
  std::vector<MC> mcwk;
  DAG.eval( F_op, mcwk, NF, F, mcF, NX, X, mcX );
  for( unsigned i=0; i<NF; i++ ) std::cout << "F[" << i << "] = " << mcF[i] << std::endl;

  // Forward AD
  const mc::FFVar* dFdX = DAG.FAD( NF, F, NX, X, true );
  std::ofstream o_dFdX( "external0_dFdX.dot", std::ios_base::out );
  DAG.dot_script( NX*NF, dFdX, o_dFdX );
  o_dFdX.close();

  auto dFdX_op  = DAG.subgraph( NX*NF, dFdX );
  DAG.output( dFdX_op );
  delete[] dFdX;


  auto F2_op  = DAG.subgraph( 1, F+1 );
  DAG.output( F2_op );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external1()
{
  std::cout << "\n==============================================\ntest_external1:\n";

  // Create DAG
  mc::FFGraph< mc::FFnorm2<0>, mc::FFxlog<1> > DAG;
  const unsigned NX = 2, NF = 2;
  mc::FFVar X[NX];
  for( unsigned int i=0; i<NX; i++ ) X[i].set( &DAG );
  mc::FFnorm2<0> norm2;
  mc::FFxlog<1>  myxlog;
  mc::FFVar F[NF] = { xlog( norm2( NX, X) ), myxlog( norm2( NX, X ) ) };
  std::cout << DAG;
  
  std::ofstream o_F( "external1_F.dot", std::ios_base::out );
  DAG.dot_script( NF, F, o_F );
  o_F.close();

  auto F_op  = DAG.subgraph( NF, F );
  DAG.output( F_op );

  // Evaluation in real arithmetic
  double dX[NX] = { 2., 3. }, dF[NF];
  std::vector<double> dwk;
  DAG.eval( F_op, dwk, NF, F, dF, NX, X, dX );
  for( unsigned i=0; i<NF; i++ ) std::cout << "F[" << i << "] = " << dF[i] << std::endl;

  // Evaluation in McCormick arithmetic
  MC mcX[NX] = { MC(I(1.5,2.5),2.), MC(I(2.5,3.5),3.) }, mcF[NF];
  std::vector<MC> mcwk;
  DAG.eval( F_op, mcwk, NF, F, mcF, NX, X, mcX );
  for( unsigned i=0; i<NF; i++ ) std::cout << "F[" << i << "] = " << mcF[i] << std::endl;

  // Forward AD
  const mc::FFVar* dFdX = DAG.FAD( NF, F, NX, X, true );
  std::ofstream o_dFdX( "external1_dFdX.dot", std::ios_base::out );
  DAG.dot_script( NF*NX, dFdX, o_dFdX );
  o_dFdX.close();

  auto dFdX_op  = DAG.subgraph( NF*NX, dFdX );
  DAG.output( dFdX_op );
  delete[] dFdX;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external2()
{
  std::cout << "\n==============================================\ntest_external2:\n";

  mc::FFGraph< mc::FFdet<0> > DAG;
  const unsigned NX = 4, NF = NX*NX;
  mc::FFVar X[NX];
  for( unsigned int i=0; i<NX; i++ ) X[i].set( &DAG );
  mc::FFVar F[NF];
  for( unsigned i=0; i<NX; ++i )
    for( unsigned j=0; j<NX; ++j )
      F[i+j*NX] = pow(X[i],(int)j);
  mc::FFdet<0> det;
  mc::FFVar G = det( NF, F );
  std::cout << DAG;
  
  std::ofstream o_G( "external2_G.dot", std::ios_base::out );
  DAG.dot_script( 1, &G, o_G );
  o_G.close();

  auto G_op  = DAG.subgraph( 1, &G );
  DAG.output( G_op );

  // Evaluation in real arithmetic
  double dX[NX], dG;
  for( unsigned i=0; i<NX; ++i ) dX[i] = i+1.;
  std::vector<double> dwk;
  DAG.eval( G_op, dwk, 1, &G, &dG, NX, X, dX );
  std::cout << "G = " << dG << std::endl;

  // Evaluation in interval arithmetic
  I IX[NX], IG;
  for( unsigned i=0; i<NX; ++i ) IX[i] = 1e-10*I(-1,1)+(double)i+1.;
  std::vector<I> iwk;
  DAG.eval( G_op, iwk, 1, &G, &IG, NX, X, IX );
  std::cout << "G = " << IG << std::endl;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external3()
{
  std::cout << "\n==============================================\ntest_external3:\n";

  mc::FFGraph< mc::FFxlog<0> > DAG;
  mc::FFVar X, Y, F, G;
  X.set( &DAG );
  Y.set( &DAG );
  mc::FFxlog<0> myxlog;
  //F = myxlog(X);
  F = exp(X);
  //G = sqr(Y)+F;
  G = myxlog(Y)+F;
  std::cout << DAG;

  std::ofstream o_comp0( "external3_0.dot", std::ios_base::out );
  DAG.dot_script( 1, &G, o_comp0 );
  o_comp0.close();

  const mc::FFVar* GoF = DAG.compose( 1, &G, 1, &Y, &F );
  std::cout << DAG;

  std::ofstream o_comp1( "external3_1.dot", std::ios_base::out );
  DAG.dot_script( 1, GoF, o_comp1 );
  o_comp1.close();

  delete[] GoF;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external4()
{
  std::cout << "\n==============================================\ntest_external4:\n";

  mc::FFGraph< mc::FFxlog<0> > DAG;
  mc::FFVar X;
  X.set( &DAG );
  mc::FFxlog<0> myxlog;

  mc::FFVar F = myxlog(X);
  std::cout << DAG;
  auto F_op  = DAG.subgraph( 1, &F );
  DAG.output( F_op, " F" );

  // Forward AD
  const mc::FFVar* dFdX = DAG.FAD( 1, &F, 1, &X );
  std::cout << DAG;
  DAG.output( DAG.subgraph( 1, dFdX ), " dFdX" );

  delete[] dFdX;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external5()
{
  std::cout << "\n==============================================\ntest_external5:\n";

  mc::FFGraph< mc::FFxlog<0> > DAG;
  mc::FFVar X;
  X.set( &DAG );
  mc::FFxlog<0> myxlog;

  mc::FFVar F = myxlog(X);
  std::cout << DAG;
  auto F_op  = DAG.subgraph( 1, &F );
  DAG.output( F_op, " F" );

  // Polyhedral relaxation
  mc::PolImg< I, mc::FFxlog<0> > IMG;
  I IX = { I(1,5) };
  POLV PX( &IMG, X, IX ), PF;
  std::vector<POLV> polwk;
  DAG.eval( F_op, polwk, 1, &F, &PF, 1, &X, &PX );
  IMG.generate_cuts( 1, &PF );
  std::cout << "F =" << IMG << std::endl;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external6()
{
  std::cout << "\n==============================================\ntest_external6:\n";

  mc::FFGraph< mc::FFDOpt<0>, mc::FFDOptGrad<0> > DAG;
  const unsigned NP = 4;
  const unsigned NS = mc::FFDOptBase::read( NP, "fims.txt", true ); 
  mc::FFVar S[NS];
  for( unsigned int i=0; i<NS; i++ ) S[i].set( &DAG );
  mc::FFDOpt<0> DOpt;
  mc::FFVar F = DOpt( NS, S );
  //std::cout << DAG;

  // Forward AD
  const mc::FFVar* dFdS = DAG.FAD( 1, &F, NS, S );
  std::cout << DAG;
  auto dFdS_op = DAG.subgraph( NS, dFdS );
  DAG.output( dFdS_op, " dFdS" );

  // Evaluation in real arithmetic
  std::vector<double> dwk;
  double dS[NS];
  for( unsigned i=0; i<NS; ++i ) dS[i] = 1./NS;

  auto F_op = DAG.subgraph( 1, &F );
  //DAG.output( F_op, " dFdS" );
  double dF;
  DAG.eval( F_op, dwk, 1, &F, &dF, NS, S, dS );
  std::cout << "F = " << dF << std::endl;

  double ddFdS[NS];
  DAG.eval( dFdS_op, dwk, NS, dFdS, ddFdS, NS, S, dS );
  for( unsigned i=0; i<NS; ++i ) std::cout << "dFdS[" << i << "] = " << ddFdS[i] << std::endl;

  delete[] dFdS;
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external7()
{
  std::cout << "\n==============================================\ntest_external7:\n";

  // Create DAG
  mc::FFGraph< mc::FFArrh<0> > DAG;
  mc::FFVar X( &DAG );
  mc::FFArrh<0> Arrh;
  double C1(2.), C2(3.);
  //std::cout << "C1: " << &C1 << "  C2: " << &C2 << std::endl;
  mc::FFVar F[2] = { Arrh( X, C1 ) + Arrh( X, C2 ), Arrh( X, C1 ) - Arrh( X, C2 ) };
  std::cout << DAG;

  std::ofstream o_F( "external7_F.dot", std::ios_base::out );
  DAG.dot_script( 2, F, o_F );
  o_F.close();

  auto F_op  = DAG.subgraph( 2, F );
  DAG.output( F_op );
  std::cout << DAG;

  // Evaluation in real arithmetic
  double dX = 2., dF[2];
  DAG.eval( F_op, 2, F, dF, 1, &X, &dX );
  std::cout << "F[0] = " << dF[0] << std::endl;
  std::cout << "F[1] = " << dF[1] << std::endl;

  // Evaluation in McCormick arithmetic
  MC mcX = MC(I(1.5,2.5),2.), mcF[2];
  DAG.eval( F_op, 2, F, mcF, 1, &X, &mcX );
  std::cout << "F[0] = " << mcF[0] << std::endl;
  std::cout << "F[1] = " << mcF[1] << std::endl;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_external8()
{
  std::cout << "\n==============================================\ntest_external8:\n";

  // Create MLP
  mc::MLP<I> f;
  f.options.RELAX     = mc::MLP<I>::Options::AUX;//MCISM;
  f.options.ISMDIV    = 16;
  f.options.ASMBPS    = 8;
  f.options.ISMCONT   = true;
  f.options.ISMSLOPE  = true;
  f.options.ISMSHADOW = false;//true;
  f.options.CUTSHADOW = false;

  //#include "ReLUANN_30L1.hpp"
  #include "ReLUANN_40L4.hpp"
  unsigned l=0;
  for( auto const& layer : MLPCOEF )
    f.append_data( layer, (++l)<MLPCOEF.size()? mc::MLP<I>::Options::RELU:
                                                mc::MLP<I>::Options::LINEAR );

  // Create DAG
  mc::FFGraph< mc::FFMLP<I,0>, mc::FFGRADMLP<I,0> > DAG;
  size_t NX = 2;
  mc::FFVar X[NX];
  for( unsigned int i=0; i<NX; i++ ) X[i].set( &DAG );
  mc::FFMLP<I,0> MLP;
  mc::FFVar F = MLP( 0, NX, X, &f );
  std::cout << DAG;

  std::ofstream o_F( "external8_F.dot", std::ios_base::out );
  DAG.dot_script( 1, &F, o_F );
  o_F.close();

  auto F_op  = DAG.subgraph( 1, &F );
  DAG.output( F_op );
  std::cout << DAG;

  // Evaluation in real arithmetic
  double dX[NX] = { 2., 2. }, dF;
  DAG.eval( F_op, 1, &F, &dF, NX, X, dX );
  std::cout << "F = " << dF << std::endl;

  // Evaluation in McCormick arithmetic
  MC mcX[NX] = { MC(I(-3.,3.),2.), MC(I(-3.,3.),2.) }, mcF;
  DAG.eval( F_op, 1, &F, &mcF, NX, X, mcX );
  std::cout << "F = " << mcF << std::endl;

  // Polyhedral relaxation
  mc::PolImg< I, mc::FFMLP<I,0>, mc::FFGRADMLP<I,0> > IMG;
  IMG.options.BREAKPOINT_TYPE = mc::PolBase<I>::Options::CONT;//BIN;//SOS2;
  IMG.options.AGGREG_LQ       = true;
  IMG.options.BREAKPOINT_RTOL =
  IMG.options.BREAKPOINT_ATOL = 0e0;
  IMG.options.ALLOW_DISJ      = { mc::FFOp::FABS, mc::FFOp::MAXF };
  IMG.options.ALLOW_NLIN      = { mc::FFOp::TANH, mc::FFOp::EXP  };
  I IX[NX] = { I(-3.,3.), I(-3.,3.) };
  POLV polX[NX] = { POLV( &IMG, X[0], IX[0] ), POLV( &IMG, X[1], IX[1] ) }, polF;
  DAG.eval( F_op, 1, &F, &polF, NX, X, polX );
  IMG.generate_cuts( 1, &polF );
  std::cout << "F =" << IMG << std::endl;

  // Evaluation of forward symbolic derivatives in real arithmetic
  const mc::FFVar* dFdX_F = DAG.FAD( 1, &F, NX, X );
  std::ofstream o_dFdX_F( "external8_dFdX_F.dot", std::ios_base::out );
  DAG.dot_script( NX, dFdX_F, o_dFdX_F );
  o_dFdX_F.close();

  auto op_dFdX_F = DAG.subgraph( NX, dFdX_F );
  DAG.output( op_dFdX_F );
  //std::cout << DAG;

  double ddFdX_F[NX];
  DAG.eval( op_dFdX_F, NX, dFdX_F, ddFdX_F, NX, X, dX );
  for( unsigned i=0; i<NX; ++i )
    std::cout << "dFdX_F[" << i << "] = " << ddFdX_F[i] << std::endl;
  delete[] dFdX_F;

  // Evaluation of forward automatic derivatives in real arithmetic
  fadbad::F<double> fdX[NX], fdF;
  for( unsigned i=0; i<NX; ++i ){
    fdX[i] = dX[i];
    fdX[i].diff(i,NX);
  }
  DAG.eval( F_op, 1, &F, &fdF, NX, X, fdX );
  for( unsigned i=0; i<NX; ++i )
    std::cout << "dFdX[" << i << "] = " << fdF.d(i) << std::endl;

  // Evaluation of forward symbolic derivatives in real arithmetic
  const mc::FFVar* dFdX_B = DAG.BAD( 1, &F, NX, X );
  std::ofstream o_dFdX_B( "external8_dFdX_B.dot", std::ios_base::out );
  DAG.dot_script( NX, dFdX_B, o_dFdX_B );
  o_dFdX_B.close();

  auto op_dFdX_B = DAG.subgraph( NX, dFdX_B );
  DAG.output( op_dFdX_B );
  //std::cout << DAG;

  double ddFdX_B[NX];
  DAG.eval( op_dFdX_B, NX, dFdX_B, ddFdX_B, NX, X, dX );
  for( unsigned i=0; i<NX; ++i )
    std::cout << "dFdX_B[" << i << "] = " << ddFdX_B[i] << std::endl;
  delete[] dFdX_B;
/*
  // Evaluation of backward automatic derivatives in real arithmetic
  fadbad::B<double> bdX[NX], bdF;
  for( unsigned i=0; i<NX; ++i )
    bdX[i] = dX[i];
  DAG.eval( F_op, 1, &F, &bdF, NX, X, bdX );
  bdF.diff(0,1);
  for( unsigned i=0; i<NX; ++i )
    std::cout << "dFdX[" << i << "] = " << bdX[i].d(0) << std::endl;
*/
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_slift_external0()
{
  std::cout << "\n==============================================\ntest_slift_external0:\n";

  // Create DAG
  mc::FFGraph< mc::FFnorm2<0>, mc::FFnorm12<1> > DAG;
  const unsigned NX = 2, NF = 3;
  mc::FFVar X[NX];
  for( unsigned int i=0; i<NX; i++ ) X[i].set( &DAG );
  mc::FFnorm2<0> norm2;
  mc::FFnorm12<1> norm12;
  //mc::FFVar F[NF] = { max( X[0], X[1] ), norm12( 0, NX, X ), norm12( 1, NX, X ) };
  mc::FFVar F[NF] = { norm2( NX, X ), norm12( 0, NX, X ), norm12( 1, NX, X ) };
  std::cout << DAG;

  mc::SLiftEnv< mc::FFnorm2<0>, mc::FFnorm12<1> > SPE( &DAG );
  SPE.process( 2, F, true );
  std::cout << SPE;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int test_slift_external1()
{
  std::cout << "\n==============================================\ntest_slift_external1:\n";

  // Create DAG
  mc::FFGraph< mc::FFArrh<0> > DAG;
  mc::FFVar X( &DAG );
  mc::FFArrh<0> Arrh;
  double C1(2.), C2(3.);
  //std::cout << "C1: " << &C1 << "  C2: " << &C2 << std::endl;
  mc::FFVar F = 1 - Arrh( X, C1 ) / Arrh( X, C2 );
  std::cout << DAG;

  mc::SLiftEnv< mc::FFArrh<0> > SPE( &DAG );
  SPE.process( 1, &F, true );
  std::cout << SPE;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
  try{
//    test_external0();
//    test_external1();
//    test_external2();
//    test_external3();
//    test_external4();
//    test_external5();
//    test_external6();
//    test_external7();
//    test_external8();
//    test_slift_external0();
    test_slift_external1();
  }
  catch( mc::FFBase::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in factorable function manipulation:" << std::endl
              << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
#if !defined(MC__USE_PROFIL) && !defined(MC__USE_FILIB) && !defined(MC__USE_BOOST)
  catch( I::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in natural interval extension:" << std::endl
	          << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
#endif
  catch( MC::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in McCormick relaxation:" << std::endl
	          << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
  catch( SCM::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in sparse Chebyshev model arithmetic:" << std::endl
              << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
  catch( CM::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in dense Chebyshev model arithmetic:" << std::endl
              << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
  catch( mc::PolBase<I>::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in polyhedral image arithmetic:" << std::endl
              << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
}

