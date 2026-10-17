#pragma once
#include "Logger.h"

//template uses placeholder type t
template <typename T>
class Vector
{
	
private:
	T* data = nullptr; //pointer to the memory block

	size_t size = 0; //items stored
	size_t capacity = 0; //total slots available

	void changeCapacity(size_t newCapacity)
	{
		//allocate new block of memory
		//allocate new block with the size of new capacity
		T* newData = new T[newCapacity];
		
		//copy and move the old elements into new block

		//if the new capacity is smaller than the size of the amount of items we have (shrinking)
		if (newCapacity < size)
		{
			size = newCapacity; //set size to our new capacity
		}

		//for each bit of data in our new size
		for (size_t x = 0; x < size; x++)
		{
			newData[x] = data[x]; //copy old data into new data
		}

		//delete old block of data
		delete[] data;

		//data now becomes the new block of data
		data = newData;

		//update our capacity to new capacity
		capacity = newCapacity;
	}

public:
	Vector() //constructor
	{
		size = 0; //set size to 0
		capacity = 8; //set capacity to small fixed number
		//set to 8 since its a power of 2 and covers general small containers while not being too large 
		data = new T[capacity]; //allocate a block of that capacity
	}

	~Vector() //destructor
	{
		delete[] data; //delete (array) data
	}

	void add(const T& value) //add item 
	{
		//if size equals or is larger than capacity
		if (size >= capacity)
		{
			//change capacity
			changeCapacity(capacity * 2); //by a growth factor of two
			//this means our data will have to grow less than 1.5x. 1.5x could save more memory but 
			//using 2x means that expensive grows get further and further apart
		}
		data[size] = value; //store value into slot
		size++; //increase in size
	}

	//returns reference to type
	//operator allows us to use it like an array [] 
	T& operator[](size_t index)
	{
		if (index >= size) //if index is bigger than size (>= to since index == size will be out of bounds)
		{
			Logger::Error("Index is outside of size"); //assert
			return;
		}
		return data[index]; //return data at index
	}

	void remove(const T& value) //remove value preserving order
	{
		//removing from the middle leaves a hole so we have to close it
		for (size_t x = 0; x < size; x++) //loop through the data
		{
			//until we find the value
			//T will need an operator== if im storing my own class or structure
			if (data[x] == value)
			{
				//shift everything left after x by one
				for (size_t y = x; y < size - 1; y++) //start at x, for each element but the last
				{
					data[y] = data[y + 1]; //data at y becomes the data from its next element
				}
				size--; //decrease size
				return; //return to only remove the first match
			}
		}
		//data cut off by size will still exist but will get overwritten by next add
	}

	void removeSwap(const T& value) //remove value not preserving the order
	{
		for (size_t x = 0; x < size; x++) //loop through the data
		{
			//until we find the value
			//T will need an operator== if im storing my own class or structure
			if (data[x] == value) 
			{
				data[x] = data[size - 1]; //overwrite with last element in the list
				size--; //data cut off by size will still exist but will get overwritten by next add
				return;
	
			}
		}

	}

	void removeSwap(const size_t index) //remove value not preserving the order with index
	{
		//we have the index so
		if (index >= size) //if index is bigger than size (>= to since index == size will be out of bounds)
		{
			Logger::Error("Index is outside of size"); //assert
			return;
		}

		data[index] = data[size - 1]; //overwrite with last element in the list

		//data cut off by size will still exist but will get overwritten by next add
		size--;
	}

	void clear() //clear data (block stays allocated)
	{
		size = 0;
	}

	const size_t getSize() //get size of vector
	{
		return size;
	}


};