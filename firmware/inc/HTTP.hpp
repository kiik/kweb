/** @file HTTP.hpp
 *  Provides classes for storing
 *  and parsing HTTP data.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#ifndef HTTP_H
#define HTTP_H

#include <functional>

#include "application.h"

#define HTTP_PORT 80
#define HTTP_SOCK_MX 3
#define HTTP_TIMEOUT 10000
#define HTTP_RESPONSE_SIZE_LIMIT 10000

#define KWEB_VERSION "0.0.1-rc0"

#define HTTP_DEBUG

namespace HTTP {

  enum Method {
    GET,
    POST
  };

  enum ContentType {
    NONE,
    JSON,
  };

  class Response;
  class Request;
  class Transfer;

  typedef std::function<int8_t (Request*, Response*)> Handler;

  inline void log(const String msg) {
#ifdef HTTP_DEBUG
    Serial.print("[HTTP::Transfer]");
    Serial.println(msg);
#endif
  }

  void setup();
  void process();

  Transfer* getAvailableSocket();

}

#include "HTTPResponse.hpp"
#include "HTTPRequest.hpp"
#include "HTTPTransfer.hpp"

#endif // HTTP_H
