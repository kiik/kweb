/** @file HTTP.cpp
 *  Async HTTP communication system implementation.
 *
 *  @author Meelik Kiik
 *  @date 10. December 2015
 *  @version 0.1
 */

#include "HTTP.hpp"

namespace HTTP {

  Transfer* transferBuffer[HTTP_SOCK_MX];

  void setup() {
    for(int i=0; i < HTTP_SOCK_MX; i++) transferBuffer[i] = nullptr;
  }

  void process() {
    Transfer* trans;
    for(int i=0; i < HTTP_SOCK_MX; i++) {
      trans = transferBuffer[i];
      if(trans == nullptr) continue;

      if(trans->isInProgress()) {
        if(Particle.connected()) trans->process(); //TODO: Else close transfer with code 404/
      }
      else {
        delete trans;
        transferBuffer[i] = nullptr;
      }
    }
  }

  Transfer* getAvailableSocket() {
    if(!Particle.connected()) return nullptr;

    for(int i = 0; i < HTTP_SOCK_MX; i++) {
      if(transferBuffer[i] == nullptr) {
        transferBuffer[i] = new Transfer();
        return transferBuffer[i];
      } else if( !(transferBuffer [i]->isInProgress()) ) {
          delete transferBuffer[i];
          transferBuffer[i] = new Transfer();
          return transferBuffer[i];
        }
      }

    return nullptr;
  }

}
