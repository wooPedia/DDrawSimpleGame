#pragma once

#include "GameObjectManager.h"
#include "../../Util/Header/ImageData.h"

#include <list>
#include <memory>

extern MyDDraw* g_pDDraw;

// �÷��̾�� 1���� �����ϴ� ������Ʈ�� �ٷ�� ������Ʈ �Ŵ��� Ŭ����
// Object pool�� ������ ����
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

	// ������ ������Ʈ�� ���� �ִ� ���� ������ ���� �Ѿ�� �ʵ���
	// (x, y) ��ġ�� Object�� ����
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
		__debugbreak(); // Draw�� ���ؼ� �� ������Ʈ �� �ݵ�� �ϳ��� ������Ʈ�� ������ �־����
	}
#endif // _DEBUG

	// �ִϸ��̼��� ������ ������Ʈ�� �ִϸ��̼� �̹����� �׸���
	// �ƴ϶�� ������ �̹����� �׸�
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
// Desc: �װų� ��ȿ���� ���� ������ ������Ʈ�� �����մϴ�.
////////////////////////////////////////////////////////////
template <class T>
void SingleObjectManager<T>::DeleteDeadObjects()
{
	std::unique_ptr<T>& pObj = this->m_listObjects.front();

#ifdef _DEBUG
	// �ش� ��ü�� Health �Ǵ� LifeState ������Ʈ �� �� �ϳ��� ������ ��ȿ�� ����
	if (pObj->GetHealthComp() == nullptr && pObj->GetLifeStateComp() == nullptr)
	{
		// ���ǵ��� ���� ����
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

	// �� ������Ʈ �߰� �� �� ������Ʈ�� ����Ű�� �ݺ��� ��ȯ
	list_iter_type itObject = this->m_listObjects.end();
	std::unique_ptr<T> uptrObject(_Args...);
	this->m_listObjects.emplace_back(std::move(uptrObject));

	--itObject;

	return itObject;
}
