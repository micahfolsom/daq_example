#include "daq.hpp"
#include <iostream>
#include <pybind11/numpy.h>
using namespace daqex;
using namespace std;

int main(int, char**) {
  DAQ daq;
  daq.start();
  int count = 0;
  while (count < 50) {
    // We are free to ask for data as frequently as desired by the UI
    pybind11::array_t<DAQData> data = daq.get_data();
    auto ptr = static_cast<DAQData*>(data.request().ptr);
    //cout << "Retrieved " << data.size() << " items" << endl;
    usleep(200000);
    ++count;
  }
  daq.stop();
  return 0;
}
