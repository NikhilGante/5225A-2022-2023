#pragma once
#include "main.h"
#include <iostream>
#include <cstring>
#include <array>
#include <limits.h>
#include <fstream>
// #include "logging.hpp"
using namespace std;

extern pros::Mutex log_mutex;

template <typename T, size_t size>
class Queue{
  T data[size];  // array to store queue
  const size_t t_size = sizeof(T);  // size of element
  const char* name;
  size_t front = -1, rear = -1;  // first and last index of queue data respectively
  
  pros::Mutex queue_mutex;

  // declares as friend to give access to front and rear pointers
  template<size_t size_cpy>
  friend void queuePrintFile (Queue<char, size_cpy>& queue, ofstream& file, const char* file_name);

public:
  Queue(const char* name):  name(name)
  {}

  bool isFull(){
    return (front == 0 && rear == size - 1) || front - rear == 1;
  }

  bool isEmpty(){
    return front == -1;
  }

  void print(){ // prints all the elements in the queue
    if(isEmpty()){
      printf("Queue \"%s\" is empty, nothing to print\n", name);
      return;
    }
    int index;
    for(size_t i = 0; i < getDataSize(); i++){
      index = (front + i) % size; // stores the current index, rolling over if necessary
      cout << data[(front + i) % size] << " " << index << " | ";
      if(index == rear) break;
    }
    cout << endl;
  }


  void push(T val){ // enqueue single element
    queue_mutex.take();
    if(isFull()){
      printf("Queue \"%s\" is full, push of value \"%d\" failed\n", name, val);
      queue_mutex.give();
      return;
    }
    // cout << "pushed" << val << endl;
    if (isEmpty()) front = rear = 0; // inserts first element if queue is empty
    else rear = (rear + 1) % size;  // otherwise move it forwards, wrapping around if necessary
    
    data[rear] = val; // push value at that position
    queue_mutex.give();
  }

  void push(T arr[], size_t arr_len){ // enqueue multiple elements
    log_mutex.take();
    size_t elements_available = size - getDataSize();
    if(arr_len > elements_available){
      // pros::lcd::print();
      pros::lcd::print(0, "%s | More elements pushed than elements available: pushing %d elements of %d elements requested.\n", name, elements_available, arr_len);
      arr_len = elements_available;
    }
    size_t elements_till_end = size - rear - 1;
    if(arr_len > elements_till_end){ // if the push needs to rollover
      memcpy(data + rear + 1, arr, elements_till_end * t_size);  // copies from rear to end of queue
      size_t leftover = arr_len - elements_till_end;
      memcpy(data, arr + elements_till_end, leftover * t_size);  // copies leftover to start of queue
      rear = leftover - 1;
    }
    else{
      if (isEmpty()){
        front = 0; // Initializes front because queue is no longer empty
        memcpy(data, arr, arr_len * t_size);  // copies arr to rear

      }
      else  memcpy(data + rear + 1, arr, arr_len * t_size);  // copies arr to rear
      rear += arr_len;
    }
    log_mutex.give();
  }

  T pop(){ // dequeue
    queue_mutex.take();
    if(isEmpty()){
      printf("Queue \"%s\" is empty, pop failed\n", name);
      queue_mutex.give();
      return std::numeric_limits<T>::min();
    }

    T result = data[front];
    if (front == rear) front = rear = -1; // if queue has 1 element left, make it empty after popping
    else front = (front + 1) % size;  // otherwise move it forwards, wrapping around if necessary
    queue_mutex.give();
    
    return result;
  }

  size_t getDataSize(){  // returns amount of elements currently filled
    if(isEmpty()) return 0;
    // if the queue rolls over
    if(rear < front) return rear + 1 + size - front; // adds elements up from start until back then from front until end
    else return rear - front + 1;
  }

  void clear(){ // empties the queue
    front = -1, rear = -1;
  }
};

// prints the contents of a char queue to a file (used for logging)
template<size_t size_cpy>
void queuePrintFile(Queue<char, size_cpy>& queue, ofstream& file, const char* file_name){
  log_mutex.take();
  file.open(file_name, ios::app);
  if(queue.rear < queue.front){ // if the queue rolls over
    file.write(queue.data + queue.front, size_cpy - queue.front);  // prints from front to end of queue
    file.write(queue.data, queue.rear + 1);  // prints from start to rear of queue
    cout.write(queue.data + queue.front, size_cpy - queue.front);  // prints from front to end of queue
    cout.write(queue.data, queue.rear + 1);  // prints from start to rear of queue   
    cout << "\n ***********************OVERFLOW\n\n\n";
  }
  else{
    file.write(queue.data + queue.front, queue.rear - queue.front + 1);  // prints from front to rear of queue
    cout.write(queue.data + queue.front, queue.rear - queue.front + 1);
    cout << "\n ***********************PRINTING\n\n\n";

  }
  // cout << flush;
  
  file.close();
  queue.clear();
  log_mutex.give();
}