#pragma once
#include "main.h"
#include <iostream>
#include <cstring>
#include <array>
#include <limits>
#include <fstream>
#include <limits>
using namespace std;

template <typename T, size_t size>
class Queue{
  T data[size];  // array to store queue
  const size_t t_size = sizeof(T);  // size of element
  const char* name;
  size_t front = -1, rear = -1;  // first and last index of queue data respectively
  
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
    for(size_t i = 0; i < size; i++){
      index = (front + i) % size; // stores the current index, rolling over if necessary
      cout << data[(front + i) % size] << " " << index << " | ";
      if(index == rear) break;
    }
    cout << endl;
  }


  void push(T val){ // enqueue single element
    if(isFull()){
      printf("Queue \"%s\" is full, push of value \"%d\" failed\n", name, val);
      return;
    }
    // cout << "pushed" << val << endl;
    if (isEmpty()) front = rear = 0; // inserts first element if queue is empty
    else rear = (rear + 1) % size;  // otherwise move it forwards, wrapping around if necessary
    
    data[rear] = val; // push value at that position
  }

  void push(T arr[], size_t arr_len){ // enqueue multiple elements
    const size_t elements_available = size - getDataSize();
    if(arr_len > elements_available){
      // pros::lcd::print();
      pros::lcd::print(0, "%s | More elements pushed than elements available: pushing %d elements of %d elements requested.\n", name, elements_available, arr_len);
      arr_len = elements_available;
    }

    const size_t elements_till_end = size - rear - 1;
    if(arr_len > elements_till_end){ // if the push needs to rollover
      const size_t leftover = arr_len - elements_till_end;
      memcpy(data + rear + 1, arr, elements_till_end * t_size);  // copies from rear to end of queue
      memcpy(data, arr + elements_till_end, leftover * t_size);  // copies leftover to start of queue
      rear += arr_len - size;
    }
    else{
      if (isEmpty()){
        front = rear = 0; // inserts first element if queue is empty
        memcpy(data + rear, arr, arr_len * t_size);  // copies arr to rear
      }
      else  memcpy(data + rear + 1, arr, arr_len * t_size);  // copies arr to rear
      rear += arr_len;
    }
  }

  T pop(){ // dequeue
    if(isEmpty()){
      printf("Queue \"%s\" is empty, pop failed\n", name);
      return std::numeric_limits<T>::max(); //returns T() if not a number
    }

    T result = data[front];
    if (front == rear) front = rear = -1; // if queue has 1 element left, make it empty after popping
    else front = (front + 1) % size;  // otherwise move it forwards, wrapping around if necessary
    
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
  file.open(file_name, ios::app);
  if(queue.rear < queue.front){ // if the queue rolls over
    file.write(queue.data + queue.front, size_cpy - queue.front);  // prints from front to end of queue
    file.write(queue.data, queue.rear + 1);  // prints from start to rear of queue    
  }
  else  file.write(queue.data + queue.front, queue.rear - queue.front + 1);  // prints from front to rear of queue
  file.close();
  queue.clear();
}