#pragma once
#include <mutex>

template <class T>
class VtDoubleBufferedQueue
{
public:

	//
	// Constructor - construct a queue with the given buffer size
	//
	VtDoubleBufferedQueue(int bufferSize = 1000) :
		bufferLen(0), bufferSize(bufferSize)
	{
		buffer0 = buffer = new T[bufferSize];
		buffer1 = new T[bufferSize];
	}

	//
	// Destructor
	//
	~VtDoubleBufferedQueue()
	{
		delete[] buffer0;
		delete[] buffer1;
	}

	//
	// Add an item to the queue. Returns true if successful, false if the buffer is full.
	//
	bool put(const T& datum)
	{
		std::lock_guard<std::mutex> lock(_mtx);
		bool canWrite = bufferLen < bufferSize;
		if (buffer[bufferLen] == datum)
			return false;
		if (canWrite) buffer[bufferLen++] = datum;
		return canWrite;
	}

	//
	// Get all the items in the queue. The T** argument should be the address of a variable to
	// receive the pointer to the item array. The return value is the size of the array.
	//
	int get(T** data)
	{
		std::lock_guard<std::mutex> lock(_mtx);
		// Pass the pointer of the current buffer
		*data = buffer;
		// Save the length of the current buffer
		int ret = bufferLen;
		// Change the buffer pointer to the available buffer pointer
		buffer = (buffer == buffer0) ? buffer1 : buffer0;
		// Initialize the length of the buffer.
		bufferLen = 0;
		// Return the length of the buffer.
		return ret;
	}

private:

	// Disable copying and assignment
	VtDoubleBufferedQueue & operator=(const VtDoubleBufferedQueue&);
	VtDoubleBufferedQueue(const VtDoubleBufferedQueue&);

	T* buffer0;
	T* buffer1;
	T* buffer;
	int bufferLen;
	int bufferSize;
	std::mutex _mtx;
};


