/** @file HTTPRequest.cpp
 *  HTTP Request class implementation.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#include "HTTP.h"

namespace HTTP {

  Request::Request() {
    m_header = nullptr;
    m_body = nullptr;

    init();
  }

  Request::Request(String url, String* body, Method mth, ContentType cty):
    Request()
  {
    endpoint = url;
    method = mth;
    contentType = cty;

    if(m_body == nullptr) m_body = body;
    else {
      // Reuse memory in this case to lessen segmentation probability
      *m_body = *body;
      delete body;
    }
  }

  Request::Request(String addr, String url, String* body, Method mth, ContentType cty):
    Request(url, body, mth, cty)
  {
    m_addr = addr;
  }

  Request::~Request() {
    cleanup();

    if(m_header != nullptr) delete m_header;
    if(m_body != nullptr) delete m_body;
  }

  void Request::init() {
    cleanup();
  }

  void Request::cleanup() {
    method = Method::GET;
    contentType = ContentType::NONE;
    endpoint = "";

    if(m_header != nullptr) *m_header = "";
    if(m_body != nullptr) *m_body = "";
  }

  String* Request::getHeader() {
    if(m_header == nullptr) m_header = new String("");

    if(*m_header == "") {
      switch(method) {
        case Method::GET:
          m_header->concat("GET ");
          break;
        case Method::POST:
          m_header->concat("POST ");
          break;
      }

      m_header->concat(String::format("%s HTTP/1.1\n", endpoint.c_str()));
      m_header->concat(String::format("Host: %s\n", m_addr.c_str()));
      m_header->concat(String::format("User-Agent: Particle kweb/%s\n", KWEB_VERSION));

      if(method != Method::GET) {
        uint32_t contentLength = m_body->length();

        if(contentLength > 0) {
          switch(contentType) {
            case ContentType::JSON:
              m_header->concat("Content-Type: application/json\n");
              break;
          }
        }

        m_header->concat(String::format("Content-Length: %i\n", contentLength));
      }

      m_header->concat('\n');
    }

    return m_header;
  }

  Response* Request::getResponse() {
    return &m_response;
  }

  String* Request::getBody() {
    if(m_body == nullptr) m_body = new String("");
    return m_body;
  }

}
