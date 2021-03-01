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

// sample�� ������ ������(������Ʈ)�� ������Ʈ Ǯ�� ���� ä��ϴ�.
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
// Desc: ������Ʈ Ǯ�� �ʱ�ȭ�մϴ�.(Ǯ�� ������Ʈ�� ä�� �ֽ��ϴ�.) 
//////////////////////////////////////////////////////////////////////
template <class T>
template <class... Type>
void ObjectPool<T>::Initialize(Type... _Arg)
{
	if (m_objectPool.empty())
	{
		// pool�� maxPoolSize��ŭ ��ü�� �����մϴ�.
		m_objectPool.reserve(m_maxPoolSize);
		for (size_t i = 0; i < m_maxPoolSize; ++i)
		{
			m_objectPool.emplace_back(std::make_unique<T>(_Arg...));
		}
	}
	else
	{
#ifdef _DEBUG
		// Ǯ�� ������� ������ �ش� �Լ��� ȣ���� ���� �Ǽ��� ���ɼ��� ����
		__debugbreak(); 
#endif // _DEBUG
	}
}

//////////////////////////////////////////////////////////////////////
// Name: Get()
// Desc: ������Ʈ Ǯ���� ������Ʈ(unique_ptr<object>) 1���� ��ȯ�մϴ�.
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
// Desc: ����� ������Ʈ�� Ǯ�� ��ȯ�մϴ�.
//////////////////////////////////////////////////////////////////////
template <class T>
void ObjectPool<T>::ReturnObject(object_uptr_type pObject)
{
	// ��ü�� Ǯ�� ��ȯ�մϴ�. ���� á�ٸ� �޸𸮸� �����մϴ�.
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