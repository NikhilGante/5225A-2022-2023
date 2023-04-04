#pragma once
#include "main.h"
#include <iostream>
#include <cstring>
#include <limits>
#include <fstream>
#include "timer.hpp"
using namespace std;

extern pros::Mutex log_mutex;

template <typename T, size_t capacity>
class Queue{
  T data[capacity];  // Array to store queue
  const size_t t_size = sizeof(T);  // size of element
  const char* name;
  size_t front = 0; // First index of queue data
  size_t rear = 0;  // First index AFTER queue
  bool full = false;
    
  pros::Mutex queue_mutex;
    
  // declares as friend to give access to front and rear pointers
  template<size_t size_cpy>
  friend void queuePrintFile (Queue<char, size_cpy>& queue, ofstream& file, const char* file_name);

public:
    explicit Queue(const char* name):  name(name)
    {
      printf("size: %lld, f:%lld r:%lld\n", getSize(), front, rear);
    }
    
    bool isFull(){
      return full;
    }
    
    bool isEmpty(){
      return getSize() == 0;
    }
    
    void print(){ // prints all the elements in the queue
      if(isEmpty()){
        printf("Queue \"%s\" is empty, nothing to print\n", name);
        return;
      }
      int index;
      for(size_t i = 0; i < getSize(); i++){
        index = (front + i) % capacity; // stores the current index, rolling over if necessary
        cout << data[index] << " " << index << " | ";
      }
      cout << endl;
    }
    
    
    void enqueue(T val){ // enqueue single element
      queue_mutex.take();
      if(isFull()){
        printf("Queue \"%s\" is full, enqueue of value \"%d\" failed\n", name, val);
//        queue_mutex.give();
        return;
      }
      // cout << "enqueueed" << val << endl;
      
      if(capacity - getSize() == 1) full = true;
      data[rear] = val; // enqueue value at rear index before moving it
      
      rear = (rear + 1) % capacity;  // Move rear forwards, wrapping around if necessary
      
      queue_mutex.give();
    }
    
    void enqueue(T arr[], size_t arr_len){ // enqueue multiple elements
      queue_mutex.take();
      size_t elements_available = capacity - getSize();
      if(arr_len >= elements_available){
        full = true;
        if(arr_len > elements_available)  printf("%s | More elements enqueued than elements available: enqueueing %lld elements of %lld elements requested.\n", name, elements_available, arr_len);
        
        arr_len = elements_available;
      }
      size_t elements_till_end = capacity - rear;
      if(arr_len > elements_till_end){ // If the enqueued data needs to rollover
        memcpy(data + rear, arr, elements_till_end * t_size);  // Copies from rear to end of queue
        arr_len -= elements_till_end; // Arr len is now size of leftover data
        memcpy(data, arr + elements_till_end, arr_len * t_size);  // Copies leftover to start of queue
      }
      else  memcpy(data + rear, arr, arr_len * t_size);  // Copies arr to rear
      
      rear = (rear + arr_len) % capacity;  // Move rear forwards by amount of elements added, wrapping around if necessary
      queue_mutex.give();
    }
    
    T dequeue(){ // dequeue
//      queue_mutex.take();



      if(isEmpty()) {
        printf("Queue \"%s\" is empty, dequeue failed\n", name);
//        queue_mutex.give();
        return numeric_limits<T>::min();
//        return INT_MIN;
      }
      if(isFull())  full = false; // Queue will no longer be full after dequeuing element
      T result = data[front];
      front = (front + 1) % capacity;  // Move front forwards, wrapping around if necessary
//      queue_mutex.give();
      
      return result;
    }
    
    size_t getSize(){  // Returns amount of elements currently filled
      if (isFull()) return capacity;
      if (front == rear)  return 0;
      if(rear < front)  return capacity - front + rear;
      return rear - front;
    }
    
    void clear(){ // Empties the queue
      front = 0, rear = 0;
      full = false;
    }
};

// Prints the contents of a char queue to a file (used for logging)
template<size_t capacity>
void queuePrintFile(Queue<char, capacity>& queue, ofstream& file, const char* file_name){
  size_t rear = queue.rear, front = queue.front;  // Saves to temp variables so enqueues can still occur in other tasks
  size_t num_ele_to_print = queue.getSize(); // How much to move front pointer by
	Timer flush_timer{"yo", true, timing_units::micros};


  file.open(file_name, ios::app);
  printf("OPEN TOOK:%lld\n", flush_timer.getTime());
  flush_timer.reset();
  if(rear < front || queue.isFull()){ // If the queue rolls over
    file.write(queue.data + front, capacity - front);  // Prints from front to end of queue
    file.write(queue.data, rear);  // Prints from start to rear of queue
    printf("WRITE TOOK:%lld\n", flush_timer.getTime());

    // cout.write(queue.data + front, capacity - front);  // prints from front to end of queue
    // cout.write(queue.data, rear);  // prints from start to rear of queue
    printf("\n\nOVERFLOW\n\n\n");
  }
  else{
    file.write(queue.data + front, rear - front);  // Prints from front to rear of queue
    printf("WRITE TOOK:%lld\n", flush_timer.getTime());
    // cout.write(queue.data + front, rear - front);
    printf("\n\nPRINTING\n\n\n");
    
    
  }
  queue.front = (queue.front + num_ele_to_print) % capacity;
  printf("AFTER: f:%d r:%d\n", queue.front, queue.rear);
  flush_timer.reset();

  
  file.close();
  printf("CLOSE TOOK:%lld\n", flush_timer.getTime());
}