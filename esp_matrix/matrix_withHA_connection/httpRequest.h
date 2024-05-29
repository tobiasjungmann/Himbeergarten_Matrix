#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_h
namespace httpRequest {
    String httpGETRequest(const char *serverName);

    String httpRequest(const char *serverName);

    String httpRequestHA(const char *serverName);

    IPAddress setup();
}

#endif