#ifndef DAQ_HPP
#define DAQ_HPP
#include <pybind11/numpy.h>

#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace daqex {
/*! \struct DAQData
 * Simple structure holding the data we want to gather from the sensor.
 * These types are used to define the contents of the numpy structured array
 * that's returned by the bindings. */
struct DAQData {
  uint8_t channel;
  uint64_t index;
  float value;
  double timestamp;
};

/*! \class DAQ
 * An example Data AcQuisition (DAQ) object. This should provide a high-level
 * interface to control collection of data from a sensor.
 */
class DAQ {
 public:
  /*! Configure on construction, rather than construct and have an
   * initialize() function; this guarantees we either have a valid object
   * or an exception is thrown. There's a name for this pattern but I can't
   * remember what it is right now!
   *
   * \param pull_rate
   *   Number of times per second to request data from the device
   * \param timeout
   *   DAQ enters a disconnected state if there's no communication for this
   *   many seconds
   */
  DAQ(float pull_rate = 10.f, float timeout = 5.f);
  ~DAQ();

  void start();
  void stop();
  bool is_running() const;
  void configure(float pull_rate = 10.f, float timeout = 5.f);

  /*! Returns all data collected since last call to get_data() and empties
   * the queue. */
  pybind11::array_t<DAQData> get_data();

 private:
  bool m_fRunning;
  /// Rate to request data from the device, in Hz
  float m_rate;
  /// Usually passed to the specific communication API (the socket, or
  /// the serial interface) rather than used ourselves
  float m_timeout;
  /*! Using a queue is a design choice, and there are many options here. I
   * find this to be the simplest - one mutex-protected queue being filled
   * in a separate thread with sensor data, then the caller (your webserver
   * or w/e) can pull the data at whatever interval makes sense. This works
   * fine up to ~GB/sec of data, then you have to start being a little
   * more careful to avoid copies, or to batch them by memcpy()ing instead
   * of doing one DAQData object at a time, for example. */
  std::queue<DAQData> m_dataBuffer;
  std::unique_ptr<std::thread> m_acqThread;
  std::mutex m_acqMutex;

  /*! Acquisition thread. The rate this loops will be fully dependent on
   * the device readout features - if it's USB, probably a few times per
   * second; a LiDAR or IMU over TCP/UDP might be hundreds of Hz. At the
   * end of the day though, it's always the same - loop while m_fRunning,
   * and put the unpacking code in a separate function. */
  void acquire();
  // It helps to keep this separate from acquire(); you really just want
  // that function taking the data from the sensor. This is the function
  // that should actually fill the queue. This takes in the raw data
  // output by the sensor and populates a DAQData object with its contents
  void parse_data(std::string const& raw_data);
  // You'll probably have a bunch of helper functions here for
  // communicating with the device, creating formatted strings for input
  // to a serial device, etc. Just keep it organized - you may have a lot
  // of them!
};
}  // namespace daqex

#endif
