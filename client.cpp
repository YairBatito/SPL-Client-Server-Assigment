#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<unistd.h>
#include <string> 
#include <iostream>
#include <sstream>

using namespace std;


int main(int arcg , char **argv){
    std::cout << "client starts" << std::endl;

    int fd = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in adder = {0};
    adder.sin_family = AF_INET;
    adder.sin_addr.s_addr = inet_addr(argv[1]);
    adder.sin_port = htons(stoi(argv[2]));
    connect(fd, (sockaddr*)&adder,sizeof(adder));
    std::cout << "client connected" << std::endl;

    std::stringstream ss;
    for (int i = 3; i < arcg; ++i) {
        ss << argv[i] << " ";
    }
    // Send the concatenated string as a message
    std::string message = ss.str();
    std::cout << "client try to wrote to server" << std::endl;

    ssize_t bytes_written = write(fd, message.c_str(), message.size());
    std::cout << message << std::endl;

    const char* end_marker = "\n"; // Use newline as end-of-message marker
    bytes_written = write(fd, end_marker, 1);

    std::cout << "client wrote to server" << std::endl;

    // Receive response from the server
    char buffer[1024];
    ssize_t bytes_received;
    while ((bytes_received = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';
        std::cout << buffer;

        if (memchr(buffer, '\n', bytes_received) != nullptr) {
            std::cout << "got terminator, exit" << endl;
            break; // Stop reading further data
        }

    }
    close(fd);
}