#if _MSC_VER > 1000
	#pragma once
#endif 

#include <thread> 
#include <queue> 
#include <mutex> 
#include <condition_variable> 
#include <memory>   

template <class T>
class BlockingQueue
{
private:
    std::queue<T> mData;     
	
	mutable std::mutex mMut;      
	std::condition_variable mEmptyCondition;  
	bool mReady = true;
	void wait_for_debugging() 
	{         
		std::this_thread::sleep_for(std::chrono::milliseconds(200+rand()%200));     
	}

public:
	void SetReady(bool flag)
	{
		std::lock_guard<std::mutex> lock(mMut);
		mReady = flag;
		mEmptyCondition.notify_one();
	}
	void Push(T pValue)     
	{         
		// block execution here, if other thread already locked mMute!         
		std::lock_guard<std::mutex> lock(mMut);          
		// if weare here no other thread is owned/locked mMute. so we can modify the internal data         
		mData.push(pValue);          
		// OK! notify one of threads which was blocked because , queue empty and make them happy!         
		mEmptyCondition.notify_one();          
		//wait_for_debugging();     
	} // //lock.unlock(); !!!! does not have such method!     
	// now mMute is unlocked in destcutor of lock_guard!       
	T Pop()     
	{         
		// own the mMute like in Push method excep that lock can be unlocked manually without need to destructor of lock!         
		std::unique_lock<std::mutex> lock(mMut);          
		// if we are here. mMute is locked and no other thread can access/modify the data!         
		// wait() method first checks if mData is not empty, allowes execution to go on. else :         
		// unlocks the mMut and waits for signla.         
		// because mMute is released other threads have a chance to Push new data into queue          
		// ... in notify this condition variable!   
		// ť�� ����ְ� ���� �غ� �Ǿ� ���� ������ ������ ��ٸ���.
		// ť�� ������� �ʰ� ���� �غ� �Ǿ� ���� ������ ������ ��ٸ���.
		// ť�� ����ְ� ���� �غ� �Ǿ� ������ ������ ��ٸ���.
		// ť�� ������� �ʰ� ���� �غ� �Ǿ� ������ ��ٸ��� �ʰ� ���� �ܰ踦 �����Ѵ�.
		mEmptyCondition.wait(lock,[this]{ return !mData.empty() && mReady;});         
		// if we are are here, mData is not empty and mMut is locked !         
		// be careful ! std::shared_ptr<T> ret(new T(mData.front()) performs 2 memory allocation!         
		auto item = mData.front();         
		mData.pop();         
		//wait_for_debugging();         
		return item;     
	}     

	T GetLatestNClear()
	{
		// own the mMute like in Push method excep that lock can be unlocked manually without need to destructor of lock!         
		std::unique_lock<std::mutex> lock(mMut);
		// if we are here. mMute is locked and no other thread can access/modify the data!         
		// wait() method first checks if mData is not empty, allowes execution to go on. else :         
		// unlocks the mMut and waits for signla.         
		// because mMute is released other threads have a chance to Push new data into queue          
		// ... in notify this condition variable!   
		// ť�� ����ְ� ���� �غ� �Ǿ� ���� ������ ������ ��ٸ���. (false, false)
		// ť�� ������� �ʰ� ���� �غ� �Ǿ� ���� ������ ������ ��ٸ���.(true, false)
		// ť�� ����ְ� ���� �غ� �Ǿ� ������ ������ ��ٸ���. (false, true)
		// ť�� ������� �ʰ� ���� �غ� �Ǿ� ������ ��ٸ��� �ʰ� ���� �ܰ踦 �����Ѵ�.(true, true)
		mEmptyCondition.wait(lock, [this] { return !mData.empty() && mReady; });
		// if we are are here, mData is not empty and mMut is locked !         
		// ���� �ֱ� �����͸� �����´�.
		auto item = mData.back();
		// ť�� ����.
		//Clear();
		// ���� �ֱ� �����͸� ��ȯ�Ѵ�.
		return item;
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lock(mMut);
		std::queue<T> empty;
		std::swap(mData, empty);
	}
	
	bool Empty() const    
	{         
		//  this function is "const", so why are we allowed to modify a member vaiable "mMute"?....         
		//we can, because we marked it "mutable"! see definition of mMute.         
		std::lock_guard<std::mutex> lock(mMut);          
		return mData.empty();     
	}

	int Size() const    
	{            
		std::lock_guard<std::mutex> lock(mMut);          
		return mData.size();     
	} 
};