//=============================================================================
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "cpplapack.h"
using namespace std;

//=============================================================================
/*! main */
int main(int argc, char** argv)
{
  srand(unsigned(time(NULL)));
  int L(4);
  
  //// make drovector x ////
  CPPL::drovector x(L);
  for(int i=0; i<x.l; i++){
	x(i) =double( rand() /(RAND_MAX/10) );
  }
  
  //// print x in two ways ////
  cout << "x =\n" << x << endl;
  for(int i=0; i<x.l; i++){
	cout << "x(" << i << ") =" << x(i) << endl;
  }
  
  //// make drovector y ////
  CPPL::drovector y(x);
  
  //// print y in two ways ////
  cout << "y =\n" << y << endl;
  for(int i=0; i<y.l; i++){
	cout << "y(" << i << ") =" << y(i) << endl;
  }
  
  //// print x+y ////
  cout << "x+y=\n" << x+y << endl;
  
  //// write/read ////
  x.write( "tmp.txt" );
  CPPL::drovector z;
  z.read( "tmp.txt" );
  cout << "z-x =\n" << z-x << "<-Should be zero." << endl;

  return 0;
}
