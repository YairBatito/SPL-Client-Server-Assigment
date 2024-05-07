#include "graph.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string> 
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;


int main(int argc, char **argv) {
    // Graph graph(argv[1]);
    Graph graph("db_toy.csv");
    // adding cache
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
       if (fd == -1) {
        perror("socket");
        return 1;
    }
    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    // addr.sin_port = htons(stoi(argv[2]));
    addr.sin_port = htons(stoi("4444"));
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    if (listen(fd, 5) == -1) {
        perror("listen");
        close(fd);
        return 1;
    }
       
    for (;;) {
        std::cout << "waiting for clients: " << endl;

        int fd2 = accept(fd, nullptr, nullptr);
        if (fd2 == -1) {
            std::cout << "error in accept " << endl;
            perror("accept");
            continue;
        }
        std::cout << "got client, try to read: " << endl;

        if (fork() == 0) {
            cout << "Child PID: " << getpid() << std::endl;
            close(fd); // Close the listening socket in the child process
            
            vector <int> input_numbers;
            char buffer[1024];
            ssize_t bytes_read = read(fd2, buffer, sizeof(buffer));
            buffer[bytes_read] = '\0';
            std::cout << "All message: " << buffer << std::endl;
            std::stringstream ss(buffer);
            // Define a vector to store the substrings
            std::string substring;
            for (int i=0; i<2;i++) {
                std::getline(ss, substring, ' ');
                std::cout << "substring: " << substring << std::endl;
                input_numbers.push_back(stoi(substring));
            }
            
            for (int num : input_numbers){
                std::cerr << num << endl; 
            }
            //TODO: adding code check in cache if pair in cache return else run bfs

            // Find BFS path
            vector<int> bfs_path = graph.BFS(input_numbers[0], input_numbers[1]);
            std::cerr << "got BFS : " << endl;
            string step_as_str;
            for (size_t i = 0; i < bfs_path.size(); ++i) {
                int step = bfs_path[i];
                std::cerr << step << endl; 
                if (i < bfs_path.size()-1){
                step_as_str = std::to_string(step)+ ' ';
                }
                else {
                    step_as_str = std::to_string(step)+ '\n';
                }
                ssize_t bytes_written = write(fd2, step_as_str.c_str(), step_as_str.size());
            }
            if (bfs_path.size()==0){
                step_as_str = '\n';
                ssize_t bytes_written = write(fd2, step_as_str.c_str(), step_as_str.size());
            }
            close(fd2); // Close the client socket in the child process
            //add to cache
            return 0;
        }
        std::cerr << "finish with client" << endl; 
        close(fd2); // Close the client socket in the parent process
    }

    std::cerr << "server is done" << endl; 
    close(fd); // Close the listening socket
    return 0;
}
