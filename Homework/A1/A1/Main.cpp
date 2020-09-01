////////////////////////////////////////////////////
//Breanna Henriquez 
//Date:08-20-2020
//This Assignemnet is to make my own priority Queue
///////////////////////////////////////////////////

#include "Queue.h"
#include <iostream>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>


using namespace std;

void Wrapper();


int main()
{
    Wrapper();
}

void Wrapper()
{
    cout << "All Queues are sorted from least to greatest" << endl << endl;

   //create the first queue of ints
   Queue<int>* intQueue(new Queue<int>());

   //push to the queue and print
   cout << "(First Queue of ints) Adding ints to the queue: " << endl;
   intQueue->push(7);
   intQueue->push(8);
   intQueue->push(6);
   intQueue->push(5);
   intQueue->push(10);
   intQueue->print();
   cout << endl;

   //pop from the queue and print
   cout << "Removing ints from the queue: " << endl;
   intQueue->pop();
   intQueue->print();
   cout << endl;

   //create the second queue of doubles
   Queue<double>* doubleQueue(new Queue<double>());

   //push doubles to the Queue and print
   cout << "(Second Queue of doubles) Adding doubles to the queue: " << endl;
   doubleQueue->push(7.3);
   doubleQueue->push(9.45);
   doubleQueue->push(1.5);
   doubleQueue->push(3.5);
   doubleQueue->push(45.7);
   doubleQueue->print();
   cout << endl;

   //peop the the double queue than print
   cout << "Removing doubles from the queue: " << endl;
   doubleQueue->pop();
   doubleQueue->print();
   cout << endl;

   //create a third queue of floats
   Queue<float>* floatQueue(new Queue<float>());

   //push floats to the queue
   cout << "(Third Queue of floats) Adding floats to the queue: " << endl;
   floatQueue->push(42.56f);
   floatQueue->push(1.234f);
   floatQueue->push(27.6f);
   floatQueue->push(3.0f);
   floatQueue->push(0.89f);
   floatQueue->print();
   cout << endl;

   //pop the queue and print
   cout << "Removing floats from the queue: " << endl;
   floatQueue->pop();
   floatQueue->print();
   cout << endl;

   //create a copy queue and use the copy constructor and operator
   cout << "Copy the int Queue and print the items: " << endl;
   Queue<int>* copyIntQueue(new Queue<int>(*intQueue));
   copyIntQueue->print();
   cout << endl;

   //change the queue from the orginal
   cout << "Push new items into the copy of the int queue then print:" << endl;
   copyIntQueue->push(420);
   copyIntQueue->push(36);
   copyIntQueue->print();
   cout << endl;

   delete copyIntQueue;
   copyIntQueue = nullptr;

   cout << "Set copy of the queue equal to the original one and print again: " << endl;
   copyIntQueue = intQueue;
   copyIntQueue->print();

   //take care of the memory leaks
   delete intQueue;
   intQueue = nullptr;

   delete doubleQueue;
   doubleQueue = nullptr;

   delete floatQueue;
   floatQueue = nullptr;
}

