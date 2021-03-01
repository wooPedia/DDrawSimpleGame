#pragma once

#include "GameObjectManager.h"
#include "../../Util/Header/ImageData.h"

#include <list>
#include <memory>

extern MyDDraw* g_pDDraw;

// 플레이어같이 1개만 존재하는 오브젝트를 다루는 오브젝트 매니저 클래스
// Object pool을 가지지 않음
template <class T>
class SingleObjectManager : public GameObjectManager<T>
{
	using list_iter_type = typename std::list<std::unique_ptr<T>>::iterator;

public:
	SingleObjectManager();
	virtual ~SingleObjectManager();

	inline void CreateObjectUsingSample() 
	{
		this->m_listObjects.clear();
		std::unique_ptr<T> uptrObj = std::make_unique<T>(*this->m_pObjectSample);
		this->m_listObjects.emplace_back(std::move(uptrObj));
	}

	template <typename... Type>
	list_iter_type CreateObject(Type... _Args);
	virtual list_iter_type CreateObject(int x, int y) override;
	virtual list_iter_type DeleteObject(size_t index) override;
	virtual list_iter_type DeleteObject(list_iter_type iter) override;
	
	virtual void DrawObjects() override;
	virtual void DeleteDeadObjects() override;

	virtual void GetPos(OUT INT_VECTOR2* ivPos);

protected:
};

template <class T>
SingleObjectManager<T>::SingleObjectManager()
	: GameObjectManager<T>(1)
{
}

template <class T>
SingleObjectManager<T>::~SingleObjectManager()
{
}

template <class T>
typename SingleObjectManager<T>::list_iter_type SingleObjectManager<T>::CreateObject(int x, int y)
{
	list_iter_type itObject = this->m_listObjects.end();

	if (this->m_szMaxObjectNum == 0)
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		return itObject;
	}

	// 소유한 오브젝트의 수가 최대 소유 가능한 수를 넘어가지 않도록
	// (x, y) 위치에 Object를 생성
	if (this->m_listObjects.size() < this->m_szMaxObjectNum)
	{
		this->m_listObjects.emplace_back(std::make_unique<T>(x, y));
		--itObject;
	}

	return itObject;
}

template <class T>
typename SingleObjectManager<T>::list_iter_type SingleObjectManager<T>::DeleteObject(size_t index)
{
	this->m_listObjects.clear();
	return this->m_listObjects.end();
}

template <class T>
typename SingleObjectManager<T>::list_iter_type SingleObjectManager<T>::DeleteObject(list_iter_type iter)
{
	this->m_listObjects.clear();
	return this->m_listObjects.end();
}

template <class T>
void SingleObjectManager<T>::DrawObjects()
{
	INT_VECTOR2 ivPos = {};
	this->GetPos(&ivPos);
	std::unique_ptr<T>& pObj = this->m_listObjects.front();

#ifdef _DEBUG
	if (   pObj->GetAnimationComp()   == nullptr 
		&& pObj->GetRenderComp() == nullptr
	)
	{
		__debugbreak(); // Draw를 위해선 두 컴포넌트 중 반드시 하나의 컴포넌트를 가지고 있어야함
	}
#endif // _DEBUG

	// 애니메이션을 가지는 오브젝트면 애니메이션 이미지를 그리고
	// 아니라면 단일한 이미지를 그림
	if (pObj->GetAnimationComp())
	{
		pObj->GetAnimationComp()->Draw(&ivPos);
	}
	else
	{
		pObj->GetRenderComp()->Draw(&ivPos);
	}
	pObj->SetPos(ivPos);
}

////////////////////////////////////////////////////////////
// Name: DeleteDeadObjects()
// Desc: 죽거나 유효하지 않은 상태의 오브젝트를 삭제합니다.
////////////////////////////////////////////////////////////
template <class T>
void SingleObjectManager<T>::DeleteDeadObjects()
{
	std::unique_ptr<T>& pObj = this->m_listObjects.front();

#ifdef _DEBUG
	// 해당 객체가 Health 또는 LifeState 컴포넌트 둘 중 하나를 가져야 유효한 동작
	if (pObj->GetHealthComp() == nullptr && pObj->GetLifeStateComp() == nullptr)
	{
		// 정의되지 않은 동작
		__debugbreak();
	}
#endif // _DEBUG

	if (pObj->GetHealthComp())
	{
		if (pObj->GetHealthComp()->IsDead())
		{
			this->DeleteObject(0);
		}
	}
	else
	{
		if (pObj->GetLifeStateComp()->IsExpired())
		{
			this->DeleteObject(0);
		}
	}
}

#include <list>
template <class T>
void SingleObjectManager<T>::GetPos(OUT INT_VECTOR2* ivPos)
{
	if (!this->m_listObjects.empty())
	{
		(this->m_listObjects.front())->GetPos(ivPos);
		return;
	}

#ifdef _DEBUG
		__debugbreak();
#endif
}

template<class T>
template<typename ...Type>
typename SingleObjectManager<T>::list_iter_type  // return type
SingleObjectManager<T>::CreateObject(Type ..._Args)
{
	if (this->m_listObjects.size() >= this->m_szMaxObjectNum)
	{
		this->m_listObjects.clear();
	}

	// 새 오브젝트 추가 후 새 오브젝트를 가리키는 반복자 반환
	list_iter_type itObject = this->m_listObjects.end();
	std::unique_ptr<T> uptrObject(_Args...);
	this->m_listObjects.emplace_back(std::move(uptrObject));

	--itObject;

	return itObject;
}
