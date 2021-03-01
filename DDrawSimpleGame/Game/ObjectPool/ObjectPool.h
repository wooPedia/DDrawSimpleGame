#pragma once

#include <vector>
#include <memory>
#include <cassert>

template <class T>
class ObjectPool final
{
public:
	using object_uptr_type = std::unique_ptr<T>;
	using pool_type = std::vector<object_uptr_type>;

	ObjectPool(size_t maxPoolSize);
	ObjectPool(size_t maxPoolSize, const T* sample);
	~ObjectPool();

	template <class... Type>
	void Initialize(Type... _Arg);

	object_uptr_type Get();
	void ReturnObject(object_uptr_type pObject);

	void Release();

	inline size_t GetMaxPoolSize() const;
	inline size_t GetAvailableSize() const;

	ObjectPool() = delete;

protected:
	const size_t m_maxPoolSize;
	pool_type    m_objectPool;

private:
	ObjectPool(const ObjectPool&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;
};

template<class T>
ObjectPool<T>::ObjectPool(size_t maxPoolSize)
	: m_maxPoolSize(maxPoolSize)
{
}

// sample과 동일한 데이터(오브젝트)로 오브젝트 풀을 가득 채웁니다.
template<class T>
ObjectPool<T>::ObjectPool(size_t maxPoolSize, const T* sample)
	: m_maxPoolSize(maxPoolSize)
{
	m_objectPool.reserve(m_maxPoolSize);
	for (size_t i = 0; i < m_maxPoolSize; ++i)
	{
		m_objectPool.emplace_back(std::make_unique<T>(*sample));
	}
}

template<class T>
ObjectPool<T>::~ObjectPool()
{
	Release();
}

//////////////////////////////////////////////////////////////////////
// Name: Initialize()
// Desc: 오브젝트 풀을 초기화합니다.(풀에 오브젝트를 채워 넣습니다.) 
//////////////////////////////////////////////////////////////////////
template <class T>
template <class... Type>
void ObjectPool<T>::Initialize(Type... _Arg)
{
	if (m_objectPool.empty())
	{
		// pool에 maxPoolSize만큼 개체를 생성합니다.
		m_objectPool.reserve(m_maxPoolSize);
		for (size_t i = 0; i < m_maxPoolSize; ++i)
		{
			m_objectPool.emplace_back(std::make_unique<T>(_Arg...));
		}
	}
	else
	{
#ifdef _DEBUG
		// 풀이 비어있지 않은데 해당 함수를 호출한 것은 실수일 가능성이 높음
		__debugbreak(); 
#endif // _DEBUG
	}
}

//////////////////////////////////////////////////////////////////////
// Name: Get()
// Desc: 오브젝트 풀에서 오브젝트(unique_ptr<object>) 1개를 반환합니다.
//////////////////////////////////////////////////////////////////////
template <class T>
typename ObjectPool<T>::object_uptr_type ObjectPool<T>::Get()
{
	if (m_objectPool.size() > 0)
	{
		object_uptr_type pObject = std::move(m_objectPool.back());
		m_objectPool.pop_back();

		return std::move(pObject);
	}
	else
	{
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////
// Name: ReturnObject()
// Desc: 사용한 오브젝트를 풀에 반환합니다.
//////////////////////////////////////////////////////////////////////
template <class T>
void ObjectPool<T>::ReturnObject(object_uptr_type pObject)
{
	// 객체를 풀에 반환합니다. 가득 찼다면 메모리를 해제합니다.
	if (m_objectPool.size() < m_maxPoolSize)
	{
		m_objectPool.emplace_back(std::move(pObject));
	}
	else
	{
		pObject = nullptr;
	}
}

template <class T>
void ObjectPool<T>::Release()
{
	m_objectPool.clear();
}

template <class T>
inline size_t ObjectPool<T>::GetMaxPoolSize() const
{
	return m_maxPoolSize;
}

template <class T>
inline size_t ObjectPool<T>::GetAvailableSize() const
{
	assert(m_objectPool.size() <= m_maxPoolSize);
	return m_objectPool.size();
}