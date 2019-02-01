#pragma once
#include <mutex>

template<typename T>
class TemplateSingleton
{
protected:
	TemplateSingleton()
	{

	}
	virtual ~TemplateSingleton()
	{

	}

public:
	static T * GetInstance()
	{
		std::lock_guard<std::mutex> lock(m_);
		if (m_pInstance == nullptr)
			m_pInstance = new T;
		return m_pInstance;
	};

	static void DestroyInstance()
	{
		std::lock_guard<std::mutex> lock(m_);
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	};

private:
	static T * m_pInstance;
	static std::mutex m_;
};

template <typename T> T * TemplateSingleton<T>::m_pInstance = 0;
template <typename T>  std::mutex TemplateSingleton<T>::m_;
