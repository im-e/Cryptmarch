#pragma once

template <typename T>
class Vector
{
	
private:
	T* data = nullptr;

	size_t size = 0;
	size_t capacity = 0;

public:
	Vector()
	{
		ReAlloc();
	}

	~Vector()
	{
		delete[] data;
	}

	void ReAlloc(size_t newCapacity)
	{
		//create new block of data
		T* newBlock = new T[newCapacity];


		if (newCapacity < size) { //if the new size of the memory is smaller than current memory
			size = newCapacity; //shink the size of the data 
		}


		// copy and move elements into new block
		for (size_t i = 0; i < size; i++)
		{
			newBlock[i] = data[i];
		}

		// delete old data 
		delete[] data;

		//reinitialise new block of data
		data = newBlock;
		
		//assign the new capacity
		capacity = newCapacity;

	}

	void pushBack(const T& value)
	{
		//if size is greater than capacity 
		if (size >= capacity)
		{
			//reallocate the memory with a size modifier of 1.5x
			ReAlloc(capacity + capacity / 2);
		}

		//put data in the first free slot
		data[size] = value;
		//increase in size
		size++;
	}

	void clear()
	{
		for (size_t i = 0; i < size; i++)
		{
			data[i].~T();
		}
		size = 0;
	}

	//emplace_back

	const T& operator[](size_t index) const //const version
		//operator[] lets you use it like an array

	{
		return data[index]; //returning the data at an index
	}

	const T& operator[](size_t index)
	{
		return data[index];
	}

	size_t Size() const
	{
		return size;
	}

};