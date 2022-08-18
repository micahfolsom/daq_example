#include "daq.hpp"

#include <iostream>
#include <random>
using namespace std;

namespace daqex {
DAQ::DAQ(float pull_rate, float timeout)
    : m_fRunning(false),
      m_rate(pull_rate),
      m_timeout(timeout),
      m_dataBuffer(),
      m_acqThread(nullptr),
      m_acqMutex() {
  // Do initial setup here - try to communicate with the device, without
  // starting it, if that's possible
  configure(m_rate, m_timeout);
  cout << "DAQ object created and configured" << endl;
}

DAQ::~DAQ() {
  // There's almost always a serial port to close or something, so make sure
  // to clean up here!
  cout << "DAQ object destroyed" << endl;
}

void DAQ::start() {
  m_fRunning = true;
  m_acqThread = make_unique<thread>(&DAQ::acquire, this);
  cout << "DAQ: starting acquisition" << endl;
  return;
}

void DAQ::stop() {
  m_fRunning = false;
  cout << "DAQ: stopping acquisition...";

  // join() will block until acquire() returns, so just be aware, if you're
  // in a slow loop (>>1 sec) situation, this may take that long to unblock,
  // which may not be ideal from the user side
  m_acqThread->join();
  m_acqThread = nullptr;
  cout << "done" << endl;
  return;
}

bool DAQ::is_running() const { return m_fRunning; }

void DAQ::configure(float pull_rate, float timeout) {
  m_rate = pull_rate;
  m_timeout = timeout;

  // Communicate with device; if it fails, or if the configuration is not
  // valid, then there are 2 options, depending on your needs: throw an
  // exception so there's no DAQ state that isn't talking to a device, or
  // add a "m_fConnected" flag to indicate this additional state. This can be
  // nice to have, but increases complexity, since it's one more thing to
  // check for in the other functions. Just depends on how it's being used!
  return;
}

pybind11::array_t<DAQData> DAQ::get_data() {
  cout << "DAQ: retrieving data from buffer" << endl;
  m_acqMutex.lock();
  pybind11::array_t<DAQData> output(m_dataBuffer.size());
  auto ptr = static_cast<DAQData*>(output.request().ptr);
  size_t n = 0;
  while (!m_dataBuffer.empty()) {
    ptr[n] = m_dataBuffer.front();
    m_dataBuffer.pop();
    ++n;
  }
  m_acqMutex.unlock();
  cout << "DAQ: retrieved " << n << " data from buffer" << endl;

  return output;
}

void DAQ::acquire() {
  // We'll make some random data
  auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
  mt19937 mt_rand(seed);
  random_device rd;
  mt19937 generator{rd()};
  uniform_int_distribution<uint8_t> random_channel(0, 255);
  uniform_int_distribution<uint64_t> random_index(0, 10000);
  uniform_real_distribution<float> random_value(0, 100.f);

  // This will break when stop() is called, but you *must* be cognizant of
  // how long sleep()s are happening - if the cycle length is long then it
  // may take awhile to actually stop. If that's the case, you may want to
  // loop faster and downsample the communication calls to every Nth loop so
  // it remains responsive
  while (m_fRunning) {
    stringstream data;
    data << random_channel(generator) << " ";
    data << random_index(generator) << " ";
    data << random_value(generator) << " ";
    data << (double)chrono::high_resolution_clock::now()
                .time_since_epoch()
                .count();
    cout << "DAQ: generated random data\n\t" << data.str() << endl;
    parse_data(data.str());

    // Depending on how long communication and parsing takes, and how much
    // consistency (timing-wise) matters to the device, you may want to keep
    // track of how long and sleep the rest of the tick, like you would do for
    // a game engine. This is rarely needed in my experience with sensors,
    // though
    sleep(1);
    // int interval_us = (int)(1e6 / m_rate);
    // usleep(interval_us);
  }
  return;
}

void DAQ::parse_data(std::string const& raw_data) {
  cout << "DAQ: parsing data (" << raw_data << ")" << endl;
  m_acqMutex.lock();
  DAQData data;
  m_dataBuffer.push(data);
  m_acqMutex.unlock();
  // TODO: actually unpack
  return;
}
}  // namespace daqex
