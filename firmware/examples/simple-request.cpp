/** @file simple-request.cpp
 *  A simple HTTP GET request example.
 *
 *  @author Meelik Kiik
 *  @date 29. January 2015
 *  @version 0.1
 */

#include "HTTP.hpp"

uint32_t request_time = 0;
uint32_t request_period = 10 * 1000;

int request_data() {
  HTTP::Transfer* trans = HTTP::getAvailableSocket();
  if(trans == nullptr) return -1;

  HTTP::Request* req = new HTTP::Request("google.com", String::format("/"));

  trans->setup(req);
  trans->setHandler([](HTTP::Request* req, HTTP::Response* res) {
    Serial.println("Request received.");

    Serial.println(res->statusCode);
    Serial.println(res->getHeader()->c_str());
    delay(5);
    Serial.println(res->getBody()->c_str());

    return 0;
  });

  trans->start();
  Serial.println("Sending request.");

  return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("simple-request.cpp");

  HTTP::setup();
}

void loop() {
  HTTP::process();

  if(millis() > request_time) {
    request_data();
    request_time = millis() + request_period;
  }
}
