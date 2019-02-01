#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class VtQueue
{
public:

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto val = queue_.front();
		queue_.pop();
		return val;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
	}

	T& Peek()
	{
		return queue_.front();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}
	VtQueue() = default;
	VtQueue(const VtQueue&) = delete;            // disable copying
	VtQueue& operator=(const VtQueue&) = delete; // disable assignment

	void Clear();
private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

template <typename T>
void VtQueue<T>::Clear()
{
	std::queue<T> empty;
	std::swap(queue_, empty);
}
