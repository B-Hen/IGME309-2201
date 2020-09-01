#ifndef _MAIN_H
#define _MAIN_H


#include <iostream>
#include <string>
#include<stdio.h>


#endif //_MAIN_H

using namespace std;

template<class T>
class Queue
{
public:
	//make the default construtor for this class
	Queue()
	{
		//make the inital size of the queue and set it
		tempArraySize = 1;
		tempArray = new T[tempArraySize]();
	};

	//consturctor to take a pointer that points to a queue
	Queue(const Queue& otherQueue)
	{
		//make it the same size and xopy it over
		tempArraySize = otherQueue.tempArraySize;
		tempArray = new T[tempArraySize];
		copy(otherQueue.tempArray, otherQueue.tempArray + otherQueue.tempArraySize, tempArray);
	};

	//copy constructior
	Queue& operator=(const Queue& otherQueue)
	{
		//check to see if its the same queue
		if (this == &otherQueue)
		{
			return *this;
		}

		//make ithe temp Array null to copy if it isn't
		if (tempArray != nullptr)
		{
			delete[] tempArray;
			tempArray = nullptr;
			tempArraySize = 1;
		}

		//now copy the other queue into this on
		tempArraySize = otherQueue.tempArraySize;
		tempArray = new T[tempArraySize]();
		copy(otherQueue.tempArray, otherQueue.tempArray + otherQueue.tempArraySize, tempArray);

		return *this;
	};

	//Deconstructor
	~Queue()
	{
		//take care of memeory leaks
		delete[] tempArray;
		tempArray = nullptr;
		tempArraySize = 0;
	}

	//method to add things to the queue
	void push(T item)
	{
		//add to the queue is it is emty
		if (tempArray[0] == NULL)
		{
			tempArray[0] = item;
		}
		//add to the queue if not emty but not full
		else if (tempArray[tempArraySize - 1] == NULL)
		{
			//loop through until a null place is found and add the item
			for (int i = 0; i < tempArraySize; i++)
			{
				if (tempArray[i] == NULL)
				{
					tempArray[i] = item;
					break;
				}
			}
		}
		//add to the queue if full
		else if (tempArray[tempArraySize-1] != NULL)
		{
			//place holder
			int placeHolder = tempArraySize;

			//make a new array twice as big as the old
			T* newArray = new T[tempArraySize * 2]();

			//copy the array into eacg other
			copy(tempArray, tempArray + tempArraySize, newArray);

			//modifiee the old one so it fits the new items
			delete[] tempArray;
			tempArray = nullptr;
			tempArraySize *= 2;

			tempArray = new T[tempArraySize]();

			//copy again
			copy(newArray, newArray + tempArraySize, tempArray);
			delete[] newArray;
			newArray = nullptr;

			//add the item in now
			tempArray[placeHolder] = item;
		}

		if (getSize() > 1)
		{
			Sort();
		}
	}

	//method to sort
	void Sort()
	{
		//save the newly added item
		T item = tempArray[getSize() - 1];

		int noChange = getSize() - 1;

		tempArray[getSize() - 1] = NULL;

		//save an index
		int index = -1;

		int size = getSize() - 1;

		//loop through and check if the new item is less than naything
		for (int i = 0; i < tempArraySize; i++)
		{
			if (item < tempArray[i])
			{
				index = i;
				break;
			}
		}

		//if the index chnages 
		if (index != -1)
		{
			//loop through again but move everything up in the queue
			for (int j = size; j > index - 1; j--)
			{
				//move every thing up
				tempArray[j + 1] = tempArray[j];
			}

			tempArray[index] = item;
		}
		else
		{
			tempArray[noChange] = item;
		}
	}

	//mehtod to pop the queue
	void pop()
	{
		for (int i = 0; i < getSize() - 1; i++)
		{
			T item = tempArray[i + 1];
			tempArray[i] = item;
		}

		tempArray[getSize() - 1] = NULL;
	}

	//method to print out the contents of the queue
	void print()
	{
		//first check if the queue is empty
		if (getSize() < 1)
		{
			cout << "The Queue is currently empty." << endl;
		}

		//loop through and print out the contents of the queue
		for (int i = 0; i < tempArraySize; i++)
		{
			if (tempArray[i] == NULL)
			{
				break;
			}

			cout << tempArray[i] << endl;
		}
	}

	//method to get the size of the array
	int getSize()
	{
		//first check if the queue is empty
		if (isEmpty() == false)
		{
			int entries = 0;

			//loop through the queue until it is null
			for (int i = 0; i < tempArraySize; i++)
			{
				//check if the queue is ever null
				if (tempArray[i] == NULL)
				{
					return entries;
				}

				entries++;
			}
			return entries;
		}
		return 0;
	}

	//method to see if the queue is empty
	bool isEmpty()
	{
		//check is the first place in the queue null
		if (tempArray[0] == NULL)
		{
			return true;
		}

		return false;
	}

private:
	//make the variables for this class
	T* tempArray;
	int tempArraySize;
};