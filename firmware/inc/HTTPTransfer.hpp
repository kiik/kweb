/** @file HTTPTransfer.hpp
 *  The Transfer class handles communication state flow.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#ifndef HTTP_TRANSFER
#define HTTP_TRANSFER

#include "HTTP.hpp"

namespace HTTP {

  class Transfer {
  private:
    enum State {
      UINIT,
      INIT,
      SEND_HEAD,
      SEND_BODY,
      READ_HEAD,
      READ_BODY,
      RECVD,
      ERROR,
      DONE,
    } m_state = State::UINIT;

    Handler m_callback;

    // Socket abstraction methods
    int open();
    int write(String*);
    bool available();
    char read();
    void close();

  public:
    Transfer();
    Transfer(Request*);
    ~Transfer();

    int setup(Request*);
    int process();
    int processUntilBlock();

    void start();

    int sendHeader();
    int sendBody();
    int readHeader();
    int readBody();
    int processResponse();

    void setRequest(Request*);
    void setHandler(Handler);

    bool isInProgress();

  private:
    TCPClient m_tcp;

    Request* m_req;
    Response* m_res;

    uint32_t responseTimeout;
  };

}

#endif // HTTP_TRANSFER
