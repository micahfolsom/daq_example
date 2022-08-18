#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>  // PYBIND11_NUMPY_DTYPE

#include "daq.hpp"
namespace py = pybind11;

PYBIND11_MODULE(daqex_cpp, m) {
  // The module's docstring
  m.doc() = "DAQ example C++ code, with python bindings";

  // C-struct for numpy structured array
  PYBIND11_NUMPY_DTYPE(daqex::DAQData, channel, index, value, timestamp);
  // Make it available in python as a plain old object with attributes
  // Only difference is you can't arbitrarily add attributes
  // Pass "py::dynamic_attr()" as an arg after "DAQData" below to get this
  // capability
  py::class_<daqex::DAQData> daqdata(m, "DAQData");
  daqdata.def(py::init<>())
      // Plain old python object attributes
      .def_readwrite("channel", &daqex::DAQData::channel)
      .def_readwrite("index", &daqex::DAQData::index)
      .def_readwrite("value", &daqex::DAQData::value)
      .def_readwrite("timestamp", &daqex::DAQData::timestamp);

  py::class_<daqex::DAQ> daq(m, "DAQ");
  daq
    .def(py::init<float, float>())
    .def("start", &daqex::DAQ::start)
    .def("stop", &daqex::DAQ::stop)
    .def("configure", &daqex::DAQ::configure, py::arg("pull_rate") = 10.f,
        py::arg("timeout") = 5.f)
    // Can access with "daq.running" without needing the (); can't set (setter
    // is nullptr)
    .def_property("running", &daqex::DAQ::is_running, nullptr)
    .def("get_data", &daqex::DAQ::get_data);
}
