#include "daq.hpp"

DAQ::DAQ(float pull_rate, float timeout): m_rate(pull_rate),
  m_timeout(timeout) {
  }

DAQ::~DAQ() {
}

void DAQ::start() {
  return;
}

void DAQ::stop() {
  return;
}

bool DAQ::is_running() {
  return false;
}
