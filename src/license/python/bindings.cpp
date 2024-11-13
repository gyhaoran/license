#include <pybind11/pybind11.h>
#include "verify.h"
#include "utils.h"

namespace py = pybind11;

PYBIND11_MODULE(licensepy, m) {
    // Expose the verify function
    m.def("verify", &lic::verify, "Verify the license",
          py::arg("license_file"));
    m.def("encrypt_info", &lic::encrypt_info, "Encrypt data",
          py::arg("data"), py::arg("key") = "");
    m.def("decrypt_info", &lic::decrypt_info, "Decrypt data",
          py::arg("data"), py::arg("key") = "");
}
