/*****************************************************************************/
/*                                 noname                                    */
/*****************************************************************************/

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
  int M(4);
  
  CPPL::zcovector x;
  cout << "x || l=" << x.l << ", array=" << x.array << endl;
  
  
  CPPL::zcovector y(M);
  for(int i=0; i<y.l; i++){
    y(i) =complex<double>(rand()/(RAND_MAX/10), rand()/(RAND_MAX/10));
  }
  cout << "y || l=" << y.l << ", array=" << y.array << endl;
  cout << y << endl;
  
  CPPL::zcovector z(y);
  cout << "z || l=" << z.l << ", array=" << z.array << endl;
  cout << z << endl;
  
  return 0;
}

/*****************************************************************************/
