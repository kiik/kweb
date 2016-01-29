/** @file simple-post.cpp
 *  A simple HTTP POST request example.
 *
 *  @author Meelik Kiik
 *  @date 29. January 2015
 *  @version 0.1
 */

#include "kiikweb.h"

uint32_t post_time = 0;
uint32_t post_period = 10 * 1000;

int32_t database[] = {1, 2, -3, 4, -2};

int post_data() {
  HTTP::Transfer* trans = HTTP::getAvailableSocket();
  if(trans == nullptr) return -1;

  String* body = new String("{'data': [1, 2, -3, 4, -2]}");

  HTTP::Request* req = new HTTP::Request("google.com", String::format("/"), body, HTTP::Method::POST, HTTP::ContentType::JSON);

  trans->setup(req);
  trans->setHandler([](HTTP::Request* req, HTTP::Response* res) {
    Serial.println("Response received.");

    Serial.println(res->statusCode);
    Serial.println(res->getHeader()->c_str());
    delay(5);
    Serial.println(res->getBody()->c_str());

    return 0;
  });

  trans->start();
  Serial.println("Sending data.");

  return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("simple-post.cpp");

  HTTP::setup();
}

void loop() {
  HTTP::process();

  if(millis() > post_time) {
    post_data();
    post_time = millis() + post_period;
  }
}
