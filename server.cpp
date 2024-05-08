#include "ring.h"
#include "graph.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <mutex>
#include <thread>
using namespace std;
#define RING_SIZE 10


bool startsWith(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}


int contains(ring<std::string, RING_SIZE>& history, int v, int u) {
    for (int i = 0; i < 10; i++) {
        if (startsWith(history[i], std::to_string(v) + " ") && endsWith(history[i], " " +std::to_string(u) + "\n")) {
            return i; // Added semicolon
        }
    }
    return -1;
}

void handleRequest(int fd2, Graph& graph, ring<string, RING_SIZE>* myRing, std::mutex ringMutex) {
                // ringMutex.lock();
            cout << "Child PID: " << getpid() << std::endl;
            // close(fd2); // Close the listening socket in the child process
            
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
            
            // std::this_thread::sleep_for(std::chrono::milliseconds(1000*30));
            // Find BFS path
            std::cout << "search in cache" << "\n";
            std::cout << "myRing=" << *myRing << endl;
            ringMutex.lock();
            int containsIndex = contains(*myRing, input_numbers[0], input_numbers[1]);
            if(containsIndex >= 0){
                std::cerr << "reading from the cache index " << containsIndex << "\n";
                string cachedPath = (*myRing)[containsIndex];
                ssize_t bytes_written = write(fd2, cachedPath.c_str(), cachedPath.size());
            }
            else
            {
                std::cerr << "calc bfs" << endl;
                vector<int> bfs_path = graph.BFS(input_numbers[0], input_numbers[1]);
                std::cerr << "got BFS : " << endl;
                string step_as_str;
                string pathToCache;
                for (size_t i = 0; i < bfs_path.size(); ++i) {
                    int step = bfs_path[i];
                    std::cerr << step << endl; 
                    if (i < bfs_path.size()-1){
                        step_as_str = std::to_string(step)+ ' ';
                        pathToCache += step_as_str;
                        pathToCache += ' ';
                    }
                    else {
                        step_as_str = std::to_string(step)+ '\n';
                        pathToCache += step_as_str;
                    }
                    ssize_t bytes_written = write(fd2, step_as_str.c_str(), step_as_str.size());
                }
                if (bfs_path.size()==0){
                    step_as_str = '\n';
                    ssize_t bytes_written = write(fd2, step_as_str.c_str(), step_as_str.size());
                }else{
                    std::cerr << "ring address: " << &myRing << endl; 
                    std::cerr << "pushing to ring " << pathToCache<< endl; 
                    myRing->push_back(pathToCache);
                    std::cerr << "element 0 " << myRing[0]<< endl; 
                }
                
            }
            ringMutex.unlock();
            close(fd2); // Close the client socket in the child process

}

int main(int argc, char **argv) {
    // Graph graph(argv[1]);
    Graph graph("db_toy.csv");
    // ring<string, RING_SIZE> myRing;
    ring<string, RING_SIZE>* myRing = new ring<string, RING_SIZE>();
    std::mutex ringMutex;

    // ring <struct path ,10 > last_ten;
    
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
        std::thread handleRequest(fd2, graph, myRing, ringMutex);
        std::cerr << "finish with client" << endl; 
        close(fd2); // Close the client socket in the parent process
    }

    std::cerr << "server is done" << endl; 
    close(fd); // Close the listening socket
    return 0;
}
