/** @file HTTPResponse.hpp
 *  HTTP Response class declaration.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "HTTP.hpp"

namespace HTTP {

  class Response {
  private:
    bool m_hFlag;
    bool m_hRecv;

    Request* req;

    String* m_header;
    String* m_body;
  public:
    uint16_t statusCode;
    uint32_t contentLength;

    Response();
    Response(Request*);
    ~Response();

    void init();
    void cleanup();

    int digest(char);
    int parseHeader();

    bool isHeaderReceived();
    bool isReceived();

    Request* getRequest();
    String* getBody();
    String* getHeader();
  };

}

#endif // HTTP_RESPONSE_H
