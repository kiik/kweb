/** @file HTTPTransfer.cpp
 *  HTTP Transfer class implementation.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#include "HTTP.hpp"

namespace HTTP {

  Transfer::Transfer() {
    m_req = nullptr;
    m_req = nullptr;
    m_callback = nullptr;
  }

  Transfer::Transfer(Request* req) {
    Transfer();
    setup(req);
  }

  Transfer::~Transfer() {
    m_res = nullptr;
    if(m_req != nullptr) delete m_req;
  }

  int Transfer::open() {
    if(m_tcp.connected()) {
      log("warning: TCP already in use or buffer was not processed. Closing previous connection.");
      m_tcp.flush();
      m_tcp.stop();
    }

    log("Connecting to server...");
    m_tcp.connect(m_req->getAddress(), HTTP_PORT); //TODO: dynamic values
    delay(5);

    if(m_tcp.connected()) {
      responseTimeout = millis() + HTTP_TIMEOUT + 2000;
      return 1;
    } else {
      log("Connection failed!");
      return -1;
    }

    return 0;
  }

  int Transfer::write(String* data) {
#ifdef HTTP_MEMDBG
    Serial.println(String::format("[Transfer::write]Pre %i, %i\n", System.freeMemory(), data->length()));
#endif
    if(data->length() > 0) m_tcp.write((uint8_t*)data->c_str(), data->length());
#ifdef HTTP_MEMDBG
    Serial.println(String::format("[Transfer::write]Post %i\n", System.freeMemory()));
#endif
    return 0;
  }

  bool Transfer::available() {
    return m_tcp.available();
  }

  char Transfer::read() {
    char c = m_tcp.read();
    return c;
  }

  void Transfer::close() {
    m_tcp.stop();
  }

  int Transfer::setup(Request* req) {
    setRequest(req);
    return 0;
  }

  int Transfer::process() {
    int res;

    State prev_state = m_state;
    switch(m_state) {
      case State::INIT:
        res = open();
        if(res < 0) m_state = State::ERROR;
        else if(res > 0) m_state = State::SEND_HEAD;
        break;
      case State::SEND_HEAD:
        res = sendHeader();
        if(res < 0) m_state = State::ERROR;
        else if(res > 0) m_state = State::SEND_BODY;
        break;
      case State::SEND_BODY:
        res = sendBody();
        if(res < 0) m_state = State::ERROR;
        else if(res > 0) {
          responseTimeout = millis() + HTTP_TIMEOUT;
          m_state = State::READ_HEAD;
        }
        break;
      case State::READ_HEAD:
        res = readHeader();
        if(res < 0) m_state = State::ERROR;
        else if(res > 0) {
          responseTimeout = millis() + HTTP_TIMEOUT;
          m_state = State::READ_BODY;
        }
        break;
      case State::READ_BODY:
        res = readBody();
        if(res < 0) m_state = State::ERROR;
        else if(res > 0) m_state = State::RECVD;
        break;
      case State::RECVD:
        res = processResponse();
        if(res < 0) m_state = State::ERROR;
        else if(res > 0) m_state = State::DONE;
        break;
      case State::DONE:
        return 1;
      case State::ERROR:
        return -1;
    }

    //TODO: Failsafes
    switch(m_state) {
      case State::READ_HEAD:
      case State::READ_BODY:
        if(millis() >= responseTimeout) {
          close();
          log("Error: Server response timeout.");
          m_state = State::ERROR;
        }

        if(!m_tcp.connected()) {
          if(m_res->getBody()->length() + m_req->getHeader()->length() <= 0) {
            close();
            log("Error: No response.");
            m_state = State::ERROR;
          }
        }
        break;
    }

    if(prev_state != m_state) {
      log(String::format("Transition to %i", m_state));

      switch(m_state) {
        case State::ERROR:
          log(String::format("Error at %i", prev_state));
          processResponse();
          break;
      }
    }

    return 0;
  }

  int Transfer::processUntilBlock() {
    State st;
    int res;

    do {
      st = m_state;
      res = process();
      if(res < 0) return res;
    } while(m_state != st);

    return res;
  }

  void Transfer::start() {
    if(m_state == State::UINIT) m_state = State::INIT;
  }

  int Transfer::sendHeader() {
    String* h = m_req->getHeader();
    if(h->length() <= 0) {
      close();
      return -1;
    }

    write(h);
    return 1;
  }

  int Transfer::sendBody() {
    if(m_req->method == Method::GET) return 1;

    String* b = m_req->getBody();
    if(b == nullptr) return 1;

    if(m_req->method != Method::GET && b->length() <= 0) {
      close();
      return -1;
    }

#ifdef HTTP_MEMDBG
    Serial.println(String::format("[Transfer::sendBody]Enter %i\n", System.freeMemory()));
#endif
    if(b->length() > 0) write(b);
#ifdef HTTP_MEMDBG
    Serial.println(String::format("[Transfer::sendBody]Exit %i\n", System.freeMemory()));
#endif
    return 1;
  }

  int Transfer::readHeader() {
    if(available()) {
      log("Reading header...");

      int res;
      while(available()) {
        res = m_res->digest(read());
        if(res != 0) {
          close();
          if(res > 0) return 1;
          else return -1;
        }
      }

      if(m_res->isHeaderReceived()) return 1;
    }

    return 0;
  }

  int Transfer::readBody() {
    if(available()) {
      log("Reading body...");

      int res;
      while(available()) {
        res = m_res->digest(read());
        if(res != 0) {
          close();
          if(res > 0) return 1;
          else return -1;
        }
      }

      if(m_res->isReceived()) return 1;
    }

    return 0;
  }

  int Transfer::processResponse() {
    log("Processing response...");
    int res;
    if(m_callback != nullptr) {
      res = m_callback(m_req, m_res);
      if(res < 0) {
        log(String::format("Callback returned with error code %i.", res));
      }
    }

    delete m_req;
    m_req = nullptr;
    m_callback = nullptr;

    return 1;
  }

  void Transfer::setRequest(Request* req) {
    if(m_req != nullptr) {
      delete m_req;
      m_req = nullptr;
    }
    if(m_res != nullptr) m_res = nullptr;

    m_req = req;
    m_res = req->getResponse();
  }

  void Transfer::setHandler(Handler h) {
    m_callback = h;
  }

  bool Transfer::isInProgress() {
    return (m_state != State::DONE) && (m_state != State::ERROR);
  }

}
