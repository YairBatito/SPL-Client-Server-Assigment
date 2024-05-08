#pragma once

#include "std_lib_facilities.h"

class index_out_of_bounds_exception {};
using namespace std;


template<class T, int N>
class ring {
    int currIndex;
    T *elements;
 public:
    ring() :elements(new T[N]), currIndex(0){
    }

    T& operator [](int index){
        std::cerr << "index = " << index << endl;
        if(index < 0 || index >= N)
            throw index_out_of_bounds_exception();
        return elements[index];
    }
   ~ring(){
            std::cout << "call the ~ring" << endl;
            delete [] elements ;
        } 

    void push_back( T data ){
        std::cout << "before update this->currIndex = " << this->currIndex << endl;
        std::cout << "insert " << data << " in pos " << this->currIndex << endl;
        elements[this->currIndex] = data;
        this->currIndex = (this->currIndex + 1) % N; // Update index properly
        std::cout << "after update this->currIndex = " << this->currIndex << endl;
    }



};
template<class T, int N >
ostream& operator<<(ostream& os , ring <T,N>& x ){
    // std::cout << "reading from the list" << endl;
    os << "[";
    for(int i = 0; i< N; i++){
            os<< x[i] << ", ";
    }
    os << "]";
    return os;
};
