#include <pybind11/pybind11.h>

namespace py = pybind11;

void mc_interval( py::module_ & );
void mc_ffunc( py::module_ & );

PYBIND11_MODULE( mcpy, m )
{

  m.doc() = "Python interface of library MC++";

  mc_interval( m );
  mc_ffunc( m );

}

