#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "httpProxy.h"


int main() {
    const int PORT = 8080;
    HttpProxy proxy(PORT);
    proxy.start();

    return 0;
}
