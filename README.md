# Data AcQuisition (DAQ) Example

Example code demonstrating a programming pattern I've used numerous times
to continuously collect data from a sensor in a way that's both performant
and easy to understand. In radiation detection, we usually refer to these
codes as DAQs. I've used this approach to read TCP/IP, serial, and USB
data, including saturating a gigabit ethernet link with UDP sensor data.

The code defines one C++ class that provides the control and collection
API. Depending on the complexity of the sensor, more classes may be used to
handle configuration or other functionality. The C++ class then has its
interface bound to python for easy access.

This package can be installed with the usual `pip install [-e] .`. If not
using `-e`, it's recommended to use a virtual environment due to problems
with the packaging ecosystem that are beyond my control.

## Using `pybind11`

If you want to know more about the `pybind11` aspect, check out [my example
here](https://github.com/micahfolsom/pybind_scikit_example). TLDR: the
C++ code (along with the bindings) are in `src/cpp`, and the python code is
in `src/daq_example`. The C++ code can be compiled independently using the
`CMakeLists.txt` in the `src/cpp` directory. Otherwise, it will be compiled
(using `CMake` still) when you run `pip install`.
