#ifndef DAQ_HPP
#define DAQ_HPP

namespace daqex {
  /*! \class DAQ
   *
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
       *  many seconds
       */
      DAQ(float pull_rate = 1.f, float timeout = 5.f);
      ~DAQ();

      void start();
      void stop();
      bool is_running();

    private:
      bool m_fRunning;
      float m_rate;
      float m_timeout;
  };
}

#endif
