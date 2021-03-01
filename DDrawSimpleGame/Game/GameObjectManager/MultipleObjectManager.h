#pragma once
#pragma warning(disable:4984)

#include "GameObjectManager.h"
#include "../../MyDDraw/Header/MyDDraw.h"
#include "../ObjectPool/ObjectPool.h"
#include "../../DataStructure/GameData_Lua.h"

#include <list>
#include <memory>
#include <type_traits>

class GotLife;
class AirCraft;

extern GameData_Lua* g_pLuaGameData;

// object pool�� ����
template <class T>
class MultipleObjectManager : public GameObjectManager<T>
{
	using list_iter_type = typename std::list<std::unique_ptr<T>>::iterator;

public:
	MultipleObjectManager(size_t maxObjectNum);
	virtual ~MultipleObjectManager();

	virtual list_iter_type CreateObject(int x, int y) override;
	virtual list_iter_type DeleteObject(size_t index) override;
	virtual list_iter_type DeleteObject(list_iter_type iter) override;

	virtual void DrawObjects() override;
	virtual void DeleteDeadObjects() override;

	void CreateObjectPool();
	
private:
	void Release();

	std::shared_ptr<ObjectPool<T>> m_sptrObjectPool;
};

template<class T>
MultipleObjectManager<T>::MultipleObjectManager(size_t maxObjectNum)
	: GameObjectManager<T>(maxObjectNum)
{
}

template <class T>
MultipleObjectManager<T>::~MultipleObjectManager()
{
	Release();
}

////////////////////////////////////////////////////////////
// Name: CreateObject()
// Desc: (x, y) ��ġ�� ������Ʈ�� �����մϴ�.
// �� ������Ʈ�� ������Ʈ Ǯ���� �����ɴϴ�.
////////////////////////////////////////////////////////////
template <class T>
typename MultipleObjectManager<T>::list_iter_type MultipleObjectManager<T>::CreateObject(int x, int y)
{
	list_iter_type itObject = this->m_listObjects.end();

	if (this->m_szMaxObjectNum == 0)
	{
		goto lb_false;
	}

	// ���� ������ �ִ� ������Ʈ ���� ���� �ʵ���
	if (this->m_listObjects.size() < this->m_szMaxObjectNum)
	{
		if (this->m_sptrObjectPool->GetAvailableSize() > 0)
		{
			std::unique_ptr<T> pObject = std::move(this->m_sptrObjectPool->Get());
			pObject->ResetData(this->m_pObjectSample); // ��ü�� �ʱ� ���·� ����(������ ������ �ƴ�)
			pObject->SetPos(x, y);

			this->m_listObjects.emplace_back(std::move(pObject));
			--itObject;
			assert(itObject != this->m_listObjects.end());
		}
		else
		{
			goto lb_false;
		}
	}

	return itObject;

lb_false:
#ifdef _DEBUG
	__debugbreak();
#endif // _DEBUG
	return itObject;
}

////////////////////////////////////////////////////////////
// Name: DeleteObject()
// Desc: [index] ��ġ�� ������Ʈ�� �����մϴ�.(������Ʈ Ǯ�� ��ȯ)
////////////////////////////////////////////////////////////
template <class T>
typename MultipleObjectManager<T>::list_iter_type MultipleObjectManager<T>::DeleteObject(size_t index)
{
	list_iter_type itTarget = this->m_listObjects.begin();
	for (size_t i = 0; i < index; ++i)
	{
		++itTarget;
	}

	if (itTarget != this->m_listObjects.end())
	{
		// ���ŵ� ����� ���� ��Ҹ� ����Ű�� �ݺ��� ��ȯ
		// i.e. {1,2,3,4} ���� 3 ���� �� 4�� ����Ű�� �ݺ��� ��ȯ
		this->m_sptrObjectPool->ReturnObject(std::move(*itTarget));
		itTarget = this->m_listObjects.erase(itTarget);
	}

	return itTarget;
}

////////////////////////////////////////////////////////////
// Name: DeleteObject()
// Desc: iter�� ����Ű�� ������Ʈ�� �����մϴ�.(������Ʈ Ǯ�� ��ȯ)
////////////////////////////////////////////////////////////
template <class T>
typename MultipleObjectManager<T>::list_iter_type MultipleObjectManager<T>::DeleteObject(list_iter_type iter)
{
	list_iter_type itNext = this->m_listObjects.end();
	if (iter != this->m_listObjects.end())
	{
		// ���ŵ� ����� ���� ��Ҹ� ����Ű�� �ݺ��� ��ȯ
		this->m_sptrObjectPool->ReturnObject(std::move(*iter));
		itNext = this->m_listObjects.erase(iter);
	}

	return itNext;
}

////////////////////////////////////////////////////////////
// Name: DrawObjects()
// Desc: �����ϰ� �ִ� ������Ʈ���� �׸��ϴ�.
////////////////////////////////////////////////////////////
template <class T>
void MultipleObjectManager<T>::DrawObjects()
{
	if (this->IsEmpty())
	{
		return;
	}

	INT_VECTOR2 ivPos = {};

	// ���� �̹����� �ִϸ��̼��� ��츦 ����� ����
	if (this->HasRenderComp())
	{
		for (std::unique_ptr<T>& pObj : this->m_listObjects)
		{
			pObj->GetPos(&ivPos);
			pObj->GetRenderComp()->Draw(&ivPos);
			pObj->SetPos(ivPos);
		}
	}
	else // Animation
	{
		assert(this->HasAnimationComp());
		for (std::unique_ptr<T>& pObj : this->m_listObjects)
		{
			pObj->GetPos(&ivPos);
			pObj->GetAnimationComp()->Draw(&ivPos);
			pObj->SetPos(ivPos);
		}
	}
}

////////////////////////////////////////////////////////////
// Name: DeleteDeadObjects()
// Desc: Dead ���� �Ǵ� Disable ������ ���� �����մϴ�.
////////////////////////////////////////////////////////////
template <class T>
void MultipleObjectManager<T>::DeleteDeadObjects()
{
	if (this->IsEmpty())
	{
		return;
	}

	// HealthComp or LifeStateComp�� ���� ������Ʈ ������� ����
	if (this->HasHealthComp())
	{
		auto itObject = this->GetBegin();
		while (itObject != this->GetEnd())
		{
			if ((*itObject)->GetHealthComp()->IsDead())
			{
				itObject = this->DeleteObject(itObject);

				// ���Ⱑ �׾��ٸ� ���ھ� ȹ��
				if constexpr (std::is_same_v<T, AirCraft>)
				{
					ScoreManager::GetInstance()->GainScore(g_pLuaGameData->iScorePerEnemyKill);
				}
			}
			else
			{
				++itObject;
			}
		}
	}
	else // LifeState
	{
		assert(this->HasLifeStateComp());
		auto itObject = this->GetBegin();
		while (itObject != this->GetEnd())
		{
			if ((*itObject)->GetLifeStateComp()->IsExpired())
			{
				itObject = this->DeleteObject(itObject);
			}
			else
			{
				++itObject;
			}
		}
	}
}

////////////////////////////////////////////////////////////
// Name: CreateObjectPool()
// Desc: ������Ʈ Ǯ�� �����մϴ�.
////////////////////////////////////////////////////////////
template<class T>
void MultipleObjectManager<T>::CreateObjectPool()
{
	m_sptrObjectPool = std::make_shared<ObjectPool<T>>(this->m_szMaxObjectNum, this->m_pObjectSample);

	if (this->m_pObjectSample->HasImage())
	{
		this->SetWidth(this->m_pObjectSample->GetWidth());
		this->SetHeight(this->m_pObjectSample->GetHeight());
	}
}

////////////////////////////////////////////////////////////
// Name: Release()
// Desc: �Ҵ��� �����͸� �����մϴ�.
////////////////////////////////////////////////////////////
template <class T>
void MultipleObjectManager<T>::Release()
{
	m_sptrObjectPool->Release();
}
