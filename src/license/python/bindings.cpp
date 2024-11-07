#include <pybind11/pybind11.h>
#include "verify.h"

namespace py = pybind11;

PYBIND11_MODULE(license, m) {
    // Expose the verify function
    m.def("verify", &lic::verify, "Verify the license",
          py::arg("license_file"));
}
