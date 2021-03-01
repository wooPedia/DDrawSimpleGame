#pragma once

#include "../../Util/Header/ImageData.h"

#include <memory>
#include <list>

class MyDDraw;
class ImageData;

// Base class
// T�� �ݵ�� CGameObject Ŭ������ ��ӹ��� Ŭ�������� ��!
template <class T>
class GameObjectManager
{
public:
	using list_iter_type = typename std::list<std::unique_ptr<T>>::iterator;

	GameObjectManager(size_t maxObjectNum = 1);
	virtual ~GameObjectManager();

	inline list_iter_type GetBegin() { return m_listObjects.begin(); }
	inline list_iter_type GetEnd()   { return m_listObjects.end(); }
	inline size_t GetCurObjectNum() const { return m_listObjects.size(); }

	bool IsEmpty() const;
	void Clear();

	void RegisterObjectSample(T&& sample);
	void DeregisterObjectSample();

	virtual list_iter_type CreateObject(int x, int y) = 0;
	virtual list_iter_type DeleteObject(size_t index) = 0;
	virtual list_iter_type DeleteObject(list_iter_type iter) = 0;

	virtual void DrawObjects() { ; }
	virtual void DeleteDeadObjects() { ; }

	inline bool HasRenderComp() const    { return m_listObjects.front()->GetRenderComp()    != nullptr; }
	inline bool HasAnimationComp() const { return m_listObjects.front()->GetAnimationComp() != nullptr; }
	inline bool HasPhysicsComp() const   { return m_listObjects.front()->GetPhysicsComp()   != nullptr; }
	inline bool HasLifeStateComp() const { return m_listObjects.front()->GetLifeStateComp() != nullptr; }
	inline bool HasHealthComp() const	 { return m_listObjects.front()->GetHealthComp()	!= nullptr; }

	inline bool GetWidth() const { return m_szWidth; }
	inline bool GetHeight() const { return m_szHeight; }
	inline void SetWidth(size_t width) { m_szWidth = width; }
	inline void SetHeight(int height) { m_szHeight = height; }

protected:
	std::list<std::unique_ptr<T>> m_listObjects;
	//std::shared_ptr<ImageData> m_pImageData;
	T* m_pObjectSample;
	size_t m_szMaxObjectNum;
	
	size_t m_szWidth;
	size_t m_szHeight;
};

template <class T>
GameObjectManager<T>::GameObjectManager(size_t maxObjectNum /* = 1 */)
	: m_szMaxObjectNum(maxObjectNum)
	, m_pObjectSample(nullptr)
	, m_szWidth(0)
	, m_szHeight(0)
{
	static_assert(std::is_base_of_v<CGameObject, T>, "T must be a derived class of the CGameObject class.");
}

template <class T>
GameObjectManager<T>::~GameObjectManager()
{
	Clear();
}

template <class T>
bool GameObjectManager<T>::IsEmpty() const
{
	return m_listObjects.empty();
}

template <class T>
void GameObjectManager<T>::Clear()
{
	m_listObjects.clear();
	m_szMaxObjectNum = 0;
	m_szWidth = 0;
	m_szHeight = 0;
	SAFE_DELETE(m_pObjectSample);
}

////////////////////////////////////////////////////////////
// Name: RegisterObjectSample()
// Desc: ���� ������Ʈ�� ����մϴ�. 
// �ش� ������Ʈ�� ������� ������Ʈ Ǯ�� �����մϴ�.
////////////////////////////////////////////////////////////
template<class T>
void GameObjectManager<T>::RegisterObjectSample(T&& sample)
{
	if (m_pObjectSample == nullptr)
	{
		m_pObjectSample = new T;
	}

	*m_pObjectSample = std::move(sample);
}

////////////////////////////////////////////////////////////
// Name: DeregisterObjectSample()
// Desc: ���� ������Ʈ�� ���� �� �����մϴ�.
////////////////////////////////////////////////////////////
template<class T>
void GameObjectManager<T>::DeregisterObjectSample()
{
	SAFE_DELETE(m_pObjectSample);
}
