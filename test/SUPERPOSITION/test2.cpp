#define TEST_EXP	    // <-- select test function here
#define USE_DAG         // <-- specify to evaluate via a DAG of the function
#define SAVE_RESULTS    // <-- specify whether to save results to file
const int NSUB = 8;	    // <-- select partition size here
const int NX = 200;	    // <-- select X discretization here
const int NY = 200;	    // <-- select Y discretization here
////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>

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

#include "ismodel.hpp"
typedef mc::ISModel<I> ISM;
typedef mc::ISVar<I>   ISV;

#ifdef USE_DAG
 #include "ffunc.hpp"
#endif

using namespace std;
using namespace mc;

////////////////////////////////////////////////////////////////////////

#if defined( TEST_MULT )
const double XL   =  1.;	// <-- X range lower bound
const double XU   =  2.;	// <-- X range upper bound
const double YL   =  0.;	// <-- Y range lower bound
const double YU   =  1.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return x*y;
}

#elif defined( TEST_SQRT )
const double XL   =  1.;	// <-- X range lower bound
const double XU   =  2.;	// <-- X range upper bound
const double YL   =  0.;	// <-- Y range lower bound
const double YU   =  1.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return sqrt(x+y);
}

#elif defined( TEST_SQR )
const double XL   =  -2.;	// <-- X range lower bound
const double XU   =  3.;	// <-- X range upper bound
const double YL   =  -3.;	// <-- Y range lower bound
const double YU   =  2.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  using mc::sqr;
  return sqr(x+y);
}

#elif defined( TEST_EXP )
const double XL   =  1.;	// <-- X range lower bound
const double XU   =  2.;	// <-- X range upper bound
const double YL   =  0.;	// <-- Y range lower bound
const double YU   =  1.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  using mc::sqr;
  return x*exp(x+sqr(y))-sqr(y);
}

#elif defined( TEST_TRIG )
const double XL   =  0.;	// <-- X range lower bound
const double XU   =  10.;	// <-- X range upper bound
const double YL   =  0.;	// <-- Y range lower bound
const double YU   =  20.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return exp( sin(x) + sin(y)*cos(y) );
}

#elif defined( TEST_POW )
const double XL   =  -2.;	// <-- X range lower bound
const double XU   =   2.;	// <-- X range upper bound
const double YL   =  -2.;	// <-- Y range lower bound
const double YU   =   2.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return pow(x+y,3);
}

#elif defined( TEST_POW2 )
const double XL   =  -2.;	// <-- X range lower bound
const double XU   =   2.;	// <-- X range upper bound
const double YL   =  -2.;	// <-- Y range lower bound
const double YU   =   2.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return pow(x,3)+pow(y,3);
}

#elif defined( TEST_INV )
const double XL   = -0.6;	// <-- X range lower bound
const double XU   = -0.5;	// <-- X range upper bound
const double YL   = -0.5;	// <-- Y range lower bound
const double YU   = -0.2;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return 1./(x+y);
}

#elif defined( TEST_MAX )
const double XL   =  -2.;	// <-- X range lower bound
const double XU   =   2.;	// <-- X range upper bound
const double YL   =  -2.;	// <-- Y range lower bound
const double YU   =   2.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return max(x+y,0.);
}

#elif defined( TEST_TANH )
const double XL   =  -2.;	// <-- X range lower bound
const double XU   =   2.;	// <-- X range upper bound
const double YL   =  -2.;	// <-- Y range lower bound
const double YU   =   2.;	// <-- Y range upper bound
template <class T>
T myfunc
( const T&x, const T&y )
{
  return tanh(x+y);
}
#endif

////////////////////////////////////////////////////////////////////////

int main()
{
  try{ 

    ISM mod( 2, NSUB );
    mod.options.ASYREM_USE = true;
    
    I IX( XL, XU );
    I IY( YL, YU );
    ISV ISX( &mod, 0, IX );
    ISV ISY( &mod, 1, IY );

    auto IF  = myfunc( IX, IY ); 
    std::cout << "Interval inclusion of f:\n" << IF << std::endl;
  
    auto ISF = myfunc( ISX, ISY );
    std::cout << "Interval superposition model of f:\n" << ISF << std::endl;
    //auto&& mat = ISF.C();

    ///////////////////////////

//    mc::FFGraph DAG;
//    mc::FFVar X( &DAG), Y(&DAG);
//    mc::FFVar F = myfunc( X, Y );

//    // Evaluate in interval arithmetic
//    std::vector<I> IWK;
//    DAG.eval( IWK, 1, &F, &IF, 1, &X, &IX, 1, &Y, &IY );
//    std::cout << "Interval inclusion of f:\n" << IF << std::endl;

//    // Evaluate in interval superposition arithmetic
//    std::vector<ISV> ISWK;
//    DAG.eval( ISWK, 1, &F, &ISF, 1, &X, &ISX, 1, &Y, &ISY );
//    std::cout << "Interval superposition model of f:\n" << ISF << std::endl;

    ///////////////////////////

#ifdef SAVE_RESULTS
    std::ofstream ofile( "test2_ism.out", std::ofstream::out );
    ISF.display( 0, ofile );
    ofile.close();

    // Repeated calculations at grid points (for display)
    std::ofstream ofile2( "test2_fct.out", std::ofstream::out );
    for( int iX=0; iX<NX; iX++ ){
      for( int iY=0; iY<NY; iY++ ){
        double DXY[2] = { XL+iX*(XU-XL)/(NX-1.), YL+iY*(YU-YL)/(NY-1.) };
        double DF = myfunc( DXY[0], DXY[1] );
        I BF = ISF.eval( DXY );
        ofile2 << std::setw(14) << DXY[0] << std::setw(14) << DXY[1] << std::setw(14) << DF
               << std::setw(14) << mc::Op<I>::l(BF) << std::setw(14) << mc::Op<I>::u(BF) 
               << std::endl;
      }
      ofile2 << std::endl;
    }
    ofile2.close();

    // Repeated calculations for different orders
    std::ofstream ofile3( "test2_div.out", std::ofstream::out );
    for( unsigned int q=0; q<16; q++ ){
      ISM mod( 2, pow(2,q) );
      I IX(XL,XU);
      I IY(YL,YU);
      ISV ISX( &mod, 0, IX );
      ISV ISY( &mod, 1, IY );

      auto ISF = myfunc( ISX, ISY );
      ofile3 << std::setw(5) << pow(2,q)
             << std::setw(14) << mc::Op<I>::l(ISF.B())
             << std::setw(14) << mc::Op<I>::u(ISF.B())
             << std::endl;
    }
    ofile3.close();
#endif
  } // end: try
  
#ifndef USE_PROFIL
  catch( I::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in natural interval extension:" << std::endl
              << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }
#endif
  catch( ISM::Exceptions &eObj ){
    std::cerr << "Error " << eObj.ierr()
              << " in interval superposition model:" << std::endl
              << eObj.what() << std::endl
              << "Aborts." << std::endl;
    return eObj.ierr();
  }

  return 0;
}

