#pragma once

#include "std_lib_facilities.h"

class ring_overflow {};
class ring_underflow{};
class index_out_of_bounds_exception {};

template<class T, int N>
class ring_it {
    T *elements;
    int i;
public:
ring_it(T *elements, int i) : elements(elements), i(i) {}
T operator*() { return elements[i]; }
bool operator!=(const ring_it &o) { return i != o.i; }
void operator++() {
    if(i== N-1 ) i=0;
    else ++i; }

};

template<class T, int N>
class ring {
    int head ,tail , size ;
    T *elements;
 public:
    ring() :elements(new T[N]), head(0), tail(-1),size(0){
    }

    T& operator [](int index){
        if(index < 0 || index >= N)
            throw index_out_of_bounds_exception();
        return elements[index];
    }
    ring_it<T,N> begin(){ return ring_it<T,N> (elements,head) ;  }
    ring_it<T,N> end(){if(tail == N - 1) tail = -1; return ring_it<T,N> (elements,tail + 1) ; }
   ~ring(){delete [] elements ;} 

    void push_back( T data ){
        if(size != N ){
            if(tail == N - 1) tail = -1;
            elements[++tail] = data;
            size++;
            return;
        }
        throw ring_overflow();
    }

    T& pop_front(){
        if(size != 0){
            size--;
            if(head == N-1){
                head = 0;
                return elements[N-1];
            }
            return elements[head++];
        }
        throw ring_underflow();
    }


};
template<class T, int N >
ostream& operator<<(ostream& os , ring <T,N>& x ){
        for(auto it = x.begin() ; it != x.end() ; ++it)
            os<< *it << " ";
        return os;
};
