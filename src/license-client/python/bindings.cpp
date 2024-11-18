#include <pybind11/pybind11.h>
#include "verify.h"

namespace py = pybind11;

PYBIND11_MODULE(licensepy, m) {
    // Expose the verify function
    m.def("verify", &lic::verify, "Verify the license",
          py::arg("ip"), py::arg("port") = 8442, py::arg("period") = 30);
    m.def("cleanup", &lic::cleanup, "Clean up resoure");
}
