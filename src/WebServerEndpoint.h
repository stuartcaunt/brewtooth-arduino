#ifndef WEBSERVERENDPOINT_H
#define WEBSERVERENDPOINT_H

class WebServerEndpoint {
public:
    WebServerEndpoint() {}
    virtual ~WebServerEndpoint() {}

    virtual void pathHandler() = 0;

private:

};

#endif /*WEBSERVERENDPOINT_H*/
