// Copyright (C) Benoit Chachuat, Imperial College London.
// All Rights Reserved.
// This code is published under the Eclipse Public License.

/*!
\page page_CHEBYSHEV_SPARSE Sparse Chebyshev Model Arithmetic for Factorable Functions

The classes mc::SCModel and mc::SCVar provide an implementation of sparse Chebyshev model arithmetic. The polynomial part is propagated in floating-point arithmetic, without preallocation to enable sparse storage. In that respect,  The remainder term is propagated in the arithmetic of the templated type, typically a verified interval arithmetic. Rounding errors are not accounted for during the polynomial propagation, so mc::SCModel and mc::SCVar do not provide a fully verified implementation.

\section sec_CHEBYSHEV_SPARSE_use How do I compute a sparse Chebyshev model of a factorable function?

Suppose we want to compute a 4th-order sparse Chebyshev model for the real-valued function \f$f(x,y)=x\exp(y^2)-y^2\f$ with \f$(x,y)\in [1,2]\times[0,1]\f$. For simplicity, bounds on the remainder terms are computed using the default interval type mc::Interval here:

\code
      #include "interval.hpp"
      #include "scmodel.hpp"
      typedef mc::Interval I;
      typedef mc::SCModel<I> SCM;
      typedef mc::SCVar<I> SCV;
\endcode


First, an mc::SCModel object is initialized by passing the order of the polynomial inclusion (4th order here):

\code
      SCM mod( 4 );
\endcode

Next, the variables \f$x\f$ and \f$y\f$ are added to the sparse Chebyshev model:

\code
      SCV X( &mod, 0, I(1.,2.) );
      SCV Y( &mod, 1, I(0.,1.) );
\endcode

Essentially, the first line means that <tt>X</tt> is a variable of class mc::SCVar, participating in the Chebyshev model <tt>mod</tt>, belonging to the interval \f$[1,2]\f$, and having index 0 (C-style indexing in use). The same holds for the variable <tt>Y</tt>, participating in the model <tt>mod</tt>, belonging to the interval \f$[0,1]\f$, and having index 1.

Having defined the variables, a sparse Chebyshev model of \f$f(x,y)=x\exp(x+y^2)-y^2\f$ on \f$[1,2]\times[0,1]\f$ at the mid-point \f$(\frac{3}{2},\frac{1}{2})\f$ is computed as:

\code
      SCV F = X*exp(pow(Y,2))-pow(Y,2);
\endcode

This model can be displayed to the standard output as:

\code
      std::cout << "f sparse Chebyshev model: " << F << std::endl;
\endcode

which produces the following output:

\verbatim
f sparse Chebyshev model: 
 1.9638483e+00   0  1
 7.7961610e-01   1  T1[0]
 7.0133649e-01   1  T1[1]
 4.0044550e-01   2  T1[0]·T1[1]
 3.0580072e-01   2  T2[1]
 1.4360024e-01   3  T1[0]·T2[1]
 8.4292715e-02   3  T3[1]
 2.8097572e-02   4  T1[0]·T3[1]
 1.8401991e-02   4  T4[1]
   R     =  [-1.1124050e-02, 1.1124050e-02]
   B     =  [-5.0886708e-01, 4.4365637e+00]
\endverbatim

The coefficients and corresponding monomials in the Chebyshev model are displayed first, with the middle integer being the total monomial order. The remainder term as well as the Chebyshev model range estimator are reported next.

Other operations involve retreiving the remainder bound, or computing the value of its polynomial part at a given point:

\code
      I B = F.B();
      std::map<unsigned,double> x = { {0,1.5}, {1,0.5} };
      double Pval = F.P( x );
\endcode

See the documentations of mc::SCModel and mc::SCVar for a complete list of member functions. 


\section sec_CHEBYSHEV_SPARSE_fct Which functions are overloaded for sparse Chebyshev model arithmetic?

mc::SCVar overloads the usual functions <tt>exp</tt>, <tt>log</tt>, <tt>sqr</tt>, <tt>sqrt</tt>, <tt>pow</tt>, <tt>inv</tt>, <tt>cos</tt>, <tt>sin</tt>, <tt>tan</tt>, <tt>acos</tt>, <tt>asin</tt>, <tt>atan</tt>, <tt>cosh</tt>, <tt>sinh</tt>, <tt>tanh</tt>, <tt>erf</tt>, <tt>erfc</tt>, <tt>fabs</tt>. The functions <tt>min</tt>, <tt>max</tt>, <tt>fstep</tt>, and <tt>bstep</tt> are not currently overloaded in mc::SCVar. Moreover, mc::SCVar defines the following functions:
- <tt>inter(x,y,z)</tt>, computing a Chebyshev model of the intersection \f$x = y\cap z\f$ of two Chebyshev models and returning true/false if the intersection is nonempty/empty. With Chebyshev models \f$\mathcal P_y\oplus\mathcal R_y\f$ and \f$\mathcal P_z\oplus\mathcal R_z\f$, this intersection is computed as follows:
\f{align*}
  \mathcal P_{x} =\ & (1-\eta) \mathcal P_y^{\rm C} + \eta \mathcal P_z^{\rm C}\\
  \mathcal R_{x} =\ & [\mathcal R_y^{\rm C}\oplus\eta\mathcal{B}(\mathcal P_y^{\rm C}-\mathcal P_z^{\rm C})] \cap [\mathcal R_z^{\rm C}\oplus (1-\eta)\mathcal{B}(\mathcal P_z^{\rm C}-\mathcal P_y^{\rm C})]\,.
\f}
with \f$\mathcal{B}(\cdot)\f$ the Chebyshev model range bounder, and \f$\eta\f$ a real scalar in \f$[0,1]\f$. Choosing \f$\eta=1\f$ amounts to setting the polynomial part \f$\mathcal P_{x}\f$ as \f$\mathcal P_y\f$, whereas \f$\eta=0\f$ sets \f$\mathcal P_{x}\f$ as \f$\mathcal P_z\f$. The parameter \f$\eta\f$ can be defined in mc::CModel::Options::REF_POLY.
- <tt>hull(x,y)</tt>, computing a Chebyshev model of the union \f$x = y\cup z\f$ of two Chebyshev models. With Chebyshev models \f$\mathcal P_y\oplus\mathcal R_y\f$ and \f$\mathcal P_z\oplus\mathcal R_z\f$, this union is computed as follows:
\f{align*}
  \mathcal P_{x} =\ & (1-\eta) \mathcal P_y^{\rm C} + \eta \mathcal P_z^{\rm C}\\
  \mathcal R_{x} =\ & {\rm hull}\{\mathcal R_y^{\rm C}\oplus\eta\mathcal{B}(\mathcal P_y^{\rm C}-\mathcal P_z^{\rm C}), \mathcal R_z^{\rm C}\oplus (1-\eta)\mathcal{B}(\mathcal P_z^{\rm C}-\mathcal P_y^{\rm C})\}\,.
\f}
with \f$\mathcal{B}(\cdot)\f$ and \f$\eta\f$ as previously.
.

\section sec_CHEBYSHEV_SPARSE_opt How are the options set for the computation of a sparse Chebyshev model?

The class mc::SCModel has a public member called mc::SCModel::options that can be used to set/modify the options. For instance:

\code
      mod.options.BOUNDER_TYPE = SCM::Options::NAIVE;
      mod.options.SCALE_VARIABLES = true;
\endcode

The available options are the following:

<TABLE border="1">
<CAPTION><EM>Options in mc::SCModel::Options: name, type and description</EM></CAPTION>
     <TR><TH><b>Name</b>  <TD><b>Type</b><TD><b>Default</b>
         <TD><b>Description</b>
     <TR><TH><tt>BASIS</tt> <TD><tt>unsigned int</tt> <TD>1
         <TD>Basis representation of the monomials: 0-Monomial basis; 1-Chebyshev basis.
     <TR><TH><tt>REMEZ_USE</tt> <TD><tt>bool</tt> <TD>false
         <TD>Whether to use the Remez algorithm for computing a minimax approximation for univariate terms.
     <TR><TH><tt>REMEZ_MAXIT</tt> <TD><tt>unsigned int</tt> <TD>10
         <TD>Maximal number of iterations in Remez algorithm for computing a minimax approximation for univariate terms.
     <TR><TH><tt>REMEZ_TOL</tt> <TD><tt>double</tt> <TD>1e-5
         <TD>Stopping tolerance in Remez algorithm for computing a minimax approximation for univariate terms.
     <TR><TH><tt>REMEZ_MIG</tt> <TD><tt>double</tt> <TD>1e-10
         <TD>Threshold for interval width below which Remez algorithm is not useds.
     <TR><TH><tt>INTERP_EXTRA</tt> <TD><tt>unsigned int</tt> <TD>0
         <TD>Extra terms in Chebyshev interpolation of univariates: 0-Chebyshev interpolation of order NORD; extra terms allow approximation of Chebyshev truncated series.
     <TR><TH><tt>INTERP_THRES</tt> <TD><tt>double</tt> <TD>1e2*machprec()
         <TD>Threshold for coefficient values in Chebyshev expansion for bounding of transcendental univariates.
     <TR><TH><tt>BOUNDER_TYPE</tt> <TD><tt>mc::SCModel::Options::BOUNDER</tt> <TD>mc::SCModel::Options::LSB
         <TD>Chebyshev model range bounder.
     <TR><TH><tt>MIXED_IA</tt> <TD><tt>bool</tt> <TD>false
         <TD>Whether to intersect internal bounds with underlying bounds in the templated arithmetics.
     <TR><TH><tt>MIN_FACTOR</tt> <TD><tt>double</tt> <TD>0e0
         <TD>Threshold for monomial coefficients below which the term is removed and appended to the remainder term.
     <TR><TH><tt>REF_POLY</tt> <TD><tt>double</tt> <TD>0.
         <TD>Scalar in \f$[0,1]\f$ related to the choice of the polynomial part in the overloaded functions mc::inter and mc::hull (see \ref sec_CHEBYSHEV_SPARSE_fct). A value of 0. amounts to selecting the polynomial part of the left operand, whereas a value of 1. selects the right operand.
     <TR><TH><tt>DISPLAY_DIGITS</tt> <TD><tt>unsigned int</tt> <TD>5
         <TD>Number of digits in output stream for Chebyshev model coefficients.
</TABLE>


\section sec_CHEBYSHEV_SPARSE_err Errors What errors can I encounter during computation of a sparse Chebyshev model?

Errors are managed based on the exception handling mechanism of the C++ language. Each time an error is encountered, a class object of type mc::SCModel::Exceptions is thrown, which contains the type of error. Possible errors encountered during the computation of a sparse Chebyshev model are:

<TABLE border="1">
<CAPTION><EM>Errors from class mc::SCModel::Exceptions during the Computation of a Sparse Chebyshev Model</EM></CAPTION>
     <TR><TH><b>Number</b> <TD><b>Description</b>
     <TR><TH><tt>1</tt> <TD>Division by zero scalar
     <TR><TH><tt>2</tt> <TD>Inverse operation with zero in range
     <TR><TH><tt>3</tt> <TD>Log operation with non-positive numbers in range
     <TR><TH><tt>4</tt> <TD>Square-root operation with negative numbers in range
     <TR><TH><tt>5</tt> <TD>Real power operation with negative numbers in range
     <TR><TH><tt>6</tt> <TD>Tangent operation with (k+1/2)·PI in range
     <TR><TH><tt>7</tt> <TD>Cosine inverse operation with range outside [-1,1]
     <TR><TH><tt>8</tt> <TD>Sine inverse operation with range outside [-1,1]
     <TR><TH><tt>9</tt> <TD>Failed to compose sparse Chebyshev variable
     <TR><TH><tt>-1</tt> <TD>Failed to construct sparse Chebyshev variable
     <TR><TH><tt>-2</tt> <TD>Inconsistent bounds with template parameter arithmetic
     <TR><TH><tt>-3</tt> <TD>Operation between Chebyshev variables linked to different Chebyshev models
     <TR><TH><tt>-4</tt> <TD>Internal error
     <TR><TH><tt>-33</tt> <TD>Feature not yet implemented in mc::SCModel
</TABLE>

Further exceptions may be thrown by the template parameter class itself.
*/

#ifndef MC__SCMODEL_H
#define MC__SCMODEL_H

#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stdarg.h>
#include <cassert>
#include <climits>
#include <limits>
#include <stdlib.h>
#include <complex>
#include <numeric>

#include "mcfunc.hpp"
#include "mclapack.hpp"
#include "mcop.hpp"
#include "smon.hpp"
#include "remez.hpp"

#undef  MC__SCMODEL_DEBUG
#undef  MC__SCMODEL_DEBUG_SCALE
#define MC__SCMODEL_CHECK
#undef  MC__SCMODEL_CHECK_PMODEL
#undef  MC__SCVAR_DEBUG_EXP
#undef  MC__SCVAR_DEBUG_BERNSTEIN
#define MC__SCMODEL_USE_PROD
#undef  MC__SCMODEL_DEBUG_MINIMAX

//#undef MC__SCVAR_FABS_SQRT
//#undef MC__SCVAR_FORCE_REM_DERIV
//#undef MC__SCVAR_SPARSE_PRODUCT_FULL

namespace mc
{

template <typename T, typename KEY, typename COMP> class SCVar;

//! @brief C++ class for the computation of sparse Chebyshev models for factorable function: environment
////////////////////////////////////////////////////////////////////////
//! mc::SCModel is a C++ class for definition of Chebyshev model
//! environment in sparse format. Propagation of sparse Chebyshev models
//! for factorable functions is via the C++ class mc::SCVar. The
//! template parameter corresponds to the type used to propagate the
//! remainder bound.
////////////////////////////////////////////////////////////////////////
template < typename T, typename KEY=unsigned, typename COMP=std::less<unsigned> >
class SCModel
////////////////////////////////////////////////////////////////////////
{
  friend class SCVar<T,KEY,COMP>;
  template <typename U, typename K, typename C> friend class SCModel;
  template <typename U, typename K, typename C> friend SCVar<U,K,C> inv
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sqr
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sqrt
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> exp
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> log
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> xlog
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( const SCVar<U,K,C>&, const int );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( const SCVar<U,K,C>&, double const& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( double const, const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( const SCVar<U,K,C>&, const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> prod
    ( const unsigned int, const SCVar<U,K,C>* );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> monom
    ( const unsigned int, const SCVar<U,K,C>*, const unsigned* );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> cheb
    ( const SCVar<U,K,C>&, const unsigned );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> cos
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sin
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> tan
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> acos
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> asin
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> atan
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> cosh
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sinh
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> tanh
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> erf
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> erfc
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> fabs
    ( const SCVar<U,K,C>& );

public:

  // Monomial representation: SMon := <total order, <variable, order>>
  typedef SMon<KEY,COMP> t_mon;
  typedef lt_SMon<COMP> lt_mon;
  typedef std::map< t_mon, double, lt_mon > t_poly;
  typedef std::set< KEY, COMP > t_var;

protected:

  //! @brief Max order of polynomial model
  unsigned _maxord;

  //! @brief Set of participating variables
  t_var _setvar;

  //! @brief Set of auxiliary (lifted) variables
  t_var _setaux;

  //! @brief Bounds on model variables
  std::map<KEY,T,COMP> _bndvar;

  //! @brief Reference points for the model variables
  std::map<KEY,double,COMP> _refvar;

  //! @brief Scaling factors of the model variables
  std::map<KEY,double,COMP> _scalvar; 

  //! @brief Coefficients in Chebyshev interpolant of univariate functions
  std::vector<double> _coefuniv;

  //! @brief Points in Chebyshev interpolant of univariate functions
  std::vector<double> _xinterp;
      
  //! @brief Function values in Chebyshev interpolant of univariate functions
  std::vector<double> _finterp;

  //! @brief Resize the model variable data containers
  void _set
    ( KEY const& id, T const& X );

//  //! @brief Resize and return a pointer to the Chebyshev coefficient interpolant array
//  std::vector<double>& _resize_coefuniv
//    ()
//    { _coefuniv.resize( _maxord + options.INTERP_EXTRA + 1 );
//      return _coefuniv; }

  //! @brief Overloads returning a pointer of a new KEY
  template<typename K>
  static K* _newaux
   ( K* dum, int ndx )
   { return new K( ndx ); }
   //{ K* aux = new K( ndx ); std::cout << "AUX: " << aux << std::endl; return aux; }
  template<typename K>
  static K _newaux
   ( K& dum, int ndx )
   { return K( ndx ); }

  //! @brief Overloads deleting a KEY
  template<typename K>
  static void _delaux
   ( K* aux )
   { delete aux; }
   //{ std::cout << "AUX: " << aux << std::endl; delete aux; }
  template<typename K>
  static void _delaux
   ( K& aux )
   {}

public:
  /** @addtogroup SCHEBYSHEV Sparse Chebyshev Model Arithmetic for Factorable Functions
   *  @{
   */
  //! @brief Constructor of Sparse Chebyshev model environment for maximal order <tt>maxord</tt>
  SCModel
    ( const unsigned maxord=3 )
    : _maxord( maxord )
    { _coefuniv.reserve( _maxord + options.INTERP_EXTRA + 1 ); }

  //! @brief Destructor of Sparse Chebyshev model environment
  ~SCModel
    ()
    { reset_aux(); }

  //! @brief Append new auxiliary variable and return a reference
   KEY const& append_aux
     ();

  //! @brief Maximal order of polynomial model
  void reset_aux
    ();

  //! @brief Maximal order of polynomial model
  unsigned maxord
    ()
    const
    { return _maxord; };

  //! @brief Retrieve auxiliary variables
  t_var const& setaux
    ()
    const
    { return _setaux; }

  //! @brief Retrieve model variables
  t_var const& setvar
    ()
    const
    { return _setvar; }

  //! @brief Retrieve bounds of the model variables
  std::map<KEY,T,COMP> const& bndvar
    ()
    const
    { return _bndvar; }

  //! @brief Retrieve reference points for the model variables
  std::map<KEY,double,COMP> const& refvar
    ()
    const
    { return _refvar; }

  //! @brief Retrieve scaling factors of the model variables
  std::map<KEY,double,COMP> const& scalvar
    ()
    const
    { return _scalvar; }

  //! @brief Get Chebyshev basis functions in U arithmetic for variable bounds <a>bndvar</a>
  template <typename U>
  std::map<KEY,std::vector<U>,COMP>& get_basis
    ( const unsigned maxord, std::map<KEY,U,COMP> const& bndvar,
      std::map<KEY,std::vector<U>,COMP>& bndbasis, const bool scaled=false )
    const;

  //! @brief Get Chebyshev monomial bounds in U arithmetic for variable bounds <a>bndvar</a>
  template <typename U>
  void get_bndmon
    ( std::map<t_mon,U,lt_mon>& bndmon, std::map<KEY,U,COMP> const& bndvar,
      bool const scaled=false, bool const useprod=false )
    const;

  //! @brief Exceptions of mc::SCModel
  class Exceptions
  {
  public:
    //! @brief Enumeration type for mc::SCModel exception handling
    enum TYPE{
      DIV=1,	//!< Division by zero scalar
      INV,	//!< Inverse operation with zero in range
      LOG,	//!< Log operation with non-positive numbers in range
      SQRT,	//!< Square-root operation with negative numbers in range
      DPOW,    //!< Real power operation with negative numbers in range
      TAN,	//!< Tangent operation with (k+1/2)·PI in range
      ACOS,	//!< Cosine inverse operation with range outside [-1,1]
      ASIN,	//!< Sine inverse operation with range outside [-1,1]
      COMPOSE,	//!< Failed to compose Chebyshev variable
      INIT=-1,	//!< Failed to construct Chebyshev variable
      INCON=-2, //!< Inconsistent bounds with template parameter arithmetic
      MODEL=-3,//!< Operation between variables linked to different models
      INTERNAL = -4,//!< Internal error
      UNDEF=-33 //!< Feature not yet implemented
    };
    //! @brief Constructor for error <a>ierr</a>
    Exceptions( TYPE ierr ) : _ierr( ierr ){}
    //! @brief Error flag
    int ierr(){ return _ierr; }
    //! @brief Error description
    std::string what(){
      switch( _ierr ){
      case DIV:
        return "mc::SCModel\t Division by zero scalar";
      case INV:
        return "mc::SCModel\t Inverse operation with zero in range";
      case LOG:
        return "mc::SCModel\t Log operation with non-positive numbers in range";
      case SQRT:
        return "mc::SCModel\t Square-root operation with negative numbers in range";
      case DPOW:
        return "mc::SCModel\t Real power operation with negative numbers in range";
      case TAN:
        return "mc::SCModel\t Tangent operation with (k+1/2)·PI in range";
      case ACOS:
        return "mc::SCModel\t Cosine inverse operation with range outside [-1,1]";
      case ASIN:
        return "mc::SCModel\t Sine inverse operation with range outside [-1,1]";
      case COMPOSE:
        return "mc::SCModel\t Chebyshev composition failed";
      case INIT:
        return "mc::SCModel\t Chebyshev variable initialization failed";
      case INCON:
        return "mc::SCModel\t Inconsistent bounds with template parameter arithmetic";
      case MODEL:
        return "mc::SCModel\t Operation between variables in different model environments";
      case UNDEF:
        return "mc::SCModel\t Feature not yet implemented";
      case INTERNAL:
      default:
        return "mc::SCModel\t Internal error";
      }
    }

  private:
    TYPE _ierr;
  };

  //! @brief Options of mc::SCModel
  struct Options
  {
    //! @brief Constructor of mc::SCModel::Options
    Options():
      BASIS(CHEB),
      LIFT_USE(false), LIFT_ATOL(1e-10), LIFT_RTOL(1e-3),
      REMEZ_USE(true), REMEZ_MAXIT(10), REMEZ_TOL(1e-5), REMEZ_MIG(1e-10),
      INTERP_EXTRA(0), INTERP_THRES(1e2*machprec()), BOUNDER_TYPE(LSB),
      MIG_USE(false), MIG_ATOL(0e0), MIG_RTOL(machprec()), 
      MIXED_IA(false), REF_POLY(0e0), DISPLAY_DIGITS(7)
      {}
    //! @brief Copy constructor of mc::SCModel::Options
    template <typename U> Options
      ( U const& options )
      : BASIS( options.BASIS ),
        LIFT_USE( options.LIFT_USE ),
        LIFT_ATOL( options.LIFT_ATOL ),
        LIFT_RTOL( options.LIFT_RTOL ),
        REMEZ_MAXIT( options.REMEZ_MAXIT ),
        REMEZ_TOL( options.REMEZ_TOL ),
        REMEZ_MIG( options.REMEZ_MIG ),
        INTERP_EXTRA( options.INTERP_EXTRA ),
        INTERP_THRES( options.INTERP_THRES ),
        BOUNDER_TYPE( options.BOUNDER_TYPE ),
        MIG_USE( options.MIG_USE ),
        MIG_ATOL( options.MIG_ATOL ),
        MIG_RTOL( options.MIG_RTOL ),
        MIXED_IA( options.MIXED_IA ),
        REF_POLY(options.REF_POLY),
        DISPLAY_DIGITS( options.DISPLAY_DIGITS )
      {}
    //! @brief Assignment of mc::SCModel::Options
    template <typename U> Options& operator=
      ( U const& options ){
        BASIS            = options.BASIS;
        LIFT_USE         = options.LIFT_USE;
        LIFT_ATOL        = options.LIFT_ATOL;
        LIFT_RTOL        = options.LIFT_RTOL;
        REMEZ_USE        = options.REMEZ_USE;
        REMEZ_MAXIT      = options.REMEZ_MAXIT;
        REMEZ_TOL        = options.REMEZ_TOL;
        REMEZ_MIG        = options.REMEZ_MIG;
        INTERP_EXTRA     = options.INTERP_EXTRA;
        INTERP_THRES     = options.INTERP_THRES;
        BOUNDER_TYPE     = (BOUNDER)options.BOUNDER_TYPE;
        MIG_USE          = options.MIG_USE;
        MIG_ATOL         = options.MIG_ATOL;
        MIG_RTOL         = options.MIG_RTOL;
        MIXED_IA         = options.MIXED_IA;
        REF_POLY         = options.REF_POLY;
        DISPLAY_DIGITS   = options.DISPLAY_DIGITS;
        return *this;
      }
    //! @brief Chebyshev model range bounder option
    enum BOUNDER{
      NAIVE=0,	//!< Naive polynomial range bounder
      LSB	//!< Lin & Stadtherr range bounder
    };
    //! @brief Available basis representations
    enum MONBASIS{
      MONOM=0,	//!< Monomial basis
      CHEB	//!< Chebyshev basis
    };
    //! @brief Basis representation of the monomials
    unsigned BASIS;
    //! @brief Whether to lift the remainder term in nonlinear operations by introducing auxiliary variables in the model
    bool LIFT_USE;
    //! @brief Absolute tolerance for lifting the reminder term - only if LIFT_USE == true
    double LIFT_ATOL;
    //! @brief relative tolerance for lifting the reminder term - only if LIFT_USE == true
    double LIFT_RTOL;
    //! @brief Whether to use the Remez algorithm for computing a minimax approximation for univariate terms
    bool REMEZ_USE;
    //! @brief Maximal number of iterations in Remez algorithm for computing a minimax approximation for univariate terms
    unsigned REMEZ_MAXIT;
    //! @brief Stopping tolerance in Remez algorithm for computing a minimax approximation for univariate terms
    double REMEZ_TOL;
    //! @brief Threshold for interval width below which Remez algorithm is not used
    double REMEZ_MIG;
    //! @brief Extra terms in chebyshev interpolation of univariates: 0-Chebyshev interpolation of order MAXORD; extra terms allow approximation of Chebyshev truncated series
    unsigned INTERP_EXTRA;
    //! @brief Threshold for coefficient values in Chebyshev expansion for bounding of transcendental univariates
    double INTERP_THRES;
    //! @brief Chebyshev model range bounder - See \ref sec_CHEBYSHEV_SPARSE_opt
    BOUNDER BOUNDER_TYPE;
    //! @brief Array of Chebyshev model range bounder names (for display)
    static const std::string BOUNDER_NAME[2];
    //! @brief Whether to simplify the monomial terms with small magnitude in the model
    bool MIG_USE;
    //! @brief Absolute tolerance for simplifying monomial terms - only if MIG_USE == true
    double MIG_ATOL;
    //! @brief relative tolerance for simplifying monomial terms - only if MIG_USE == true
    double MIG_RTOL;
    //! @brief Whether to intersect internal bounds with underlying bounds in T arithmetics
    bool MIXED_IA;
    //! @brief Scalar in \f$[0,1]\f$ related to the choice of the polynomial part in the overloaded functions mc::inter and mc::hull (see \ref sec_CHEBYSHEV_SPARSE_fct). A value of 0. amounts to selecting the polynomial part of the left operand, whereas a value of 1. selects the right operand.
    double REF_POLY;
    //! @brief Number of digits in output stream for Chebyshev model coefficients.
    unsigned DISPLAY_DIGITS;
  } options;
  
  //!brief Unit ball in T arithmetic
  static T TOne;
  
  //!brief Zero-one in T arithmetic
  static T TZerOne;
  /** @} */

private:

  //! @brief Get Chebyshev basis functions in U arithmetic for variable <a>bndvar</a>
  template <typename U>
  std::vector<U>& _get_bndpow
    ( unsigned const maxord, U const& bndvar, double const& refvar, double const& scalvar,
      std::vector<U>& bndpow )
    const;

  //! @brief Get Chebyshev basis functions in U arithmetic for [-1,1] scaled variable <a>bndvar</a>
  template <typename U>
  std::vector<U>& _get_bndpow
    ( unsigned const maxord, U const& bndvar, std::vector<U>& bndpow )
    const;

  //! @brief Conversion from power series to Chebyshev series
  template <typename VEC>
  VEC _to_chebyshev
    ( VEC const& veccoef )
    const;

  //! @brief Construct minimax polynomial coefficient for univariate <a>f</a> in <a>_coefuniv</a> and return corresponding error for univariate <a>f</a> using Remez algorithm
  template <typename PUNIV>
  double _minimax
    ( PUNIV const& f );

  //! @brief Compose minimax polynomial for univariate <a>f</a> with Chebyshev variable <a>CV</a> and return resulting Chebyshev variable in <a>CV2</a>
  template <typename PUNIV>
  bool _minimax
    ( PUNIV const& f, SCVar<T,KEY,COMP> const& CV, SCVar<T,KEY,COMP>& CV2 );

  //! @brief Return error at bound between univariate <a>f</a> and corresponding interpolating coefficients in <a>_coefuniv</a>
  template <typename PUNIV>
  double _rematbound
    ( PUNIV const& f );

  //! @brief Construct interpolating polynomial coefficient for univariate <a>f</a> in <a>_coefuniv</a> up to order <a>maxord</a>
  template <typename PUNIV>
  void _chebinterp
    ( PUNIV const& f, unsigned const maxord );

  //! @brief Construct interpolating polynomial coefficient for univariate <a>f</a> in <a>_coefuniv</a> until coefficient magnitude is less than <a>tol</a>
  template <typename PUNIV>
  void _chebinterp
    ( PUNIV const& f, double const& tol, unsigned& maxord );

  //! @brief Apply the Clenshaw algorithm
  SCVar<T,KEY,COMP> _clenshaw
    ( SCVar<T,KEY,COMP> const& CVinner, unsigned const maxord );

  //! @brief Apply the Horner algorithm
  SCVar<T,KEY,COMP> _horner
    ( SCVar<T,KEY,COMP> const& CVinner, unsigned const maxord );

  //! @brief Compose interpolating polynomial for univariate <a>f</a> with Chebyshev variable <a>CV</a> and return resulting Chebyshev variable in <a>CV2</a>
  template <typename PUNIV>
  bool _chebinterp
    ( PUNIV const& f, SCVar<T,KEY,COMP> const& CV, SCVar<T,KEY,COMP>& CV2,
      bool const rematbound );

  //! @brief Apply Chebyshev composition to variable <a>CVI</a> using the coefficients <a>coefmon</a> of the outer function
  template <typename U> static SCVar<T,KEY,COMP> _composition
    ( std::vector<U> const& coefouter, unsigned const maxord,
      SCVar<T,KEY,COMP> const& CVinner );

  //! @brief Recursive calculation of nonnegative integer powers
  SCVar<T,KEY,COMP> _intpow
    ( SCVar<T,KEY,COMP> const& CV, int const n )
    const;

  //! @brief Polynomial range bounder - Naive approach
  template <typename C, typename U> U _polybound_naive
    ( std::map<t_mon,C,lt_mon> const& coefmon,
      std::map<KEY,std::vector<U>,COMP> const& bndbasis,
      unsigned const minord=0 )
    const;

  //! @brief Polynomial range bounder - Lin & Stadtherr approach
  template <typename C, typename U> U _polybound_LSB
    ( std::map<t_mon,C,lt_mon> const& coefmon, 
      std::map<KEY,std::vector<U>,COMP> const& bndbasis )
    const;

  //! @brief Polynomial range bounder using specified bounder <a>type</a> with basis functions <a>bndbasis</a> in U arithmetic and monomial coefficients <a>coefmon</a> in C arithmetic
  template <typename C, typename U> U _polybound
    ( std::map<t_mon,C,lt_mon> const& coefmon,
      std::map<KEY,std::vector<U>,COMP> const& bndbasis,
      int const type )
    const;

  //! @brief Monomial range bounder for <a>mon</a>
  T _monbound
    ( t_mon const& mon )
    const;

  //! @brief Monomial value of <a>mon</a> at point <a>x</a>
  double _monval
    ( t_mon const& mon, std::map<KEY,double,COMP> const& x )
    const;

  //! @brief Recursive product of univariate Chebyshev polynomials
  void _sprod1D
    ( std::map<unsigned,t_poly> const& sp1map,
      std::map<unsigned,t_poly> const& sp2map,
      t_poly& coefmon, T& rem, t_var const& ndxvar,
      typename t_var::const_iterator itvar )
    const;

  //! @brief Scaling of Chebyshev coefficient maps
  void _sscal1D
    ( const t_poly& coefmon0, double const&coefscal,
      t_poly& coefmon )
    const;

  //! @brief Lifting of Chebyshev coefficient maps
  void _slift1D
    ( const t_poly& coefmon0, double const& dscal,
      t_poly& coefmon )
    const;

  //! @brief Lifting of Chebyshev coefficient maps
  void _slift1D
    ( t_poly const& coefmon0, double const& dscal,
      t_poly& coefmon, T& rem,
      typename t_var::const_iterator itvar, unsigned const ndxord )
    const;

  //! @brief Lifting of Chebyshev coefficient maps
  void _slift1D
    ( const t_poly& coefmon0, double const& dscal,
      T& rem, unsigned const ndxor )
    const;

  //! @brief Build 1D vector of Chebyshev coefficients
  void _svec1D
    ( typename t_var::const_iterator itvar,
      std::pair<t_mon,double> const& mon,
      std::map<unsigned,t_poly>& mapspoly )
    const;

  //! @brief Display of recursive univariate Chebyshev polynomials
  void _sdisp1D
    ( std::map<unsigned,t_poly> const& coefmon, typename t_var::const_iterator itvar,
      const std::string&name="", std::ostream&os=std::cout )
    const;

  //! @brief Display of recursive univariate Chebyshev polynomials
  void _sdisp1D
    ( const t_poly& coefmon, const std::string&name="",
      std::ostream&os=std::cout )
    const;

  //! @brief Build 1D vector of Chebyshev coefficients
  void _svec1Dfull
    ( typename t_var::const_iterator itvar,
      std::pair<t_mon,double> const& mon,
      std::vector< SCVar<T,KEY,COMP> >& vec )
    const;

  //! @brief Display of recursive univariate Chebyshev polynomials
  void _sdisp1Dfull
    ( std::vector< SCVar<T,KEY,COMP> > const& vec, typename t_var::const_iterator itvar,
      std::string const& name="", std::ostream& os=std::cout )
    const;

  //! @brief Product of multivariate Chebyshev polynomials in sparse format
  void _sprod
    ( SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2, t_poly& coefmon,
      double&coefrem )
    const;
};

template <typename T, typename KEY, typename COMP>
inline
const std::string SCModel<T,KEY,COMP>::Options::BOUNDER_NAME[2]
  = { "NAIVE", "LSB" };

template <typename T, typename KEY, typename COMP>
inline
T SCModel<T,KEY,COMP>::TZerOne
  = Op<T>::zeroone();

template <typename T, typename KEY, typename COMP>
inline
T SCModel<T,KEY,COMP>::TOne
  = 2.*Op<T>::zeroone()-1.;

//! @brief C++ class for the computation of sparse Chebyshev models for factorable function: propagation
////////////////////////////////////////////////////////////////////////
//! mc::SCVar is a C++ class for propagation of sparse Chebyshev models
//! through factorable functions. Variables of type mc::SCVar are
//! registered in a Chebyshev model environment of class mc::SCModel.
//! The template parameter corresponds to the type used in computing the
//! remainder bound.
////////////////////////////////////////////////////////////////////////
template < typename T, typename KEY=unsigned, typename COMP=std::less<unsigned> >
class SCVar
////////////////////////////////////////////////////////////////////////
{
  template <typename U, typename K, typename C> friend class SCVar;
  template <typename U, typename K, typename C> friend class SCModel;

  template <typename U, typename K, typename C> friend SCVar<U,K,C> operator-
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> operator*
    ( const SCVar<U,K,C>&, const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend std::ostream& operator<<
    ( std::ostream&, const SCVar<U,K,C>& );

  template <typename U, typename K, typename C> friend U funcptr
    ( const U, const unsigned  );
  template <typename U, typename K, typename C> friend void interpolation
    ( double*, const SCVar<U,K,C>&, const unsigned  );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> composition
    ( double const*, const SCVar<U,K,C>& );

  template <typename U, typename K, typename C> friend SCVar<U,K,C> inv
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sqr
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sqrt
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> exp
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> log
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> xlog
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( const SCVar<U,K,C>&, const int );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( const SCVar<U,K,C>&, double const& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( double const, const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> pow
    ( const SCVar<U,K,C>&, const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> prod
    ( const unsigned int, const SCVar<U,K,C>* );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> monom
    ( const unsigned int, const SCVar<U,K,C>*, const unsigned* );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> cheb
    ( const SCVar<U,K,C>&, const unsigned );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> cos
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sin
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> tan
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> acos
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> asin
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> atan
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> cosh
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> sinh
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> tanh
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> erf
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> erfc
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> fabs
    ( const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend SCVar<U,K,C> hull
    ( const SCVar<U,K,C>&, const SCVar<U,K,C>& );
  template <typename U, typename K, typename C> friend bool inter
    ( SCVar<U,K,C>&, const SCVar<U,K,C>&, const SCVar<U,K,C>& );

public:

  // Monomial representation: SMon := <total order, <variable, order>>
  typedef SMon<KEY,COMP> t_mon;
  typedef lt_SMon<COMP> lt_mon;
  typedef std::map< t_mon, double, lt_mon > t_poly;
  typedef std::set< KEY, COMP > t_var;
  
private:

  //! @brief Pointer to Chebyshev model environment
  SCModel<T,KEY,COMP> *_CM;

  //! @brief Set of participating variables in polynomial expression
  t_var _ndxvar;

  //! @brief Array of size <tt>_nmon</tt> with monomial coefficients of variable
  t_poly _coefmon;

  //! @brief Remainder bound of variable
  T _bndrem;

  //! @brief Pointer to variable bound in underlying T arithmetic (possibly NULL if not computed)
  mutable T* _bndT;

  //! @brief Pointer to polynomial bound of variable (possibly NULL if not available)
  mutable T* _bndpol;

  //! @brief Initialize private/protected members of model variable
  void _init
    ();

  //! @brief Reinitialize private/protected members of model variable
  void _reinit
    ();

  //! @brief Clean up private/protected members of variable
  void _cleanup
    ();

  //! @brief Center remainder error term <tt>_bndrem</tt>
  void _center();

  //! @brief Set variable bound in unerlying T arithmetic
  void _set_bndT
    ( T const& bndT );

  //! @brief Set variable bound in unerlying T arithmetic
  void _set_bndT
    ( T const* bndT );

  //! @brief Unset variable bound in underlying T arithmetic
  void _unset_bndT
    ();

  //! @brief Set polynomial bound in variable as <tt>bndpol</tt>
  void _set_bndpol
    ( T const& bndpol );

  //! @brief Set polynomial bound in variable as <tt>bndpol</tt>
  void _set_bndpol
    ( T const* bndpol );

  //! @brief Unset polynomial bound in variable
  void _unset_bndpol
    ();

  //! @brief Original bounds on variable <tt>id</tt>
  T const& _bndvar
    ( KEY const& id )
    const
    { return _CM->_bndvar[id]; };

  //! @brief Reference point for variable <tt>id</tt> in Chebyshev model
  double const& _refvar
    ( KEY const& id )
    const
    { return _CM->_refvar[id]; };

  //! @brief Scaling for variable <tt>id</tt> in Cheyshev model
  double const& _scalvar
    ( KEY const& id )
    const
    { return _CM->_scalvar[id]; };

  //! @brief Set Chebyshev variable equal to <a>CV</a>
  SCVar<T,KEY,COMP>& _set
    ( SCVar<T,KEY,COMP> const& CV );

  //! @brief Set Chebyshev variable equal to <a>CV</a>
  SCVar<T,KEY,COMP>& _set
    ( SCVar<T,KEY,COMP> && CV );

  //! @brief Set Chebyshev variable <a>id</a> with domain <a>dom</a>
  SCVar<T,KEY,COMP>& _set
    ( KEY const& id, T const& dom, const bool updt=true );

  //! @brief Product of multivariate Chebyshev polynomials in sparse format
  void _sprod
    ( const SCVar<T,KEY,COMP>& CV1, const SCVar<T,KEY,COMP>& CV2, t_poly& coefmon,
      double& coefrem )
    const
    { return _CM->_sprod( CV1, CV2, coefmon, coefrem ); }

public:
  /** @addtogroup SCHEBYSHEV Sparse Chebyshev Model Arithmetic for Factorable Functions
   *  @{
   */

  //! @brief Display sparse polynomial
  std::string display
    ( t_poly coefmon, int const& BASIS=1, int const& IDISP=5 )
    const;
    
  //! @brief Get pointer to linked Chebyshev model environment
  SCModel<T,KEY,COMP>* env() const
    { return _CM; }

  //! @brief Max order of model environment
  unsigned maxord
    ()
    const
    { return( _CM? _CM->_maxord: 0 ); };

  //! @brief Max order of participating variables in polynomial variable
  unsigned nord
    ()
    const
    { //std::cout << "NORD = " << _coefmon.crbegin()->first.tord << std::endl; 
      return _coefmon.crbegin()->first.tord; }

  //! @brief Number of participating variables in polynomial variable
  unsigned nvar
    ()
    const
    { return _ndxvar.size(); }

  //! @brief Total number of monomial terms in polynomial variable
  unsigned nmon
    ()
    const
    { return _coefmon.size(); }

  //! @brief Get const map of monomial coefficients
  t_poly const& coefmon
    ()
    const
    { return _coefmon; }

  //! @brief Get map of monomial coefficients
  t_poly& coefmon
    ()
    { return _coefmon; }

  //! @brief Get const set of participating variables
  t_var const& ndxvar
    ()
    const
    { return _ndxvar; }

  //! @brief Get set of participating variables
  t_var& ndxvar
    ()
    { return _ndxvar; }

  //! @brief Constructor of Chebyshev variable for a real scalar
  SCVar
    ( SCModel<T,KEY,COMP>*CM );

  //! @brief Constructor of Chebyshev variable for a real scalar
  SCVar
    ( double const& d = 0., SCModel<T,KEY,COMP>* CM = nullptr );

  //! @brief Constructor of Chebyshev variable for a remainder bound
  SCVar
    ( T const& B, SCModel<T,KEY,COMP>*CM = nullptr );

  //! @brief Constructor of Chebyshev variable <a>id</a> with domain <a>dom</a>
  SCVar
    ( SCModel<T,KEY,COMP>* CM, KEY const& id, T const& dom );

  //! @brief Copy constructor of Chebyshev variable
  SCVar
    ( SCVar<T,KEY,COMP> const& CV );

  //! @brief Copy constructor of Chebyshev variable
  SCVar
    ( SCVar<T,KEY,COMP> && CV );

  //! @brief Destructor of Chebyshev variable
  ~SCVar
    ()
    { _unset_bndpol(); _unset_bndT(); }

  //! @brief Set Chebyshev variable <a>id</a> with domain <a>dom</a>
  SCVar<T,KEY,COMP>& set
    ( SCModel<T,KEY,COMP>* CM, KEY const& id, T const& dom )
    { set( CM ); _set( id, dom ); return *this; }

  //! @brief Set environment in Chebyshev model
  SCVar<T,KEY,COMP>& set
    ( SCModel<T,KEY,COMP>* CM )
    { _CM = CM; return *this; }
    
  //! @brief Set multivariate polynomial coefficients in variable as <tt>coefmon</tt>
  SCVar<T,KEY,COMP>& set
    ( t_poly& coefmon )
    { _coefmon = coefmon; _unset_bndT(); _unset_bndpol();
      return *this; } // this is assuming the same order and number of variables

  //! @brief Set remainder term in variable as <tt>bndrem</tt>
  SCVar<T,KEY,COMP>& set
    ( T const& bndrem )
    { _bndrem = bndrem; return *this; }

  //! @brief Compute bound on variable using bounder <a>type</a>
  T bound
    ( int const type )
    const
    { if( !_bndT ) return _polybound(type) + _bndrem;
      else{ T bndT; return Op<T>::inter( bndT, _polybound(type) + _bndrem, *_bndT )?
                           bndT: _polybound(type) + _bndrem; } }

  //! @brief Retreive bound on variable using default bounder
  T bound
    ()
    const
    { if( !_bndT ) return bndpol() + _bndrem;
      else{ T bndT; return Op<T>::inter( bndT, bndpol() + _bndrem, *_bndT )?
                           bndT: bndpol() + _bndrem; } }

  //! @brief Retreive bound on variable using default bounder in U arithmetic
  template <typename U>
  U bound
    ( std::map<KEY,std::vector<U>,COMP> const& bndbasis, U const& bndrem )
    const
    { return _polybound( bndbasis ) + bndrem; }

  //! @brief Retreive bound on variable using bounder <a>type</a> in U arithmetic
  template <typename U>
  U bound
    ( std::map<KEY,std::vector<U>,COMP> const& bndbasis, U const& bndrem,
      const int type )
    const
    { return _polybound( bndbasis, type ) + bndrem; }

  //! @brief Retreive bound on multivariate polynomial using default bounder
  T bndpol
    ()
    const
    { if( !_bndpol ) _bndpol = new T( _polybound() );
      return *_bndpol; }

  //! @brief Retreive bound on all terms with (total) order <tt>minord</tt> or higher in polynomial part
  T bndord
    ( const unsigned minord )
    const
    { if( !_CM ) return !minord && !_coefmon.empty() && !_coefmon.begin()->first.tord?
                                   _coefmon.begin()->second: 0.;
      return _CM->_polybound_naive( _coefmon, std::map<KEY,std::vector<T>,COMP>(), minord ); }

  //! @brief Shortcut to mc::SCVar::bound
  T B
    ( const int type )
    const
    { return bound( type ); }

  //! @brief Shortcut to mc::SCVar::bound
  T B
    ()
    const
    { return bound(); }

  //! @brief Return remainder term of variable
  T R
    ()
    const
    { return _bndrem; }

  //! @brief Evaluate polynomial part at <tt>x</tt>
  double P
    ( std::map<KEY,double,COMP> const& x )
    const;

  //! @brief Return new Chebyshev variable with same multivariate polynomial part but zero remainder
  SCVar<T,KEY,COMP> P
    ()
    const
    { SCVar<T,KEY,COMP> var( *this ); var._bndrem = 0.; return var; }

  //! @brief Center remainder term of Chebyshev variable
  SCVar<T,KEY,COMP>& center
    ()
    { _center(); return *this; }

  //! @brief Shortcut to mc::SCVar::center
  SCVar<T,KEY,COMP>& C
    ()
    { return center(); }

  //! @brief Get coefficient of constant term in Chebyshev variable. The value of this coefficient is reset to 0 if <tt>reset=true</tt>, otherwise it is left unmodified (default).
  double constant
    ( bool const reset=false );

  //! @brief Get coefficients of linear term for variable <tt>id</tt> in Chebyshev variable. The value of this coefficient is reset to 0 if <tt>reset=true</tt>, otherwise it is left unmodified (default).
  double linear
    ( KEY const& id, bool const reset = false );

  //! @brief Lift the model by appending new vauxiliary ariable corresponding to the remainder term if magnitude greater than TOL
  SCVar<T,KEY,COMP>& lift
    ( SCModel<T,KEY,COMP>* SCM, double const& ATOL, double const& RTOL );

  //! @brief Project the model by removing monomials with auxiliary variables participating
  SCVar<T,KEY,COMP>& project
    ( bool const reset=false );

  //! @brief Project the model by removing monomials with variable id participating
  SCVar<T,KEY,COMP>& project
    ( KEY const& id );

  //! @brief Simplify the model by appending coefficient less than TOL as well as terms with order greater than or equal to TORD to the remainder term
  SCVar<T,KEY,COMP>& simplify
    ( double const& ATOL = 0e0, double const& RTOL = 0e0, int const TORD = -1 );
    
  //! @brief Scale coefficients in Chebyshev variable for the modified domain <a>dom</a> of variable id
  SCVar<T,KEY,COMP>& scale
    ( KEY const& id, T const& dom );
    
  //! @brief Scale coefficients in Chebyshev variable <a>id</a> for the modified variables domain <a>dom</a>
  SCVar<T,KEY,COMP>& scale
    ( std::map<KEY,T,COMP> const& dom );
    
  //! @brief Unscale coefficients in Chebyshev variable for their original variable ranges
  t_poly unscale
    ()
    const;
        
  //! @brief Return new coefficient map in monomial basis representation
  t_poly to_monomial
    ( bool const scaled = false )
    const;
    
  //! @brief Return new coefficient map in monomial basis representation after removing terms with coefficient less than TOL or order greater than or equal to ORD, and also return a bound on the removed terms
  std::pair<t_poly,T> to_monomial
    ( bool const scaled, double const& ATOL, double const& RTOL, int const TORD = -1 )
    const;
 /** @} */

  SCVar<T,KEY,COMP>& operator=
    ( SCVar<T,KEY,COMP> const& );
  SCVar<T,KEY,COMP>& operator=
    ( SCVar<T,KEY,COMP> && );
  SCVar<T,KEY,COMP>& operator=
    ( double const& );
  SCVar<T,KEY,COMP>& operator=
    ( T const&  );
  template <typename U> SCVar<T,KEY,COMP>& operator+=
    ( SCVar<U,KEY,COMP> const& );
  template <typename U> SCVar<T,KEY,COMP>& operator+=
    ( U const&  );
  SCVar<T,KEY,COMP>& operator+=
    ( double const& );
  template <typename U> SCVar<T,KEY,COMP>& operator-=
    ( const SCVar<U,KEY,COMP>& );
  template <typename U> SCVar<T,KEY,COMP>& operator-=
    ( U const&  );
  SCVar<T,KEY,COMP>& operator-=
    ( double const& );
  SCVar<T,KEY,COMP>& operator*=
    ( const SCVar<T,KEY,COMP>& );
  SCVar<T,KEY,COMP>& operator*=
    ( double const& );
  SCVar<T,KEY,COMP>& operator*=
    ( T const&  );
  SCVar<T,KEY,COMP>& operator/=
    ( SCVar<T,KEY,COMP> const& );
  SCVar<T,KEY,COMP>& operator/=
    ( double const& );

private:
 
  //! @brief Polynomial range bounder using specified bounder <a>type</a> with basis functions <a>bndbasis</a> in U arithmetic
  template <typename U>
  U _polybound
    ( std::map<KEY,std::vector<U>,COMP> const& bndbasis, int const type )
    const
    { if( !_CM ) return !_coefmon.empty() && !_coefmon.begin()->first.tord? _coefmon.begin()->second: 0.;
      return _CM->_polybound( _coefmon, bndbasis, type ); }

  //! @brief Polynomial range bounder using default bounder in U arithmetic
  template <typename U>
  U _polybound
    ( std::map<KEY,std::vector<U>,COMP> const& bndbasis )
    const
    { return _polybound( bndbasis, _CM? _CM->options.BOUNDER_TYPE: 0 ); }

  //! @brief Polynomial range bounder using specified bounder <a>type</a>
  T _polybound
    ( int const type )
    const
    { return _polybound( std::map<KEY,std::vector<T>,COMP>(), type ); }

  //! @brief Polynomial range bounder using default bounder
  T _polybound
    ()
    const
    { return _polybound( _CM? _CM->options.BOUNDER_TYPE: 0 ); }

  //! @brief Scale current variable in order for its range to be within [-1,1], with <a>c</a> and <a>w</a> respectively the center and width, respectively, of the orginal variable range
  SCVar<T,KEY,COMP> _rescale
    ( double const& w, double const& c ) const
    { return( !isequal(w,0.)? (*this-c)/w: c ); }

  //! @brief Scale variable <a>var</a>
  template <typename U> static
  U _rescale
    ( U& var, double const& w, double const& c )
    { return( !isequal(w,0.)? (var-c)/w: c ); }

  //! @brief Cancel zero entries in coefficient map <a>coefmon</a>
  void _simplify
    ( t_poly& coefmon )
    const;

  //! @brief Simplify the monomial itmon by appending coefficient less than TOL as well as terms with order greater than or equal to TORD to the remainder term
  SCVar<T,KEY,COMP>& _simplify
    ( typename SCVar<T,KEY,COMP>::t_poly::iterator itmon,
      double const& ATOL, double const& RTOL, int const TORD=-1 );

  //! @brief Simplify the coefficient map <a>coefmon</a> by removing entries with coefficient less than TOL or order greater than or equal to ORD, and return a bound on the removed entries
  T _simplify_monomial
    ( t_poly& coefmon, bool const scaled,
      double const& ATOL, double const& RTOL, int const TORD=-1 )
    const;

  //! @brief Scale coefficient map <a>coefmon</a> for the modified range <a>bnd</a> of variable id
  void _scale
    ( typename t_var::const_iterator itvar, T const& bndvar, t_poly& coefmon )
    const;

  //! @brief Convert Chebyshev basis into monomial basis for variable <a>ivar</a> in coefficient map <a>coefmon</a>
  void _to_monomial
    ( KEY const& id, t_poly& coefmon )
    const;
};

////////////////////////////////// SCModel //////////////////////////////////////

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_set
( KEY const& id, T const& X )
{
  _setvar.insert( id );
  _bndvar[id] = X;
  _refvar[id] = Op<T>::mid(X);
  _scalvar[id] = 0.5*Op<T>::diam(X);
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::reset_aux
()
{
  for( auto& aux : _setaux ){
    _setvar.erase( aux );
    _bndvar.erase( aux );
    _refvar.erase( aux );
    _scalvar.erase( aux );
    _delaux( aux );
  }
  _setaux.clear();
}

template <typename T, typename KEY, typename COMP>
inline
KEY const&
SCModel<T,KEY,COMP>::append_aux
()
{
  auto dummy = KEY();
  auto [itaux,ins] = _setaux.insert( _newaux( dummy, 10000+_setaux.size() ) );
  assert( ins );
  _setvar.insert( *itaux );
  _bndvar[*itaux] = TOne;
  _refvar[*itaux] = 0e0;
  _scalvar[*itaux] = 1e0;
  return *itaux;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
std::vector<U>&
SCModel<T,KEY,COMP>::_get_bndpow
( unsigned const maxord, U const& bndvar, double const& refvar, double const& scalvar,
  std::vector<U>& bndpow )
const
{
  bndpow.reserve( maxord+1 );
  bndpow[0] = 1;
  if( maxord ) bndpow[1] = ( bndvar - refvar ) / scalvar;
  for( unsigned i=2; i<=maxord; i++ )
    switch( options.BASIS ){
      case Options::MONOM: bndpow[i] = Op<U>::pow( bndpow[1], i );  break;
      case Options::CHEB:  bndpow[i] = Op<U>::cheb( bndpow[1], i ); break;
    }
 return bndpow;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
std::vector<U>&
SCModel<T,KEY,COMP>::_get_bndpow
( unsigned const maxord, U const& bndvar, std::vector<U>& bndpow )
const
{
  bndpow.reserve( maxord+1 );
  bndpow[0] = 1;
  if( maxord ) bndpow[1] = bndvar;
  for( unsigned i=2; i<=maxord; i++ )
    switch( options.BASIS ){
      case Options::MONOM: bndpow[i] = Op<U>::pow( bndpow[1], i );  break;
      case Options::CHEB:  bndpow[i] = Op<U>::cheb( bndpow[1], i ); break;
    }
 return bndpow;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
std::map<KEY,std::vector<U>,COMP>&
SCModel<T,KEY,COMP>::get_basis
( unsigned const maxord, std::map<KEY,U,COMP> const& bndvar,
  std::map<KEY,std::vector<U>,COMP>& bndbasis, bool const scaled )
const
{
  bndbasis.clear();
  for( auto const& id : _setvar ){
    bndbasis[id].clear();
    if( scaled ) _get_bndpow( maxord, bndvar[id], bndbasis[id] );
    else         _get_bndpow( maxord, bndvar[id], _refvar[id], _scalvar[id] );
  }
  return bndbasis;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
void
SCModel<T,KEY,COMP>::get_bndmon
( std::map<t_mon,U,lt_mon>& bndmon, std::map<KEY,U,COMP> const& bndvar,
  bool const scaled, bool const useprod )
const
{
  if( bndmon.empty() ) return;

  unsigned const nord = bndmon.rbegin()->first.tord;
  std::map<KEY,std::vector<U>,COMP>& bndbasis;
  get_basis( nord, bndvar, bndbasis, scaled );

  auto it = bndmon.begin();
  if( !it->first.tord ){ it->second = 1; ++it; }

  // Use product operation in U arithmetic
  if( useprod ){
    std::vector<U> Umon( _setvar.size() );
    for( ; it!=bndmon.end(); ++it ){
      unsigned nmon=0;
      for( auto const& [ivar,iord] : it->first.second )
        Umon[nmon++] = bndbasis[ivar][iord];
      if( nmon == 1 ) it->second = Umon.front();
      else            it->second = Op<U>::prod( nmon, Umon.data() );
    }
  }

  // Create product operation recursively in U arithmetic
  else{
    for( ; it!=bndmon.end(); ++it ){
      it->second = 1;
      for( auto const& [ivar,iord] : it->first.second )
        it->second *= bndbasis[ivar][iord];
    }
  }
}

template <typename T, typename KEY, typename COMP>
template <typename PUNIV>
inline
double
SCModel<T,KEY,COMP>::_minimax
( PUNIV const& f ) // range of f assumed as [-1,1]
{
  boost::math::tools::remez_minimax<double> problem
    ( f, _maxord, 0, -1., 1., false, false, 0, 64 );

  for( unsigned iter=0; iter<options.REMEZ_MAXIT; ++iter ){
    problem.iterate();
#ifdef MC__SCMODEL_DEBUG_MINIMAX
    const boost::math::tools::polynomial<double> a = problem.numerator();
    std::cout << iter << ": [ " << std::right << std::scientific << std::setprecision(15);
    for( unsigned k=0; k<a.size(); ++k ) std::cout << std::setw(23) << a[k];
    std::cout << " ] +/- " << std::setw(23) << problem.max_error() << std::endl;
#endif
    if( problem.max_change() < options.REMEZ_TOL ) break;
  }

  switch( options.BASIS ){
  case Options::CHEB:
    _coefuniv = _to_chebyshev(problem.numerator().data());
    break;
  case Options::MONOM:
    _coefuniv = problem.numerator().data();
    break;
  }
  return problem.max_error();
}

template <typename T, typename KEY, typename COMP>
template <typename VEC>
inline
VEC
SCModel<T,KEY,COMP>::_to_chebyshev
( VEC const& veccoef )
const
{
  // Converts COEFF in monomial basis to the Chebyshev basis.
  VEC vecnew = veccoef;
  int const N = vecnew.size()-1;
  if( N <= 1 ) return vecnew;
  // TP = .5D0**(N-1)
  // COEFF(N) = TP * COEFF(N)
  // COEFF(N-1) = TP * COEFF(N-1)
  double TP = std::pow(0.5,N-1);
  vecnew[N]   *= TP;
  vecnew[N-1] *= TP;
  // do 20 J = N-2, 0, -1
  //   TP = 2.D0 * TP
  //   COEFF(J) = TP * COEFF(J)
  //   COEFF(J+1) = 2.D0 * COEFF(J+1)
  for( int J=N-2; J>=0; --J ){
    TP *= 2;
    vecnew[J]   *= TP;
    vecnew[J+1] *= 2;
    // do 10 I = J, N-2
    //   COEFF(I) = COEFF(I) + COEFF(I+2)
    for( int I=J; I<N-1; ++I )
      vecnew[I] += vecnew[I+2];
  }
  return vecnew;
}

template <typename T, typename KEY, typename COMP>
template <typename PUNIV>
inline
bool
SCModel<T,KEY,COMP>::_minimax
( PUNIV const& f, SCVar<T,KEY,COMP> const& CV, SCVar<T,KEY,COMP>& CV2 )
{
  auto LIFT_USE_SAVE = options.LIFT_USE;
  options.LIFT_USE = false;

  try{
    double m( Op<SCVar<T,KEY,COMP>>::mid(CV) ), r( 0.5*Op<SCVar<T,KEY,COMP>>::diam(CV) );
    double rem = _minimax( [=]( const double& x ){ return f( r*x + m ); } );
    assert( _coefuniv.size() == _maxord+1 );

    SCVar<T,KEY,COMP> CVI( this );
    CVI = CV._rescale(r,m);
    switch( options.BASIS ){
    case Options::CHEB:
      CV2 = _clenshaw( CVI, _maxord ) + TOne * rem;
      break;
    case Options::MONOM:
      CV2 = _horner( CVI, _maxord ) + TOne * rem;
      break;
    }
    if( options.MIG_USE ) CV2.simplify( options.MIG_ATOL, options.MIG_RTOL );
  }

  catch(...){
    options.LIFT_USE = LIFT_USE_SAVE;
    return false;
  }

  options.LIFT_USE = LIFT_USE_SAVE;
  return true;
}

template <typename T, typename KEY, typename COMP>
template <typename PUNIV>
inline
double
SCModel<T,KEY,COMP>::_rematbound
( PUNIV const& f ) // range of f assumed as [-1,1]
{
  double ub(0), lb(0);
  for( unsigned i=0; i<=_maxord; i++ ){
    ub += _coefuniv[i];
    lb += i%2? -_coefuniv[i]: _coefuniv[i];
  }
  return std::max( std::fabs( f(1.) - ub ), std::fabs( f(-1.) - lb ) );
}

template <typename T, typename KEY, typename COMP>
template <typename PUNIV>
inline
void
SCModel<T,KEY,COMP>::_chebinterp
( PUNIV const& f, unsigned const maxord ) // range of f assumed as [-1,1]
{
  _xinterp.resize( maxord+1 );
  _finterp.resize( maxord+1 );
  double mulconst( PI/(2.*double(maxord+1)) );
  for( unsigned i=0; i<=maxord; i++ ){
    _xinterp[i]  = std::cos( mulconst*(2.*double(i)+1.) );
    _finterp[i] = f( _xinterp[i] );
  }

  _coefuniv.resize( maxord+1 );
  switch( maxord ){
  case 0:
    _coefuniv[0] = _finterp[0];
    return;
  case 1:
    _coefuniv[0] = 0.5 * ( _finterp[0] + _finterp[1] );
    _coefuniv[1] = ( _finterp[1] - _finterp[0] ) / ( _xinterp[1] - _xinterp[0] );
    return;
  default:
    for( unsigned i=0; i<=maxord; i++ ){
      double mulconst2( std::cos( mulconst*double(i) ) ),
             mulconst3( 4*mulconst2*mulconst2-2 ), 
             b0( 0 ), b1( 0 );
      b0 = _finterp[maxord];
      b1 = _finterp[maxord-1] + mulconst3*b0;
      for( unsigned j=maxord-2; j>1; j-=2 ){
        b0 = _finterp[j] + mulconst3*b1 - b0;
        b1 = _finterp[j-1] + mulconst3*b0 - b1;
      }
      if( !(maxord%2) )
        b0 = _finterp[0] + mulconst3*b1 - b0 - b1;
      else{
        b0 = _finterp[1] + mulconst3*b1 - b0;
        b0 = _finterp[0] + mulconst3*b0 - b1 - b0;
      }
      _coefuniv[i] = 2./double(maxord+1)*mulconst2*b0;
    }
    _coefuniv[0] *= 0.5;
    return;
  }
}

template <typename T, typename KEY, typename COMP>
template <typename PUNIV>
inline
void
SCModel<T,KEY,COMP>::_chebinterp
( PUNIV const& f, double const& tol, unsigned& maxord ) // range of f assumed as [-1,1]
{
  _chebinterp( f, maxord );
  for( ; std::fabs(_coefuniv[maxord])>tol || (maxord && std::fabs(_coefuniv[maxord-1])>tol); ){
    maxord *= 2;
    _chebinterp( f, maxord );
#ifdef MC__SCMODEL_DEBUG_CHEBINTERP
    for( unsigned i=0; i<=maxord; i++ )
      std::cout << "a[" << i << "] = " << _coefuniv[i] << std::endl;
    { int dum; std::cout << "ENTER <1> TO CONTINUE"; std::cin >> dum; }
#endif
  }
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
SCModel<T,KEY,COMP>::_horner
( SCVar<T,KEY,COMP> const& CVinner, unsigned const maxord )
{
  assert( maxord < _coefuniv.size() );
  
  // composition based on Horner's scheme
  switch( maxord ){
  case 0:
    return _coefuniv[0];
    break;
  default:
    SCVar<T,KEY,COMP> CV2 = _coefuniv[_maxord] * CVinner;
    for( unsigned ord=_maxord-1; ord>0; --ord ){
      CV2 += _coefuniv[ord];
      CV2 *= CVinner;
    }
    CV2 += _coefuniv[0];
    return CV2;
  }
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
SCModel<T,KEY,COMP>::_clenshaw
( SCVar<T,KEY,COMP> const& CVinner, unsigned const maxord )
{
  assert( maxord < _coefuniv.size() );
  
  if( !maxord )
    return _coefuniv[0];

  else if( maxord == 1 )
    return CVinner * _coefuniv[1] + _coefuniv[0];

  //composition based on http://en.wikipedia.org/wiki/Clenshaw_algorithm#Special_case_for_Chebyshev_series
  SCVar<T,KEY,COMP> CVinnerx2 = 2. * CVinner;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CVinner:" << CVinner;
  std::cout << "CVinnerx2:" << CVinnerx2;
#endif
  SCVar<T,KEY,COMP> CV1 = _coefuniv[maxord];
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CV1:" << CV1;
#endif
  SCVar<T,KEY,COMP> CV2 = _coefuniv[maxord-1] + CVinnerx2 * CV1;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CV2:" << CV2;
#endif

  for( unsigned i=maxord-2; i>1; i-=2 ){
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    std::cout << "CVinnerx2 * CV2" << CVinnerx2 * CV2;
    std::cout << "_coefuniv[i]:" << _coefuniv[i];
    std::cout << "_coefuniv[i] + CVinnerx2 * CV2" << _coefuniv[i] + CVinnerx2 * CV2;
#endif
    CV1 = _coefuniv[i]   + CVinnerx2 * CV2 - CV1;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    std::cout << "CV1:" << CV1;
#endif
    CV2 = _coefuniv[i-1] + CVinnerx2 * CV1 - CV2;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    std::cout << "CV2:" << CV2;
#endif
  }

  if( !(maxord%2) ){
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    CV1 = _coefuniv[0] + CVinner * CV2 - CV1;
    std::cout << "CV1:" << CV1;
    return CV1;
#else
    return _coefuniv[0] + CVinner * CV2 - CV1;
#endif
  }

  CV1 = _coefuniv[1] + CVinnerx2 * CV2 - CV1;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CV1:" << CV1;
  std::cout << "CVinner * CV1:" << CVinner * CV1;
  CV2 = _coefuniv[0] + CVinner * CV1 - CV2;
  std::cout << "CV2:" << CV2;
  return CV2;
#else
  return _coefuniv[0] + CVinner * CV1 - CV2;
#endif
}

template <typename T, typename KEY, typename COMP>
template <typename PUNIV>
inline
bool
SCModel<T,KEY,COMP>::_chebinterp
( PUNIV const& f, SCVar<T,KEY,COMP> const& CV, SCVar<T,KEY,COMP>& CV2, bool const rematbound )
{
  auto LIFT_USE_SAVE = options.LIFT_USE;
  options.LIFT_USE = false;

  try{
    double m( Op<SCVar<T,KEY,COMP>>::mid(CV) ), r( 0.5*Op<SCVar<T,KEY,COMP>>::diam(CV) ), rem( 0. );
    auto const& fscaled = [=]( const double& x ){ return f( r*x + m ); };
    if( rematbound ){
      unsigned const nord = _maxord + options.INTERP_EXTRA;
      _chebinterp( fscaled, nord );
      rem = _rematbound( fscaled );
    }
    else{
      unsigned nord = _maxord+2;
      double tol = options.INTERP_THRES;
      _chebinterp( fscaled, tol, nord );
      rem = 2*tol;
      for( unsigned i=_maxord+1; i<=nord; i++ )
        rem += std::fabs( _coefuniv[i] );
    }

    CV2 = _clenshaw( CV._rescale( r, m ), _maxord );
    CV2 += TOne * rem;
    //CV2._ndxvar = CV._ndxvar;
    if( options.MIG_USE ) CV2.simplify( options.MIG_ATOL, options.MIG_RTOL );
  }

  catch(...){
    options.LIFT_USE = LIFT_USE_SAVE;
    return false;
  }

  options.LIFT_USE = LIFT_USE_SAVE;
  return true;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
SCVar<T,KEY,COMP>
SCModel<T,KEY,COMP>::_composition
( std::vector<U> const& coefouter, unsigned const maxord, SCVar<T,KEY,COMP> const& CVinner )
{
  //composition based on http://en.wikipedia.org/wiki/Clenshaw_algorithm#Special_case_for_Chebyshev_series
  if( !maxord )
    return coefouter[0];

  else if( maxord == 1 )
    return CVinner * coefouter[1] + coefouter[0];

  SCVar<T,KEY,COMP> CVinnerx2 = 2. * CVinner;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CVinner:" << CVinner;
  std::cout << "CVinnerx2:" << CVinnerx2;
#endif
  SCVar<T,KEY,COMP> CV1 = coefouter[maxord];
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CV1:" << CV1;
#endif
  SCVar<T,KEY,COMP> CV2 = coefouter[maxord-1] + CVinnerx2 * CV1;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CV2:" << CV2;
#endif

  for( unsigned i=maxord-2; i>1; i-=2 ){
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    std::cout << "CVinnerx2 * CV2" << CVinnerx2 * CV2;
    std::cout << "coefouter[i]:" << coefouter[i];
    std::cout << "coefouter[i] + CVinnerx2 * CV2" << coefouter[i] + CVinnerx2 * CV2;
#endif
    CV1 = coefouter[i]   + CVinnerx2 * CV2 - CV1;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    std::cout << "CV1:" << CV1;
#endif
    CV2 = coefouter[i-1] + CVinnerx2 * CV1 - CV2;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    std::cout << "CV2:" << CV2;
#endif
  }

  if( !(maxord%2) ){
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
    CV1 = coefouter[0] + CVinner * CV2 - CV1;
    std::cout << "CV1:" << CV1;
    return CV1;
#else
    return coefouter[0] + CVinner * CV2 - CV1;
#endif
  }

  CV1 = coefouter[1] + CVinnerx2 * CV2 - CV1;
#ifdef MC__SCMODEL_DEBUG_COMPOSITION
  std::cout << "CV1:" << CV1;
  CV2 = coefouter[0] + CVinner * CV1 - CV2;
  std::cout << "CV2:" << CV2;
  return CV2;
#else
  return coefouter[0] + CVinner * CV1 - CV2;
#endif
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
SCModel<T,KEY,COMP>::_intpow
( SCVar<T,KEY,COMP> const& CV, int const n )
const
{
  if( n == 0 ) return 1.;
  else if( n == 1 ) return CV;
  else if( n == 2 ) return sqr( CV );
  return n%2 ? sqr( _intpow( CV, n/2 ) ) * CV : sqr( _intpow( CV, n/2 ) );
}

template <typename T, typename KEY, typename COMP>
inline void
SCModel<T,KEY,COMP>::_sscal1D
( t_poly const& coefmon0, double const& dscal, t_poly& coefmon )
const
{
  if( isequal(dscal,0.) ) return;
  coefmon = coefmon0;
  if( isequal(dscal,1.) ) return;
  for( auto& [mon,coef] : coefmon )
    coef *= dscal;
  return;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_slift1D
( t_poly const& coefmon0, double const& dscal, t_poly& coefmon )
const
{
  if( isequal(dscal,0.) ) return;
  for( auto const& [mon0,coef0] : coefmon0 ){
    auto [itmon,ins] = coefmon.insert( std::make_pair(mon0,coef0) );
    if( ins ){
      if( isequal(dscal,1.) ) continue;
      itmon->second *= dscal;
    }
    else{
      itmon->second += coef0*dscal;
    }
  }
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_slift1D
( t_poly const& coefmon0, double const& dscal, t_poly& coefmon,
  T& rem, typename t_var::const_iterator itvar, unsigned const ndxord )
const
{
  for( auto const& [mon0,coef0] : coefmon0 ){
    if( mon0.tord + ndxord > _maxord ){ // append to remainder coefficient if total order too large
      switch( options.BASIS ){
        case Options::CHEB:  rem += (coef0*dscal)*TOne; continue;
        case Options::MONOM: rem += (coef0*dscal)*(ndxord%2 || mon0.gcexp()%2? TOne: TZerOne); continue;
      }
    }
    auto mon = mon0; // local copy for modification
#ifdef MC__SCMODEL_DEBUG_SPROD
    std::cout << "mon: " << mon.display(options.BASIS) << "  (" << *itvar << "," << ndxord << ")" << std::endl;
#endif
    mon.tord += ndxord;
    assert( mon.expr.insert( std::make_pair( *itvar, ndxord ) ).second );
    auto [itmon,ins] = coefmon.insert( std::make_pair( mon, coef0*dscal ) );
    if( !ins ) itmon->second += coef0*dscal;
  }
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_slift1D
( t_poly const& coefmon0, double const& dscal, T& rem,
  unsigned const ndxord )
const
{
  for( auto const& [mon0,coef0] : coefmon0 )
    switch( options.BASIS ){
      case Options::CHEB:  rem += (coef0*dscal)*TOne; continue;
      case Options::MONOM: rem += (coef0*dscal)*(ndxord%2 || mon0.gcexp()%2? TOne: TZerOne); continue;
    }
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_sdisp1D
( t_poly const& coefmon, std::string const& name, std::ostream& os )
const
{
  os << name;
  for( auto it=coefmon.begin(); it!=coefmon.end(); ++it ){
    if( it != coefmon.begin() ) os << " + ";
    os << it->second;
      for( auto const& [ivar,iord] : it->first.expr )
        os << "·T" << iord << "[" << ivar << "]";
  }
  //os << std::endl;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_sdisp1D
( std::map<unsigned,t_poly> const& coefmap, typename t_var::const_iterator itvar,  
  std::string const& name, std::ostream& os )
const
{
  os << name;
  bool first = true;
  for( auto const& [i,coefmon] : coefmap ){
    if( !first ) os << " + T" << i << "[" << *itvar << "] ·";
    os << " { ";
    _sdisp1D( coefmon, "", os );
    os << " }";
    first = false;
  }    
  os << std::endl;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_sdisp1Dfull
( std::vector<SCVar<T,KEY,COMP>> const& vec, typename t_var::const_iterator itvar, 
  std::string const& name, std::ostream& os )
const
{
  os << name;
  bool first = true;
  unsigned i=0;
  for( auto const& term : vec ){
    if( !first ) os << " + T" << i++ << "[" << *itvar << "] ·";
    os << " { ";
    _sdisp1D( term.coefmon(), "", os );
    os << " }";
    first = false;
  }
  os << std::endl;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_sprod1D
( std::map<unsigned,t_poly> const& sp1map,
  std::map<unsigned,t_poly> const& sp2map,
  t_poly& coefmon, T& rem, t_var const& ndxvar,
  typename t_var::const_iterator itvar )
const
{
  // construct product matrix of polynomial coefficients
  auto itvarnext = itvar;
  if( !ndxvar.empty() ) std::advance( itvarnext, 1 );
  std::map<std::pair<unsigned,unsigned>,t_poly> sp12map;
  for( auto& [ndx1,coefmon1] : sp1map ){
    // empty monomial in sp1
    if( coefmon1.empty() ) continue; 
    // constant monomial in sp1
    if( coefmon1.size() == 1 && !coefmon1.begin()->first.tord ){
      for( auto& [ndx2,coefmon2] : sp2map ){
        auto [it12,ins] = sp12map.insert( std::make_pair( std::make_pair(ndx1,ndx2), t_poly() ) );
        assert( ins ); // map is initially empty
        _sscal1D( coefmon2, coefmon1.begin()->second, it12->second );
      }
      continue;
    }
    // general monomial in sp1
    for( auto& [ndx2,coefmon2] : sp2map ){
      // empty monomial in sp2
      if( coefmon2.empty() ) continue; // no term
      // constant monomial in sp2
      if( coefmon2.size() == 1 && !coefmon2.begin()->first.tord ){
        auto [it12,ins] = sp12map.insert( std::make_pair( std::make_pair(ndx1,ndx2), t_poly() ) );
        assert( ins ); // map is initially empty
        _sscal1D( coefmon1, coefmon2.begin()->second, it12->second );
        continue;
      }
#ifdef MC__SCMODEL_DEBUG_SPROD
      std::cout << "Term (" << ndx1 << "," << ndx2 << "):\n";
#endif
      std::map<unsigned,t_poly> sp11map, sp22map;
      for( auto& mon1 : coefmon1 )
        _svec1D( itvarnext, mon1, sp11map );
#ifdef MC__SCMODEL_DEBUG_SPROD
      _sdisp1D( sp11map, itvarnext, "Poly #1: " );
#endif
      for( auto& mon2 : coefmon2 )
        _svec1D( itvarnext, mon2, sp22map );
#ifdef MC__SCMODEL_DEBUG_SPROD
      _sdisp1D( sp22map, itvarnext, "Poly #2: " );
#endif
      auto [it12,ins] = sp12map.insert( std::make_pair( std::make_pair(ndx1,ndx2), t_poly() ) );
      assert( ins ); // map is initially empty
      _sprod1D( sp11map, sp22map, it12->second, rem, ndxvar, itvarnext );
    }
  }
  
  // construct 1D product result and augment remainder as appropriate
  coefmon.clear();
  for( auto const& [ndx12,coefmon12] : sp12map ){
    auto const& [ndx1,ndx2] = ndx12;
    // Product involving two constant terms
    if( !ndx1 && !ndx2 )
      _slift1D( coefmon12, 1., coefmon );
    // Product involving exactly one constant term
    else if( !ndx1 || !ndx2 )
      _slift1D( coefmon12, 1., coefmon, rem, itvar, ndx1+ndx2 );
    // Product between non-constant terms
    else{
      switch( options.BASIS ){
        // Chebyshev basis functions
        case Options::CHEB:
          if( ndx1+ndx2 <= _maxord )
            _slift1D( coefmon12, .5, coefmon, rem, itvar, ndx1+ndx2 );
          else
            _slift1D( coefmon12, .5, rem, ndx1+ndx2 );
          if( ndx1 == ndx2 )
            _slift1D( coefmon12, .5, coefmon );
          else if( ndx1 > ndx2 )
            _slift1D( coefmon12, .5, coefmon, rem, itvar, ndx1-ndx2 );
          else
            _slift1D( coefmon12, .5, coefmon, rem, itvar, ndx2-ndx1 );
          break;
        // Power basis functions
        case Options::MONOM:
          if( ndx1+ndx2 <= _maxord )
            _slift1D( coefmon12, 1., coefmon, rem, itvar, ndx1+ndx2 );
          else
            _slift1D( coefmon12, 1., rem, ndx1+ndx2 );
          break;
      }
    }
  }
#ifdef MC__SCMODEL_DEBUG_SPROD
  _sdisp1D( coefmon, "Prod: " );
  std::endl;
#endif
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_svec1D
( typename t_var::const_iterator itvar, std::pair<t_mon,double> const& coefmon,
  std::map<unsigned,t_poly>& mapspoly )
const
{
  auto const& [ivar,iord] = *coefmon.first.expr.begin();
  if( !coefmon.first.tord || ivar != *itvar ) // no dependence on variable *itvar 
    mapspoly[ 0 ].insert( coefmon );
  else     // dependence on variable *itvar of order iord
    mapspoly[ iord ].insert( std::make_pair( t_mon( coefmon.first.tord - iord,
      std::map<KEY,unsigned,COMP>( ++coefmon.first.expr.begin(), coefmon.first.expr.end() ) ),
      coefmon.second ) );
}

template <typename T, typename KEY, typename COMP>
inline
void
SCModel<T,KEY,COMP>::_svec1Dfull
( typename t_var::const_iterator itvar, std::pair<t_mon,double> const& coefmon,
  std::vector< SCVar<T,KEY,COMP> >& vec )
const
{
  auto& [mon,coef] = coefmon;
  auto ie = mon.expr.find( *itvar );
  if( ie == mon.expr.end() ) // no dependence on variable *itvar 
    vec[ 0 ].coefmon().insert( coefmon );
  else{ // dependence on variable *itvar of order iord
    auto& [ivar,iord] = *ie;
    t_mon monmod( mon.tord - iord, mon.expr );
    monmod.expr.erase( ivar ); // remove *itvar entry
    vec[ iord ].coefmon().insert( std::make_pair( monmod, coef ) );
  }
}

template <typename T, typename KEY, typename COMP>
template <typename C, typename U>
inline
U
SCModel<T,KEY,COMP>::_polybound_LSB
( std::map<t_mon,C,lt_mon> const& coefmon, std::map<KEY,std::vector<U>,COMP> const& bndbasis )
const
{
  // Constant or linear model
  if( coefmon.empty() || coefmon.rbegin()->first.tord < 2 )
    return _polybound_naive( coefmon, bndbasis );

  // Quadratic terms in combination with linear terms
  static double const TOL = 1e-8;
  U bndpol = coefmon.cbegin()->first.tord? 0.: coefmon.cbegin()->second;
  auto it1 = coefmon.lower_bound( t_mon( 1, std::map<KEY,unsigned,COMP>() ) );
  auto it2 = coefmon.lower_bound( t_mon( 2, std::map<KEY,unsigned,COMP>() ) );
  auto it3 = coefmon.lower_bound( t_mon( 3, std::map<KEY,unsigned,COMP>() ) );
  std::map<t_mon,C,lt_mon> coeflin; coeflin.insert( it1, it2 );

  for( ; it2!=it3; ++it2 ){
    auto ie2 = it2->first.expr.begin();
    if( ie2->second == 1 ){ // off-diagonal quadratic terms
      bndpol += it2->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[1] * bndbasis.at((++ie2)->first)[1]: TOne );
      continue;
    }
    t_mon explin( 1, std::map<KEY,unsigned,COMP>() );
    explin.expr.insert( std::make_pair( ie2->first, 1 ) ); 
    it1 = coeflin.find( explin );

    switch( options.BASIS ){
      case Options::CHEB:
        if( it1 != coeflin.end() && std::fabs(it2->second) > TOL ){
          double const& ai  = it1->second;
          double const& aii = it2->second;
          bndpol += (2.*aii)*Op<U>::sqr((!bndbasis.empty()? bndbasis.at(ie2->first)[1]: TOne)+ai/(aii*4.))-aii-ai*ai/8./aii;
          coeflin.erase( it1 );
        }
        else if( it1 != coeflin.end() ){
          bndpol += it2->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[2]: TOne )
                  + it1->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[1]: TOne );
           coeflin.erase( it1 );
        }
        else
          bndpol += it2->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[2]: TOne );
        break;
	
      case Options::MONOM:
        if( it1 != coeflin.end() && std::fabs(it2->second) > TOL ){
          double const& ai  = it1->second;
          double const& aii = it2->second;
          bndpol += aii*Op<U>::sqr((!bndbasis.empty()?bndbasis.at(ie2->first)[1]:TOne)+ai/(2.*aii))-ai*ai/(aii*4.);
          coeflin.erase( it1 );
        }
        else if( it1 != coeflin.end() ){
          bndpol += it2->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[2]: TZerOne )
                  + it1->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[1]: TOne );
           coeflin.erase( it1 );
        }
        else
          bndpol += it2->second * ( !bndbasis.empty()? bndbasis.at(ie2->first)[2]: TZerOne );
        break;
    }
  }

  // Remaining linear terms
  for( it1=coeflin.begin(); it1!=coeflin.end(); ++it1 ){
    auto ie1 = it1->first.expr.begin();
    bndpol += it1->second * ( !bndbasis.empty()? bndbasis.at(ie1->first)[1]: TOne );
  }

  // Third and higher-order terms
  if( coefmon.rbegin()->first.tord > 2 )
    bndpol += _polybound_naive( coefmon, bndbasis, 3 );

  return bndpol;
}

template <typename T, typename KEY, typename COMP>
template <typename C, typename U>
inline
U
SCModel<T,KEY,COMP>::_polybound_naive
( std::map<t_mon,C,lt_mon> const& coefmon, std::map<KEY,std::vector<U>,COMP> const& bndbasis,
  unsigned const minord )
const
{
  // Empty model
  if( coefmon.empty() || coefmon.rbegin()->first.tord < minord ) return 0.;
  auto it = coefmon.lower_bound( t_mon( minord, std::map<KEY,unsigned,COMP>() ) );

  // Polynomial bounding in T arithmetic
  if( bndbasis.empty() ){
    switch( options.BASIS ){
      case Options::CHEB:
      {
        double bndcst = 0., bndcoef = 0.;
        if( !it->first.tord ){ bndcst = it->second; ++it; }
        for( ; it!=coefmon.end(); ++it )
          bndcoef += std::fabs( it->second );
        return bndcoef * TOne + bndcst;
      }
      case Options::MONOM:
      {
        T bndpol = 0.;
        if( !it->first.tord ){ bndpol = it->second; ++it; }
        for( ; it!=coefmon.end(); ++it )
          bndpol += it->second * (it->first.gcexp()%2? TOne: TZerOne);
        return bndpol;
      }
    }
  }

  // Polynomial bounding in U arithmetic
  U bndpol( 0. );
  if( !it->first.tord ){ bndpol = it->second; ++it; }
  for( ; it!=coefmon.end(); ++it ){
    U bndmon( 1. );
    for( auto const& [var,ord] : it->first.expr )
      bndmon *= bndbasis.at(var)[ord];
    bndpol += it->second  * bndmon;
  }
  return bndpol;
}

template <typename T, typename KEY, typename COMP>
template <typename C, typename U>
inline
U
SCModel<T,KEY,COMP>::_polybound
( std::map<t_mon,C,lt_mon> const& coefmon, std::map<KEY,std::vector<U>,COMP> const& bndbasis,
  int const type )
const
{
  switch( type ){
  case Options::LSB:
    return _polybound_LSB( coefmon, bndbasis );
  case Options::NAIVE: default:
    return _polybound_naive( coefmon, bndbasis );
  }
}

template <typename T, typename KEY, typename COMP>
inline
T
SCModel<T,KEY,COMP>::_monbound
( t_mon const& mon )
const
{
  if( !mon.tord ) return 1.;
  switch( options.BASIS ){
    case Options::CHEB:
      return TOne;
    case Options::MONOM: default:
      return (mon.gcexp()%2? TOne: TZerOne);
  }
}

template <typename T, typename KEY, typename COMP>
inline
double
SCModel<T,KEY,COMP>::_monval
( t_mon const& mon, std::map<KEY,double,COMP> const& x )
const
{
#if 0
  std::cout << std::right << " setvar =  {";
  for( auto const& id : _setvar )
    std::cout << std::right << " " << id;
  std::cout << std::right << " }" << std::endl;
  std::cout << mon.display( options.BASIS ) << std::endl;
#endif

  double monval = 1.;
  for( auto const& [id,ord] : mon.expr ){
#if 0
    std::cout << "(id,ord): " << id << "," << ord << std::endl;
#endif
    if( _setaux.find( id ) != _setaux.end() ) return 0e0; // case an auxiliary is encoutered
    switch( options.BASIS ){
      case Options::CHEB:
        monval *= isequal(_scalvar.at(id),0.)? _refvar.at(id): 
                  mc::cheb((x.at(id)-_refvar.at(id))/_scalvar.at(id),ord);
        break;
      case Options::MONOM:
        monval *= isequal(_scalvar.at(id),0.)? _refvar.at(id): 
                  std::pow((x.at(id)-_refvar.at(id))/_scalvar.at(id),(int)ord);
        break;
    }
  }
  return monval;
}

////////////////////////////////// SCVar ///////////////////////////////////////

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_init
()
{
  _bndpol = nullptr;
  _bndT   = nullptr;
  _bndrem = 0.;
  return;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_cleanup
()
{
  _unset_bndpol();
  _unset_bndT();
  _ndxvar.clear();
  _coefmon.clear();
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_reinit
()
{
  _cleanup();
  _init();
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_unset_bndpol
()
{
  delete _bndpol;
  _bndpol = nullptr;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_set_bndpol
( T const* bndpol )
{
  if( !bndpol ){
    if( _bndpol ) delete _bndpol;
    _bndpol = 0;
  }
  else if( !_bndpol )
    _bndpol = new T( *bndpol );
  else
    *_bndpol = *bndpol;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_set_bndpol
( T const& bndpol )
{
  if( !_bndpol )
    _bndpol = new T( bndpol );
  else
    *_bndpol = bndpol;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_unset_bndT
()
{
  delete _bndT;
  _bndT = nullptr;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_set_bndT
( T const* bndT )
{
  if( !bndT ){
    if( _bndT ) delete _bndT;
    _bndT = 0;
  }
  else if( !_bndT )
    _bndT = new T( *bndT );
  else
    *_bndT = *bndT;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_set_bndT
( T const& bndT )
{
  if( !_bndT )
    _bndT = new T( bndT );
  else
    *_bndT = bndT;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_center()
{
  const double remmid = Op<T>::mid(_bndrem);
  if( remmid == 0. ) return;
  if( _coefmon.empty() || _coefmon.begin()->first.tord ) 
    _coefmon.insert( std::make_pair( t_mon(), remmid ) );
  else
    _coefmon.begin()->second += remmid;
  _bndrem -= remmid;
  if( _bndpol ) *_bndpol += remmid;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>::SCVar
( SCVar<T,KEY,COMP> const& CV )
{
  _init();
  _set( CV );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator=
( SCVar<T,KEY,COMP> const& CV )
{
  _set( CV );
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::_set
( SCVar<T,KEY,COMP> const& CV )
{
  // Same SCVar?
  if( this == &CV ) return *this;

  // Set coefficients and remainder
  _CM      = CV._CM;
  _bndrem  = CV._bndrem;
  _ndxvar  = CV._ndxvar;
  _coefmon = CV._coefmon;

  // Set bounds
  _set_bndpol( CV._bndpol );
  _set_bndT( CV._bndT );

  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>::SCVar
( SCVar<T,KEY,COMP> && CV )
{
#ifdef MC__SCMODEL_TRACE
    std::cerr << "-- SCVar<T,KEY,COMP>( SCVar<T,KEY,COMP> && )\n";
#endif
  _init();
  _set( CV );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator=
( SCVar<T,KEY,COMP> && CV )
{
#ifdef MC__SCMODEL_TRACE
    std::cerr << "-- SCVar<T,KEY,COMP>& operator= ( SCVar<T,KEY,COMP> && )\n";
#endif
  _set( CV );
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::_set
( SCVar<T,KEY,COMP> && CV )
{
  // Same SCVar?
  if( this == &CV ) return *this;

  // Set coefficients and remainder
  std::swap( _CM      , CV._CM      );
  std::swap( _bndrem  , CV._bndrem  );
  std::swap( _ndxvar  , CV._ndxvar  );
  std::swap( _coefmon , CV._coefmon );

  // Set bounds
  std::swap( _bndpol  , CV._bndpol  );
  std::swap( _bndT    , CV._bndT    );

  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>::SCVar
( SCModel<T,KEY,COMP>* CM )
: _CM( CM )
{
  _init();
  _set_bndpol( 0. );
  if( _CM->options.MIXED_IA ) _set_bndT( 0. );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>::SCVar
( double const& d, SCModel<T,KEY,COMP>* CM )
: _CM( CM )
{
  _init();
  if( d == 0. ) return;
  _coefmon.insert( std::make_pair( t_mon(), d ) );
  _bndrem = 0.;
  _set_bndpol( d );
  if( !_CM || _CM->options.MIXED_IA ) _set_bndT( d );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator=
( double const& d )
{
  _reinit();
  _CM = nullptr;
  if( d != 0. ) _coefmon.insert( std::make_pair( t_mon(), d ) );
  _bndrem = 0.;
  _set_bndpol( d );
  _set_bndT( d );
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>::SCVar
( T const& B, SCModel<T,KEY,COMP>* CM )
: _CM( CM )
{
  _init();
  double const midB = Op<T>::mid(B);
  _coefmon.insert( std::make_pair( t_mon(), midB ) );
  _bndrem = B - midB;
  _set_bndpol( midB );
  if( !_CM || _CM->options.MIXED_IA ) _set_bndT( B );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator=
( T const& B )
{
  _reinit();
  _CM = nullptr;
  double const midB = Op<T>::mid(B);
  _coefmon.insert( std::make_pair( t_mon(), midB ) );
  _bndrem = B - midB;
  _set_bndpol( midB );
  _set_bndT( B );
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>::SCVar
( SCModel<T,KEY,COMP>* CM, KEY const& id, T const& dom )
: _CM( CM )
{
  _init();
  _set( id, dom );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::_set
( KEY const& id, T const& dom, bool const updt )
{
  if( !_CM ) throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::INIT );  

  // Keep data for variable #ivar in model environment
  if( updt ) _CM->_set( id, dom );

  // Populate model variable
  _ndxvar.clear();
  _ndxvar.insert( id );
  _coefmon.clear();
  _coefmon.insert( std::make_pair( t_mon(), _refvar( id ) ) );
  if( _CM->_maxord && !isequal( _scalvar( id ), 0. ) ){
    _coefmon.insert( std::make_pair( t_mon( id ), _scalvar( id ) ) );
    _set_bndpol( _bndvar( id ) );
    _bndrem = 0.;
  }
  else{
    _set_bndpol( _refvar( id ) );
    _bndrem = _bndvar( id ) - _refvar( id );
  }
  if( _CM->options.MIG_USE ) simplify( _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );
#if 0
  std::cout << "coefmon size: " << _coefmon.size() << std::endl;
#endif

  // Interval bounds
  if( _CM->options.MIXED_IA ) _set_bndT( _bndvar( id ) );
  else                        _unset_bndT();
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
double
SCVar<T,KEY,COMP>::P
( std::map<KEY,double,COMP> const& x )
const
{
  assert( _CM || _coefmon.size() <= 1 );
  double Pval = 0.;
  for( auto const& [mon,coef] : _coefmon ){
    if( !mon.tord ) Pval += coef;
    else            Pval += coef * _CM->_monval( mon, x );
  }
  return Pval;
}

template <typename T, typename KEY, typename COMP>
inline
double
SCVar<T,KEY,COMP>::constant
( bool const reset )
{
  auto it_0 = ( _coefmon.empty() || _coefmon.begin()->first.tord? _coefmon.end(): _coefmon.begin() );
  double coefcst = ( it_0 == _coefmon.end()? 0.: it_0->second );
  if( reset && it_0 != _coefmon.end() ){
    _coefmon.erase( it_0 );
    if( _bndpol ) *_bndpol -= coefcst;
    if( _bndT )   *_bndT -= coefcst;
  }
  return coefcst;
}

template <typename T, typename KEY, typename COMP>
inline
double
SCVar<T,KEY,COMP>::linear
( KEY const& id, bool const reset )
{
  auto it_i = ( !nord() || _coefmon.empty()? _coefmon.end(): _coefmon.find( t_mon( id ) ) );
  double coeflin = ( it_i == _coefmon.end() || isequal(_scalvar( id ),0.)? 0.: it_i->second / _scalvar( id ) );
  if( reset && it_i != _coefmon.end() ){
    _coefmon.erase( it_i );
    _unset_bndpol();
    _unset_bndT();
  }
  return coeflin;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_scale
( typename t_var::const_iterator itvar, T const& bndvar, t_poly& coefmon )
const
{
  // Nothing to do if model _CM is nullptr, or variable range X did not change
  if( !_CM
   || ( isequal( Op<T>::l(bndvar), Op<T>::l(_bndvar(*itvar)) )
     && isequal( Op<T>::u(bndvar), Op<T>::u(_bndvar(*itvar)) ) ) ) return;

  // Get coefficients in univariate polynomial representation w.r.t variable *itvar
  std::vector< SCVar<T,KEY,COMP> > veccoef( nord()+1, SCVar<T,KEY,COMP>(_CM) );
  for( auto const& mon : coefmon ) _CM->_svec1Dfull( itvar, mon, veccoef );
  for( auto&& coef : veccoef ){
    coef._unset_bndpol();
    coef._unset_bndT();
  }
#ifdef MC__SCMODEL_DEBUG_SCALE
  _CM->_sdisp1Dfull( veccoef, itvar, "Var #i: " );
#endif
 
  // Nothing to scale if independent of current variable #i
  bool nodep = true;
  for( unsigned k=1; nodep && k<=nord(); k++ )
    if( !veccoef[k].coefmon().empty() ) nodep = false;
  if( nodep ) return;

  // Compose with rescaled inner variable
  SCVar<T,KEY,COMP> cvvar( _CM ); cvvar._set( *itvar, bndvar, false );
#ifdef MC__SCMODEL_DEBUG_SCALE
  std::cout << "cvvar[" << *itvar << "]:" << cvvar;
#endif
  if( !isequal(_scalvar(*itvar),0.) ){
    cvvar -= _refvar(*itvar);
    cvvar *= Op<T>::diam(bndvar) / (2.*_scalvar(*itvar) );
    cvvar += Op<T>::mid(bndvar);
  }
#ifdef MC__SCMODEL_DEBUG_SCALE
  std::cout << "cvvar[" << *itvar << "]:" << cvvar;
#endif
  cvvar = cvvar._rescale( _scalvar(*itvar), _refvar(*itvar) );
#ifdef MC__SCMODEL_DEBUG_SCALE
  std::cout << "cvvar[" << *itvar << "]:" << cvvar;
#endif
  coefmon = _CM->_composition( veccoef, nord(), cvvar )._coefmon;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::scale
( KEY const& id, T const& dom )
{
  auto itvar = _ndxvar.find( id );
  if( itvar != _ndxvar.end() ){
    _scale( itvar, dom, _coefmon );
    _unset_bndpol();
    _unset_bndT();
  }
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::scale
( std::map<KEY,T,COMP> const& dom )
{
  // Return *this if null pointer to model _CM or variable ranges X
  if( dom.empty() || !_CM ) return *this;
  for( auto const& id : _ndxvar ) scale( id, dom[id] );
  if( _CM && _CM->options.MIG_USE ) simplify( _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
typename SCVar<T,KEY,COMP>::t_poly
SCVar<T,KEY,COMP>::unscale
()
const
{
  // Return *this if null pointer to model _CM or variable ranges X
  if( !_CM ) return _coefmon;
  t_poly coefmon = _coefmon;
  for( auto const& id : _ndxvar ) _scale( id, T(-1,1), coefmon );
  if( _CM && _CM->options.MIG_USE ) simplify( _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );
  return coefmon;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::lift
( SCModel<T,KEY,COMP>* SCM, double const& ATOL, double const& RTOL )
{
  double const remrad = 0.5*Op<T>::diam(_bndrem);
//#ifdef MC__SCMODEL_DEBUG_LIFT
  if( Op<T>::abs(_polybound()) < 1e-10 )
    std::cout << "polybound magnitude is: " << _polybound() << std::endl;
  std::cout << remrad << " < " << 0.5*RTOL*Op<T>::diam(_polybound()) + ATOL << " : "
            << ( remrad < 0.5*RTOL*Op<T>::diam(_polybound()) + ATOL ) << std::endl;
//#endif
  if( !_CM->_maxord || ( remrad < 0.5*RTOL*Op<T>::diam(_polybound()) + ATOL + machprec() ) ) return *this;
  _center();

  // Add auxiliary variable
  KEY const& id = SCM->append_aux();
  _ndxvar.insert( id );
  _coefmon.insert( std::make_pair( t_mon( id ), remrad ) );
  _bndrem = 0.;
  if( _bndpol ) *_bndpol += remrad * SCModel<T,KEY,COMP>::TOne;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::project
( bool const reset )
{
  // Nothing to do
  if( _coefmon.empty() || !_CM || _CM->_setaux.empty() ) return *this;

  // Detect auxiliary variables and eliminate
  for( auto it=_coefmon.begin(); it!=_coefmon.end(); ){
    if( !_coefmon.begin()->first.tord ) continue;
    auto& [mon,coef] = *it;
    
    // Detect auxiliary variable in monomial
    t_mon monaux;
    for( auto const& aux : _CM->_setaux ){
      auto itaux = mon.expr.find( aux );
      if( itaux == mon.expr.end() ) continue;
      monaux += smon( itaux->first, itaux->second );
    }
    if( monaux.empty() ){ ++it; continue; }

    // Insert reduced monomial
    t_mon const monred = mon - monaux;
    T const monbound = _CM->_monbound( monaux );
    double const monmid = Op<T>::mid(monbound);
    _bndrem += coef * ( _CM->_monbound( mon ) - monmid );
    if( monmid != 0. ){
      double const coefred = coef * monmid;
      auto [itmon, ins] = _coefmon.insert( std::make_pair( monred, coefred ) );
      if( !ins ) itmon->second += coefred;
    }
    it = _coefmon.erase( it );
  }

  // Remove dependencies
  for( auto const& aux : _CM->_setaux ) _ndxvar.erase( aux );
  if( reset ) _CM->reset_aux();

  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::project
( KEY const& id )
{
  // Nothing to do
  if( _coefmon.empty() || !_CM || _CM->_setaux.empty() ) return *this;

  // Detect auxiliary variables and eliminate
  for( auto it=_coefmon.begin(); it!=_coefmon.end(); ){
    if( !_coefmon.begin()->first.tord ) continue;
    auto const& [mon,coef] = *it;

    // Detect auxiliary variable in monomial
    auto itaux = mon.expr.find( id );
    if( itaux == mon.expr.end() ){ ++it; continue; }
    t_mon const monaux = smon( itaux->first, itaux->second );

    // Insert reduced monomial
    t_mon const monred = mon - monaux;
    T const monbound = _CM->_monbound( monaux );
    double const monmid = Op<T>::mid(monbound);
    _bndrem += coef * ( _CM->_monbound( mon ) - monmid );
    if( monmid != 0. ){
      double const coefred = coef * monmid;
      auto [itmon, ins] = _coefmon.insert( std::make_pair( monred, coefred ) );
      if( !ins ) itmon->second += coefred;
    }
    it = _coefmon.erase( it );
  }

  // Remove dependencies
  _ndxvar.erase( id );

  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::simplify
( double const& ATOL, double const& RTOL, int const TORD )
{
  if( _coefmon.empty() ) return *this;
  for( auto it=_coefmon.begin(); it!=_coefmon.end(); ){
    auto const& [mon,coef] = *it;
    // Eliminate any terms with zero coefficient
    if( coef == 0e0 ){
      it = _coefmon.erase( it );
      continue;
    }
    // Eliminate any non-constant terms with small enough coefficient or large enough total order
    double const THRES = ( RTOL>0? 0.5*RTOL*Op<T>::diam(_polybound()): 0e0 ) + ATOL;
    if( ( mon.tord && std::fabs(coef) <= THRES ) || ( TORD >= 0 && (int)mon.tord > TORD ) ){
      _bndrem += coef * _CM->_monbound( mon );
      _unset_bndpol();
      it = _coefmon.erase( it );
      continue;
    }
    ++it; // only increment if current element was not erased
  }
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::_simplify
( typename SCVar<T,KEY,COMP>::t_poly::iterator itmon,
  double const& ATOL, double const& RTOL, int const TORD )
{
  auto const& [mon,coef] = *itmon;

  // Eliminate any terms with zero coefficient
  if( coef == 0e0 ){
    _coefmon.erase( itmon );
    return *this;
  }
  
  // Eliminate any non-constant terms with small enough coefficient or large enough total order
  double const THRES = ( RTOL>0? 0.5*RTOL*Op<T>::diam(_polybound()): 0e0 ) + ATOL;
  if( ( mon.tord && std::fabs(coef) <= THRES ) || ( TORD >= 0 && (int)mon.tord > TORD ) ){
    _bndrem += coef * _CM->_monbound( mon );
    _unset_bndpol();
    _coefmon.erase( itmon );
  }
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
T
SCVar<T,KEY,COMP>::_simplify_monomial
( typename SCVar<T,KEY,COMP>::t_poly& coefmon, bool const scaled,
  double const& ATOL, double const& RTOL, int const TORD )
const
{
  T bndrem( 0e0 );
  if( coefmon.empty() ) return bndrem;
  
  for( auto it=coefmon.begin(); it!=coefmon.end(); ){
    auto const& [mon,coef] = *it;

    // Eliminate any terms with zero coefficient
    if( coef == 0e0 ){
      it = coefmon.erase( it );
      continue;
    }

    // Eliminate any non-constant terms with small enough coefficient or large enough total order
    double const THRES = ( RTOL>0? 0.5*RTOL*Op<T>::diam(_polybound()): 0e0 ) + ATOL;
    if( ( mon.tord && THRES > 0e0 && std::fabs(coef) <= THRES ) || ( TORD >= 0 && (int)mon.tord > TORD ) ){
      // compute monomial bound
      if( scaled ) bndrem += coef * _CM->_monbound( mon );
      else{
        T bndmon( 1e0 );    
        for( auto const& [var,ord] : mon.expr )
          bndmon *= Op<T>::pow( _bndvar(var), (int)ord );
        bndrem += bndmon * coef;
      }
      it = coefmon.erase( it );
      continue;
    }
    ++it; // only increment if current element was not erased
  }
  return bndrem;
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_simplify
( typename SCVar<T,KEY,COMP>::t_poly& coefmon )
const
{
  if( coefmon.empty() ) return;
  for( auto it=coefmon.begin(); it!=coefmon.end(); ){
    // Eliminate any terms with zero coefficient
    if( it->second == 0e0 ){
      it = coefmon.erase( it );
      continue;
    }
    ++it; // only increment if current element was not erased
  }
}

template <typename T, typename KEY, typename COMP>
inline
void
SCVar<T,KEY,COMP>::_to_monomial
( KEY const& id, typename SCVar<T,KEY,COMP>::t_poly& coefmon )
const
{
  // Get coefficients in univariate polynomial representation w.r.t variable id
  std::vector<t_poly> veccoef( nord()+1, t_poly() );
  for( auto [mon,coef] : coefmon ){
    auto ie = mon.expr.find( id );
    if( ie == mon.expr.end() ) // no dependence on variable id 
      veccoef[ 0 ].insert( std::make_pair( mon, coef ) );
    else{
      auto const& iord = ie->second;
      t_mon monmod( mon.tord - iord, mon.expr );
      monmod.expr.erase( id ); // remove T[id] entry
      veccoef[ iord ].insert( std::make_pair( monmod, coef ) );
    }
  }
 
  // Nothing to convert if independent of current variable #i
  bool nodep = true;
  for( unsigned k=1; nodep && k<=nord(); k++ )
    if( !veccoef[k].empty() ) nodep = false;
  if( nodep ) return;

  // Convert current variable #i to monomial
  // Based on SCONCM function (http://www.netlib.org/math/docpdf/ch11-03.pdf)
  double TP = 1e0;
  // do 20 J = 0, N-2
  for( int j=0; j<(int)nord()-1; j++ ){
    // do 10 I = N-2, J, -1
    for( int i=(int)nord()-2; i>=j; i-- ){
      // COEFF(I) = COEFF(I) - COEFF(I+2)
      for( auto& [mon,coef] : veccoef[i+2] ){
        auto [itmon, ins] = veccoef[i].insert( std::make_pair( mon, -coef ) );
        if( !ins ) itmon->second -= coef;
      }
    // 10    continue
    }
    // COEFF(J+1) = .5E0 * COEFF(J+1)
    for( auto& [mon,coef] : veccoef[j+1] ) coef /= 2e0;
    // COEFF(J) = TP * COEFF(J)
    for( auto& [mon,coef] : veccoef[j] ) coef *= TP;
    // TP = 2.E0 * TP
    TP *= 2e0;
  // 20 continue
  }
  // COEFF(N) = TP * COEFF(N)
  for( auto& [mon,coef] : veccoef[nord()] ) coef *= TP;
  // COEFF(N-1) = TP * COEFF(N-1)
  for( auto& [mon,coef] : veccoef[nord()-1] ) coef *= TP;

  // Collect monomials back together into <a>coefmon</a>
  coefmon = veccoef[0];
  for( unsigned iord=1; iord<=nord(); iord++ ){
    for( auto const& [mon,coef] : veccoef[iord] ){
      t_mon monmod( mon.tord + iord, mon.expr );
      monmod.expr[ id ] = iord;
      coefmon[ monmod ] = coef;
    }
  }
}

template <typename T, typename KEY, typename COMP>
inline
typename SCVar<T,KEY,COMP>::t_poly
SCVar<T,KEY,COMP>::to_monomial
( bool const scaled )
const
{
  if( !_CM || _coefmon.empty() || !nord() ) return _coefmon;

  // Convert to monomial form
  t_poly coefmon = _coefmon;
  if( !scaled ){
    for( auto itvar=_ndxvar.cbegin(); itvar!=_ndxvar.cend(); ++itvar )
      _scale( itvar, SCModel<T,KEY,COMP>::TOne, coefmon );
  }
  
  for( auto const& id : _ndxvar ){
    _to_monomial( id, coefmon );
    _simplify( coefmon );
  }
  
  return coefmon;
}

template <typename T, typename KEY, typename COMP>
inline
std::pair<typename SCVar<T,KEY,COMP>::t_poly,T>
SCVar<T,KEY,COMP>::to_monomial
( bool const scaled, double const& ATOL, double const& RTOL, int const TORD )
const
{
  auto&& coefmon = to_monomial( scaled );
  auto&& bndrem  = _simplify_monomial( coefmon, scaled, ATOL, RTOL, TORD );
  return std::make_pair( coefmon, bndrem );
}

template <typename T, typename KEY, typename COMP>
inline
std::string
SCVar<T,KEY,COMP>::display
( t_poly coefmon, int const& BASIS, int const& IDISP )
const
{
  std::ostringstream out;
  out << std::endl << std::scientific << std::setprecision(IDISP) ;

  // Sparse multivariate polynomial
  for( auto const& [mon,coef] : coefmon )
    out << std::right << std::setw(IDISP+7) << coef << "  " << std::setw(2) << mon.tord << "  "
        << mon.display( BASIS ) << std::endl;

  return out.str();
}

template <typename T, typename KEY, typename COMP>
inline
std::ostream&
operator<<
( std::ostream& out, SCVar<T,KEY,COMP> const& CV )
{
  unsigned IDISP = CV._CM? CV._CM->options.DISPLAY_DIGITS: 7;
  int BASIS = CV._CM? CV._CM->options.BASIS: SCModel<T,KEY,COMP>::Options::CHEB;
  out << CV.display( CV._coefmon, BASIS, IDISP )
      << std::scientific << std::setprecision(IDISP) << std::right;

  // Remainder term
  out << std::right << "   R     =  "
      << "[" << std::setw(IDISP+7) << Op<T>::l(CV._bndrem)
      << "," << std::setw(IDISP+7) << Op<T>::u(CV._bndrem) << "]"
      << std::endl;

  // Range bounder
  out << std::right << "   B     =  "
      << "[" << std::setw(IDISP+7) << Op<T>::l(CV.B())
      << "," << std::setw(IDISP+7) << Op<T>::u(CV.B()) << "]"
      << std::endl;

  // Index set
#if 0
  out << std::right << "   I     =  {";
  for( auto const& ndx : CV._ndxvar )
    out << std::right << " " << ndx;
  out << std::right << " }" << std::endl;
#endif
  return out;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator+
( SCVar<T,KEY,COMP> const& CV )
{
  return CV;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator+=
( SCVar<U,KEY,COMP> const& CV )
{
  if( _CM && CV._CM && _CM != CV._CM )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::MODEL );
  if( CV._CM && !_CM ) _CM = CV._CM;

  _ndxvar.insert( CV._ndxvar.begin(), CV._ndxvar.end() );
  for( auto&& [mon,coef] : CV._coefmon ){
    // No warm-start with map::insert unfortunately...
    auto [itmon, ins] = _coefmon.insert( std::make_pair( mon, coef ) );
    if( !ins ) itmon->second += coef;
  }
  if( _CM && _CM->options.MIG_USE ) simplify( _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );

  _bndrem += CV._bndrem;
  _unset_bndpol();
  if( _bndT && CV._bndT ) *_bndT += *CV._bndT;
  else _unset_bndT();

  return *this;
}

template <typename T, typename KEY, typename COMP>
inline SCVar<T,KEY,COMP>
operator+
( SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2 )
{
  if( CV1.nmon() >= CV2.nmon() ){
    SCVar<T,KEY,COMP> CV3( CV1 );
    CV3 += CV2;
    return CV3;
  }
  
  SCVar<T,KEY,COMP> CV3( CV2 );
  CV3 += CV1;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator +=
( double const& c )
{
  if( c == 0. ) return *this;
  auto [itcst, ins] = _coefmon.insert( std::make_pair( t_mon(), c ) );
  if( !ins ){
    itcst->second += c;
    if( _CM && _CM->options.MIG_USE ) _simplify( itcst, _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );
  }
  if( _bndpol ) *_bndpol += c;
  if( _bndT )   *_bndT += c;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator+
( SCVar<T,KEY,COMP> const& CV1, double const& c )
{
  SCVar<T,KEY,COMP> CV3( CV1 );
  CV3 += c;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator+
( double const& c, SCVar<T,KEY,COMP> const& CV2 )
{
  SCVar<T,KEY,COMP> CV3( CV2 );
  CV3 += c;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator+=
( U const& B )
{
  if( Op<T>::abs(B) == 0. ) return *this;
  _bndrem += B;
  _center();
  if( _bndT ) *_bndT += B;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator+
( SCVar<T,KEY,COMP> const& CV1, T const& B )
{
  SCVar<T,KEY,COMP> CV3( CV1 );
  CV3 += B;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator+
( T const& B, SCVar<T,KEY,COMP> const& CV2 )
{
  SCVar<T,KEY,COMP> CV3( CV2 );
  CV3 += B;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator-
( SCVar<T,KEY,COMP> const& CV )
{
  //std::cout << "CV:" << CV;
  SCVar<T,KEY,COMP> CV2;
  CV2.set( CV._CM );
  CV2._ndxvar = CV._ndxvar;
  for( auto& [mon,coef] : CV._coefmon )
    CV2._coefmon.insert( std::make_pair( mon, -coef ) );
  CV2._bndrem = - CV._bndrem;
  if( CV._bndpol ) CV2._set_bndpol( - *CV._bndpol );
  if( CV._bndT )   CV2._set_bndT( - *CV._bndT );
  //std::cout << "-CV:" << CV2;
  return CV2;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator-=
( SCVar<U,KEY,COMP> const& CV )
{
  if( _CM && CV._CM && _CM != CV._CM )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::MODEL );
  if( CV._CM && !_CM ) _CM = CV._CM;

  _ndxvar.insert( CV._ndxvar.begin(), CV._ndxvar.end() );
  for( auto& [mon,coef] : CV._coefmon ){
    // No warm-start with map::insert unfortunately...
    auto [itmon, ins] = _coefmon.insert( std::make_pair( mon, -coef ) );
    if( !ins ) itmon->second -= coef;
  }

  _bndrem -= CV._bndrem;
  _unset_bndpol();
  if( _bndT && CV._bndT ) *_bndT -= *CV._bndT;
  else _unset_bndT();
  if( _CM && _CM->options.MIG_USE ) simplify( _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );

  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator-
( SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2 )
{
  if( CV1.nmon() >= CV2.nmon() ){
    SCVar<T,KEY,COMP> CV3( CV1 );
    CV3 -= CV2;
    return CV3;
  }

  SCVar<T,KEY,COMP> CV3( -CV2 );
  CV3 += CV1;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator-=
( double const& c )
{
  if( c == 0. ) return *this;
  auto [itcst, ins] = _coefmon.insert( std::make_pair( t_mon(), -c ) );
  if( !ins ){
    itcst->second -= c;
    if( _CM && _CM->options.MIG_USE ) _simplify( itcst, _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );
  }
  if( _bndpol ) *_bndpol -= c;
  if( _bndT )   *_bndT -= c;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator-
( SCVar<T,KEY,COMP> const& CV1, double const& c )
{
  SCVar<T,KEY,COMP> CV3( CV1 );
  CV3 -= c;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator-
( double const& c, SCVar<T,KEY,COMP> const& CV2 )
{
  SCVar<T,KEY,COMP> CV3( -CV2 );
  CV3 += c;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
template <typename U>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator-=
( U const& I )
{
  if( Op<T>::abs(B) == 0. ) return *this;
  *_bndrem -= B;
  _center();
  if( _bndT ) *_bndT -= B;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline SCVar<T,KEY,COMP>
operator-
( SCVar<T,KEY,COMP> const& CV1, T const& B )
{
  SCVar<T,KEY,COMP> CV3( CV1 );
  CV3 -= B;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator-
( T const& B, SCVar<T,KEY,COMP> const& CV2 )
{
  SCVar<T,KEY,COMP> CV3( -CV2 );
  CV3 += B;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator*=
( SCVar<T,KEY,COMP> const& CV )
{
  if( this == &CV ){
    *this = sqr( CV );
    return *this;
  }
   
  if( _CM && CV._CM && _CM != CV._CM )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::MODEL );
  if( CV._CM && !_CM ) _CM = CV._CM;

  // Remainder propagation
  T R1  = bound() * CV._bndrem + CV._polybound() * _bndrem;
  T R2  = _polybound() * CV._bndrem + CV.bound() * _bndrem;
  T rem = 0.;

  // Coefficient maps for first participating variable
  _ndxvar.insert( CV._ndxvar.begin(), CV._ndxvar.end() );
  auto itvar = _ndxvar.begin();
  std::map<unsigned,t_poly> sp1map, sp2map;
  for( auto&& mon : _coefmon ) _CM->_svec1D( itvar, mon, sp1map );
#ifdef MC__SCMODEL_DEBUG_SPROD
  _CM->_sdisp1D( sp1map, itvar, "Poly #1: " );
#endif
  for( auto&& mon : CV._coefmon ) _CM->_svec1D( itvar, mon, sp2map );
#ifdef MC__SCMODEL_DEBUG_SPROD
  _CM->_sdisp1D( sp2map, itvar, "Poly #2: " );
#endif

  // Recursive product of univariate Chebyshev polynomials
  _CM->_sprod1D( sp1map, sp2map, _coefmon, rem, _ndxvar, itvar );
  _bndrem = 0.;
  operator+=( rem );
#ifdef MC__SCMODEL_DEBUG_SPROD
  _unset_bndpol();
  std::cout << "Product of mid parts:" << *this;
#endif

  // Remainder propagation
  if( !Op<T>::inter( rem, R1, R2) )
    rem = ( Op<T>::diam(R1) < Op<T>::diam(R2)? R1: R2 );
  operator+=( rem );
#ifdef MC__SCMODEL_DEBUG_SPROD
  std::cout << "bndrem = " << _bndrem << std::endl;
#endif
 
  _unset_bndpol();
  if( _bndT && CV._bndT ) *_bndT *= *CV._bndT;
  else _unset_bndT();
  if( _CM->options.MIG_USE ) simplify( _CM->options.MIG_ATOL, _CM->options.MIG_RTOL );
  if( _CM->options.LIFT_USE ) lift( _CM, _CM->options.LIFT_ATOL, _CM->options.LIFT_RTOL );

#ifdef MC__SCMODEL_DEBUG_SPROD
  std::cout << "Product model:" << *this;
#endif
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator*
( SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2 )
{
  if( &CV1 == &CV2 ) return sqr( CV1 );
  if( CV1.nmon() >= CV2.nmon() ){
    SCVar<T,KEY,COMP> CV3( CV1 );
    CV3 *= CV2;
    return CV3;
  }

  SCVar<T,KEY,COMP> CV3( CV2 );
  CV3 *= CV1;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator*=
( double const& c )
{
  if( c == 0. ){ *this = 0.; return *this; }
  if( c == 1. ) return *this;
  for( auto& [mon,coef] : _coefmon ) coef *= c;
  _bndrem *= c;
  if( _bndpol ) *_bndpol *= c;
  if( _bndT ) *_bndT *= c;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator*
( SCVar<T,KEY,COMP> const& CV1, double const& c )
{
  SCVar<T,KEY,COMP> CV3( CV1 );
  CV3 *= c;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator*
( double const& c, SCVar<T,KEY,COMP> const& CV2 )
{
  SCVar<T,KEY,COMP> CV3( CV2 );
  CV3 *= c;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator*=
( T const& B )
{
  if( Op<T>::abs(B) == 0. ){ *this  = 0.; return *this; }
  double const Bmid = Op<T>::mid(B);
  T const bndmod = bound();
  for( auto& [mon,coef] : _coefmon ) coef *= Bmid;
  _bndrem *= Bmid;
  _bndrem += ( B - Bmid ) * bndmod;
  _unset_bndpol();
  if( _bndT ) *_bndT *= B;
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator*
( SCVar<T,KEY,COMP> const& CV1, T const& B )
{
  SCVar<T,KEY,COMP> CV3( CV1 );
  CV3 *= B;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator*
( T const& B, SCVar<T,KEY,COMP> const& CV2 )
{
  SCVar<T,KEY,COMP> CV3( CV2 );
  CV3 *= B;
  return CV3;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
sqr
( SCVar<T,KEY,COMP> const& CV )
{
  if( !CV._CM ) return Op<T>::sqr( CV.B() );

  // Coefficient maps for first participating variable
  auto itvar = CV._ndxvar.begin();
  std::map<unsigned,typename SCVar<T,KEY,COMP>::t_poly> sp1map;
  for( auto&& mon : CV._coefmon ) CV._CM->_svec1D( itvar, mon, sp1map );
#ifdef MC__SPOLYEXPR_DEBUG_SQR
  std::cout << "Var: " << CV;
  CV._CM->_sdisp1D( sp1map, itvar, "Var: " );
#endif

  // Recursive product of univariate Chebyshev polynomials
  SCVar<T,KEY,COMP> CVSQR;
  CVSQR.set( CV._CM );
  CVSQR._ndxvar = CV._ndxvar;
  T rem = 0.;
  CV._CM->_sprod1D( sp1map, sp1map, CVSQR._coefmon, rem, CVSQR._ndxvar, itvar );

  // Remainder propagation
  CVSQR += rem;
#ifdef MC__SCMODEL_DEBUG_SPROD
  CVSQR._unset_bndpol();
  std::cout << "Product of polynomial parts:" << CVSQR;
  std::cout << "Product of uncertainty parts:" << 2. * CV._polybound() * CV._bndrem + Op<T>::sqr( CV._bndrem ) << std::endl;
#endif
  CVSQR += 2. * CV._polybound() * CV._bndrem + Op<T>::sqr( CV._bndrem );

  // Bound propagation
  if( CV._CM->options.MIXED_IA ) CVSQR._set_bndT( Op<T>::sqr(CV.bound()) );
  if( CV._CM->options.MIG_USE ) CVSQR.simplify( CVSQR._CM->options.MIG_ATOL, CVSQR._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CVSQR.lift( CVSQR._CM, CVSQR._CM->options.LIFT_ATOL, CVSQR._CM->options.LIFT_RTOL );

  return CVSQR;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator/=
( SCVar<T,KEY,COMP> const& CV )
{
  *this *= inv(CV);
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator/
( SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2 )
{
  return CV1 * inv(CV2);
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>&
SCVar<T,KEY,COMP>::operator/=
( double const& c )
{
  if( isequal( c, 0. ) )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::DIV );
  if( c == 1. ) return *this;
  *this *= (1./c);
  return *this;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator/
( SCVar<T,KEY,COMP> const& CV, double const& c )
{
  if ( isequal( c, 0. ))
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::DIV );
  if( c == 1. ) return CV;
  return CV * (1./c);
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
operator/
( double const& c, SCVar<T,KEY,COMP> const& CV )
{
  if( c == 0. ) return 0.;
  if( c == 1. ) return inv(CV);
  return inv(CV) * c;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
inv
( SCVar<T,KEY,COMP> const& CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::inv( CV.B() ) );
  if ( Op<T>::l(CV.B()) <= 0. && Op<T>::u(CV.B()) >= 0. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::INV );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return 1./x; };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, true ) ) // with remainder at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::inv( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
sqrt
( SCVar<T,KEY,COMP> const& CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::sqrt( CV.B() ) );
  if ( Op<T>::l(CV.B()) < 0. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::SQRT );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::sqrt( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, true ) ) // with remainder at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::sqrt( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
exp
( SCVar<T,KEY,COMP> const& CV )
{ 
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::exp( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::exp( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, true ) ) // with remainder at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::exp( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
log
( SCVar<T,KEY,COMP> const& CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::log( CV.B() ) );
  if ( Op<T>::l(CV.B()) <= 0. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::LOG );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::log( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, true ) ) // with remainder at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::log( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
xlog
( SCVar<T,KEY,COMP> const& CV )
{
#if 0
  return CV * log( CV );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::xlog( CV.B() ) );
  if ( Op<T>::l(CV.B()) <= 0. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::LOG );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return x*std::log( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::xlog( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
pow
( SCVar<T,KEY,COMP> const& CV, const int n )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::pow( CV.B(), n ) );
  if( n < 0 ) return pow( inv( CV ), -n );
  
  auto LIFT_USE_SAVE = CV._CM->options.LIFT_USE;
  CV._CM->options.LIFT_USE = false;
  SCVar<T,KEY,COMP> CV2( CV._CM->_intpow( CV, n ) );
  CV._CM->options.LIFT_USE = LIFT_USE_SAVE;

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::pow( CV.B(), n ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
pow
( const SCVar<T,KEY,COMP> &CV, double const& a )
{
#if 0
  return exp( a * log( CV ) );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::pow( CV.B(), a ) );
  if ( Op<T>::l(CV.B()) <= 0. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::DPOW );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::pow( x, a ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, true ) ) // with remainder at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::pow( CV.B(), a ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
pow
( const SCVar<T,KEY,COMP> &CV1, const SCVar<T,KEY,COMP> &CV2 )
{
  return exp( CV2 * log( CV1 ) );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
pow
( double const& a, const SCVar<T,KEY,COMP> &CV )
{
  return exp( CV * std::log( a ) );
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
prod
( const unsigned n, const SCVar<T,KEY,COMP>*CV )
{
  switch( n ){
   case 0:  return 1.;
   case 1:  return CV[0];
   default: return CV[0] * prod( n-1, CV+1 );
  }
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
monom
( const unsigned n, const SCVar<T,KEY,COMP>*CV, const unsigned*k )
{
  switch( n ){
   case 0:  return 1.;
   case 1:  return pow( CV[0], (int)k[0] );
   default: return pow( CV[0], (int)k[0] ) * monom( n-1, CV+1, k+1 );
  }
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
cheb
( const SCVar<T,KEY,COMP> &CV, const unsigned n )
{
  switch( n ){
    case 0:  return 1.;
    case 1:  return CV;
    default: break;
  }
  SCVar<T,KEY,COMP> CV2( 2.*(CV*cheb(CV,n-1))-cheb(CV,n-2) );
  
  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::cheb( CV.B(), n ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
cos
( const SCVar<T,KEY,COMP> &CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::cos( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::cos( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::cos( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
sin
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return cos( CV - PI/2. );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::sin( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::sin( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::sin( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
tan
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return sin( CV ) / cos( CV );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::tan( CV.B() ) );
  if ( Op<T>::l(Op<T>::cos(CV.B())) <= 0. && Op<T>::u(Op<T>::cos(CV.B())) >= 0. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::TAN );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::tan( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::tan( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
acos
( const SCVar<T,KEY,COMP> &CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::acos( CV.B() ) );
  if ( Op<T>::l(CV.B()) < -1. || Op<T>::u(CV.B()) > 1. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::ACOS );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::acos( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::acos( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
asin
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return PI/2. - acos( CV );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::asin( CV.B() ) );
  if ( Op<T>::l(CV.B()) < -1. || Op<T>::u(CV.B()) > 1. )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::ASIN );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::asin( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::asin( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
atan
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return asin( CV / sqrt( sqr( CV ) + 1. ) );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::atan( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::atan( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::atan( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
cosh
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return 0.5*(mc::exp(CV)+mc::exp(-CV));
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::cosh( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::cosh( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::cosh( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
sinh
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return 0.5*(mc::exp(CV)-mc::exp(-CV));
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::sinh( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::sinh( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::sinh( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
tanh
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return (mc::exp(2*CV)-1)/(mc::exp(2*CV)+1);
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::tanh( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::tanh( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::tanh( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
erf
( const SCVar<T,KEY,COMP> &CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::erf( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::erf( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::erf( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
erfc
( const SCVar<T,KEY,COMP> &CV )
{
#if 0
  return( 1 - mc::erf(CV) );
#endif

  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::erfc( CV.B() ) );

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::erf( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::erfc( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
fabs
( const SCVar<T,KEY,COMP> &CV )
{
  if( !CV._CM )
    return SCVar<T,KEY,COMP>( Op<T>::fabs( CV.B() ) );
  if ( Op<T>::l(CV.B()) >= 0. )
    return CV;
  if ( Op<T>::u(CV.B()) <= 0. )
    return -CV;

#if 0
  SCVar<T,KEY,COMP> CV2( sqrt( sqr(CV) ) );
  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::fabs( CV.B() ) );
  return CV2;
#endif

  SCVar<T,KEY,COMP> CV2( CV._CM );
  auto const& f = [=]( const double& x ){ return std::fabs( x ); };
  if( ( !CV._CM->options.REMEZ_USE
     || CV._CM->options.REMEZ_MIG > Op<T>::diam(CV.B())
     || !CV._CM->_minimax( f, CV, CV2 ) )
     && !CV._CM->_chebinterp( f, CV, CV2, false ) ) // remainder not at bound
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::COMPOSE );

  if( CV._CM->options.MIXED_IA ) CV2._set_bndT( Op<T>::fabs( CV.B() ) );
  if( CV._CM->options.MIG_USE )  CV2.simplify( CV._CM->options.MIG_ATOL, CV._CM->options.MIG_RTOL );
  if( CV._CM->options.LIFT_USE ) CV2.lift( CV._CM, CV._CM->options.LIFT_ATOL, CV._CM->options.LIFT_RTOL );
  return CV2;
}

template <typename T, typename KEY, typename COMP>
inline
SCVar<T,KEY,COMP>
hull
( SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2 )
{
  // Neither operands associated to SCModel -- Make intersection in T type     
  if( !CV1._CM && !CV2._CM ){
    T R1 = CV1.B();
    T R2 = CV2.B();
    return Op<T>::hull(R1, R2);
  }

  // First operand not associated to SCModel
  else if( !CV1._CM )
    return hull( CV2, CV1 );

  // Second operand not associated to SCModel
  else if( !CV2._CM ){
    SCVar<T,KEY,COMP> CVR = CV1.P();
    return CVR + Op<T>::hull( CV1.R(), CV2._coefmon[0]+CV2._bndrem-CVR.B() );
  }

  // SCModel for first and second operands are inconsistent
  else if( CV1._CM != CV2._CM )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::MODEL );

  // Perform union
  SCVar<T,KEY,COMP> CV1C( CV1 ), CV2C( CV2 ), CVR( CV1._CM );;
  double const eta = CV1._CM->options.REF_POLY;
  T R1C = CV1C.C().R(), R2C = CV2C.C().R(); 
  CV1C.set(T(0.));
  CV2C.set(T(0.));
  T BCVD = (CV1C-CV2C).B();
  CVR = (1.-eta)*CV1C + eta*CV2C + Op<T>::hull( R1C+eta*BCVD, R2C+(eta-1.)*BCVD );

  if( CV1._CM->options.MIXED_IA ) CVR._set_bndT( Op<T>::hull( CV1.B(), CV2.B() ) );
  if( CV1._CM->options.MIG_USE )  CV1.simplify(CV1. _CM->options.MIG_ATOL, CV1._CM->options.MIG_RTOL );
  if( CV1._CM->options.LIFT_USE ) CVR.lift( CV1._CM, CV1._CM->options.LIFT_ATOL, CV1._CM->options.LIFT_RTOL );
  return CVR;
}

template <typename T, typename KEY, typename COMP>
inline
bool
inter
( SCVar<T,KEY,COMP>&CVR, SCVar<T,KEY,COMP> const& CV1, SCVar<T,KEY,COMP> const& CV2 )
{
  // Neither operands associated to SCModel -- Make intersection in T type     
  if( !CV1._CM && !CV2._CM ){
    T R1 = CV1.B();
    T R2 = CV2.B();
    T RR( 0. );
    bool flag = Op<T>::inter(RR, R1, R2);
    CVR = RR;
    return flag;
  }

  // First operand not associated to SCModel
  else if( !CV1._CM )
    return inter( CVR, CV2, CV1 );

  // Second operand not associated to SCModel
  else if( !CV2._CM ){
    // First intersect in T arithmetic
    T B2 = CV2.B(), BR;
    if( CV1._CM->options.MIXED_IA && !Op<T>::inter( BR, CV1.B(), B2 ) )
      return false;

    // Perform intersection in PM arithmetic
    T R1 = CV1.R();
    CVR = CV1.P();
    if( !Op<T>::inter(CVR._bndrem, R1, B2-CVR.B()) )
      return false;
    CVR._center();

    if( CVR._CM->options.MIXED_IA ) CVR._set_bndT( BR );
    else CVR._unset_bndT();
    return true;
  }

  // SCModel for first and second operands are inconsistent
  else if( CV1._CM != CV2._CM )
    throw typename SCModel<T,KEY,COMP>::Exceptions( SCModel<T,KEY,COMP>::Exceptions::MODEL );

  // First intersect in T arithmetic
  T BR;
  if( CV1._CM->options.MIXED_IA && !Op<T>::inter( BR, CV1.B(), CV2.B() ) )
    return false;

  // Perform intersection in PM arithmetic
  SCVar<T,KEY,COMP> CV1C( CV1 ), CV2C( CV2 );
  double const eta = CV1._CM->options.REF_POLY;
  T R1C = CV1C.C().R(), R2C = CV2C.C().R(); 
  CV1C.set(T(0.));
  CV2C.set(T(0.));
  CVR = (1.-eta)*CV1C + eta*CV2C;
  CV1C -= CV2C;
  T BCVD = CV1C.B();
  if( !Op<T>::inter( CVR._bndrem, R1C+eta*BCVD, R2C+(eta-1.)*BCVD ) )
    return false;
  CVR._center();

  if( CV1._CM->options.MIXED_IA ) CVR._set_bndT( BR );
  else                            CVR._unset_bndT();
  if( CV1._CM->options.MIG_USE )  CVR.simplify(CV1. _CM->options.MIG_ATOL, CV1._CM->options.MIG_RTOL );
  if( CV1._CM->options.LIFT_USE ) CVR.lift( CV1._CM, CV1._CM->options.LIFT_ATOL, CV1._CM->options.LIFT_RTOL );
  return true;
}

//! @brief C++ structure for specialization of the mc::Op templated structure for use of mc::SCVar in DAG evaluation and as template parameter in other MC++ types
template <typename T, typename KEY, typename COMP>
struct Op< mc::SCVar<T,KEY,COMP> >
{
  typedef mc::SCVar<T,KEY,COMP> CV;
  static CV point( double const& c ) { return CV(c); }
  static CV zeroone() { return CV( mc::Op<T>::zeroone() ); }
  static void I(CV& x, const CV&y) { x = y; }
  static double l(const CV& x) { return mc::Op<T>::l(x.B()); }
  static double u(const CV& x) { return mc::Op<T>::u(x.B()); }
  static double abs (const CV& x) { return mc::Op<T>::abs(x.B());  }
  static double mid (const CV& x) { return mc::Op<T>::mid(x.B());  }
  static double diam(const CV& x) { return mc::Op<T>::diam(x.B()); }
  static CV inv (const CV& x) { return mc::inv(x);  }
  static CV sqr (const CV& x) { return mc::sqr(x);  }
  static CV sqrt(const CV& x) { return mc::sqrt(x); }
  static CV exp (const CV& x) { return mc::exp(x);  }
  static CV log (const CV& x) { return mc::log(x);  }
  static CV xlog(const CV& x) { return x*mc::log(x); }
  static CV lmtd(const CV& x, const CV& y) { return (x-y)/(mc::log(x)-mc::log(y)); }
  static CV rlmtd(const CV& x, const CV& y) { return (mc::log(x)-mc::log(y))/(x-y); }
  static CV fabs(const CV& x) { return mc::fabs(x); }
  static CV sin (const CV& x) { return mc::sin(x);  }
  static CV cos (const CV& x) { return mc::cos(x);  }
  static CV tan (const CV& x) { return mc::tan(x);  }
  static CV asin(const CV& x) { return mc::asin(x); }
  static CV acos(const CV& x) { return mc::acos(x); }
  static CV atan(const CV& x) { return mc::atan(x); }
  static CV sinh(const CV& x) { return mc::sinh(x); }
  static CV cosh(const CV& x) { return mc::cosh(x); }
  static CV tanh(const CV& x) { return mc::tanh(x); }
  static CV erf (const CV& x) { return mc::erf(x);  }
  static CV erfc(const CV& x) { return mc::erfc(x); }
  static CV fstep(const CV& x) { return CV( mc::Op<T>::fstep(x.B()) ); }
  static CV bstep(const CV& x) { return CV( mc::Op<T>::bstep(x.B()) ); }
  static CV hull(const CV& x, const CV& y) { return mc::hull(x,y); }
  static CV min (const CV& x, const CV& y) { return mc::Op<T>::min(x.B(),y.B());  }
  static CV max (const CV& x, const CV& y) { return mc::Op<T>::max(x.B(),y.B());  }
  static CV arh (const CV& x, double const& k) { return mc::exp(-k/x); }
  template <typename X, typename Y> static CV pow(const X& x, const Y& y) { return mc::pow(x,y); }
  static CV cheb(const CV& x, const unsigned n) { return mc::cheb(x,n); }
  static CV prod (const unsigned n, const CV* x) { return mc::prod(n,x); }
  static CV monom (const unsigned n, const CV* x, const unsigned* k) { return mc::monom(n,x,k); }
  static bool inter(CV& xIy, const CV& x, const CV& y) { return mc::inter(xIy,x,y); }
  static bool eq(const CV& x, const CV& y) { return mc::Op<T>::eq(x.B(),y.B()); }
  static bool ne(const CV& x, const CV& y) { return mc::Op<T>::ne(x.B(),y.B()); }
  static bool lt(const CV& x, const CV& y) { return mc::Op<T>::lt(x.B(),y.B()); }
  static bool le(const CV& x, const CV& y) { return mc::Op<T>::le(x.B(),y.B()); }
  static bool gt(const CV& x, const CV& y) { return mc::Op<T>::gt(x.B(),y.B()); }
  static bool ge(const CV& x, const CV& y) { return mc::Op<T>::ge(x.B(),y.B()); }
};

} // namespace mc

#include "mcfadbad.hpp"

namespace fadbad
{

//! @brief Specialization of the structure fadbad::Op for use of the type mc::SCVar of MC++ as a template parameter of the classes fadbad::F, fadbad::B and fadbad::T of FADBAD++
template <typename T, typename KEY, typename COMP>
struct Op< mc::SCVar<T,KEY,COMP> >
{ 
  typedef mc::SCVar<T,KEY,COMP> CV;
  typedef double Base;
  static Base myInteger( const int i ) { return Base(i); }
  static Base myZero() { return myInteger(0); }
  static Base myOne() { return myInteger(1);}
  static Base myTwo() { return myInteger(2); }
  static double myPI() { return mc::PI; }
  static CV myPos( const CV& x ) { return  x; }
  static CV myNeg( const CV& x ) { return -x; }
  template <typename U> static CV& myCadd( CV& x, U const&  y ) { return x+=y; }
  template <typename U> static CV& myCsub( CV& x, U const&  y ) { return x-=y; }
  template <typename U> static CV& myCmul( CV& x, U const&  y ) { return x*=y; }
  template <typename U> static CV& myCdiv( CV& x, U const&  y ) { return x/=y; }
  static CV myInv( const CV& x ) { return mc::inv( x ); }
  static CV mySqr( const CV& x ) { return mc::sqr( x ); }
  template <typename X, typename Y> static CV myPow( const X& x, const Y& y ) { return mc::pow( x, y ); }
  //static CV myCheb( const CV& x, const unsigned n ) { return mc::cheb( x, n ); }
  static CV mySqrt( const CV& x ) { return mc::sqrt( x ); }
  static CV myLog( const CV& x ) { return mc::log( x ); }
  static CV myExp( const CV& x ) { return mc::exp( x ); }
  static CV mySin( const CV& x ) { return mc::sin( x ); }
  static CV myCos( const CV& x ) { return mc::cos( x ); }
  static CV myTan( const CV& x ) { return mc::tan( x ); }
  static CV myAsin( const CV& x ) { return mc::asin( x ); }
  static CV myAcos( const CV& x ) { return mc::acos( x ); }
  static CV myAtan( const CV& x ) { return mc::atan( x ); }
  static CV mySinh( const CV& x ) { return mc::sinh( x ); }
  static CV myCosh( const CV& x ) { return mc::cosh( x ); }
  static CV myTanh( const CV& x ) { return mc::tanh( x ); }
  static bool myEq( const CV& x, const CV& y ) { return mc::Op<T>::eq(const_cast<CV*>(&x)->bound(),const_cast<CV*>(&y)->bound()); } 
  static bool myNe( const CV& x, const CV& y ) { return mc::Op<T>::ne(const_cast<CV*>(&x)->bound(),const_cast<CV*>(&y)->bound()); }
  static bool myLt( const CV& x, const CV& y ) { return mc::Op<T>::lt(const_cast<CV*>(&x)->bound(),const_cast<CV*>(&y)->bound()); }
  static bool myLe( const CV& x, const CV& y ) { return mc::Op<T>::le(const_cast<CV*>(&x)->bound(),const_cast<CV*>(&y)->bound()); }
  static bool myGt( const CV& x, const CV& y ) { return mc::Op<T>::gt(const_cast<CV*>(&x)->bound(),const_cast<CV*>(&y)->bound()); }
  static bool myGe( const CV& x, const CV& y ) { return mc::Op<T>::ge(const_cast<CV*>(&x)->bound(),const_cast<CV*>(&y)->bound()); }
};

} // end namespace fadbad

#endif

