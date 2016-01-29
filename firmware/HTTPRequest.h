/** @file HTTPRequest.h
 *  HTTP Request class declaration.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "HTTP.h"

namespace HTTP {

  class Request {
  private:
    Response m_response;

    String m_addr;

    String* m_header = nullptr;
    String* m_body = nullptr;
  public:
    Method method;
    ContentType contentType;
    String endpoint;

    void setAddress(String addr) { m_addr = addr; }
    String getAddress() { return m_addr; }

    Request();
    Request(String, String* = nullptr, Method = Method::GET, ContentType = ContentType::NONE);
    Request(String, String, String* = nullptr, Method = Method::GET, ContentType = ContentType::NONE);
    ~Request();

    void init();
    void cleanup();

    Response* getResponse();
    String* getHeader();
    String* getBody();
  };

}

#endif // HTTP_REQUEST_H
