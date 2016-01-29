/** @file HTTPResponse.cpp
 *  HTTP Response class implementation.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#include "HTTP.hpp"

namespace HTTP {

  Response::Response() {
    req = nullptr;
    m_header = nullptr;
    m_body = nullptr;

    init();
  }

  Response::~Response() {
    cleanup();

    if(m_header != nullptr) delete m_header;
    if(m_body != nullptr) delete m_body;
  }

  void Response::init() {
    cleanup();

    if(m_header == nullptr) m_header = new String("");
    if(m_body == nullptr) m_body = new String("");
  }

  void Response::cleanup() {
    m_hFlag = false;
    m_hRecv = false;

    statusCode = 0;
    contentLength = 0;

    if(m_header != nullptr) *m_header = "";
    if(m_body != nullptr) *m_body = "";
  }

  bool Response::isHeaderReceived() { return m_hRecv; }
  bool Response::isReceived() { return false; }

  Request* Response::getRequest() { return req; }

  String* Response::getHeader() { return m_header; }
  String* Response::getBody() { return m_body; }

  int Response::digest(char c) {
    Serial.print(c);
    if(!m_hRecv) {
      if(c == '\n') {
        if(m_hFlag) {
          m_hRecv = true;
          parseHeader();
          if(contentLength == 0) goto DONE;
          m_body->reserve(contentLength + 1);
        } else {
          m_hFlag = true;
        }
      } else if(m_hFlag && c != '\r') {
        m_hFlag = false;
      }
    }

    if(m_hRecv) {
      m_body->concat(c);
      if(contentLength != 0 && m_body->length() >= contentLength) goto DONE;
    } else {
      m_header->concat(c);
    }

    if(m_body->length() > HTTP_RESPONSE_SIZE_LIMIT) {
      log("Response rejected. Data exeeded HTTP_RESPONSE_SIZE_LIMIT value.");
      return -1;
    }

    if(contentLength != 0 && m_body->length() >= contentLength) {
DONE:
      log("Response received");
      return 1;
    }

    return 0;
  }

  int Response::parseHeader() {
    if(m_header == nullptr || *m_header == "") return -1;

    uint32_t o1, o2;
    String data, key;

    if(m_header->indexOf("HTTP") != -1) {
      o1 = m_header->indexOf(" ");
      o2 = m_header->indexOf(" ", o1 + 1);
      data = m_header->substring(o1, o2);
      data.trim();
      statusCode = data.toInt();
    } else {
      Serial.println("[Response::parseHeader]Invalid HTTP response header.");
      return -1;
    }

    key = "Content-Length: ";
    if(m_header->indexOf(key) != -1) {
      o1 = m_header->indexOf(key) + key.length();
      o2 = m_header->indexOf("\n", o1 + 1);

      data = m_header->substring(o1, o2);
      data.replace("\r", "");
      data.trim();

      contentLength = data.toInt();
    }

    return 0;
  }

}
