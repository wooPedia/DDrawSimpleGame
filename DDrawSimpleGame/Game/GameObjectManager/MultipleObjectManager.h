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

// object pool을 가짐
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
// Desc: (x, y) 위치에 오브젝트를 생성합니다.
// 새 오브젝트는 오브젝트 풀에서 가져옵니다.
////////////////////////////////////////////////////////////
template <class T>
typename MultipleObjectManager<T>::list_iter_type MultipleObjectManager<T>::CreateObject(int x, int y)
{
	list_iter_type itObject = this->m_listObjects.end();

	if (this->m_szMaxObjectNum == 0)
	{
		goto lb_false;
	}

	// 소유 가능한 최대 오브젝트 수를 넘지 않도록
	if (this->m_listObjects.size() < this->m_szMaxObjectNum)
	{
		if (this->m_sptrObjectPool->GetAvailableSize() > 0)
		{
			std::unique_ptr<T> pObject = std::move(this->m_sptrObjectPool->Get());
			pObject->ResetData(this->m_pObjectSample); // 객체를 초기 상태로 리셋(데이터 해제가 아님)
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
// Desc: [index] 위치의 오브젝트를 삭제합니다.(오브젝트 풀로 반환)
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
		// 제거된 요소의 다음 요소를 가리키는 반복자 반환
		// i.e. {1,2,3,4} 에서 3 제거 시 4를 가리키는 반복자 반환
		this->m_sptrObjectPool->ReturnObject(std::move(*itTarget));
		itTarget = this->m_listObjects.erase(itTarget);
	}

	return itTarget;
}

////////////////////////////////////////////////////////////
// Name: DeleteObject()
// Desc: iter가 가리키는 오브젝트를 삭제합니다.(오브젝트 풀로 반환)
////////////////////////////////////////////////////////////
template <class T>
typename MultipleObjectManager<T>::list_iter_type MultipleObjectManager<T>::DeleteObject(list_iter_type iter)
{
	list_iter_type itNext = this->m_listObjects.end();
	if (iter != this->m_listObjects.end())
	{
		// 제거된 요소의 다음 요소를 가리키는 반복자 반환
		this->m_sptrObjectPool->ReturnObject(std::move(*iter));
		itNext = this->m_listObjects.erase(iter);
	}

	return itNext;
}

////////////////////////////////////////////////////////////
// Name: DrawObjects()
// Desc: 소유하고 있는 오브젝트들을 그립니다.
////////////////////////////////////////////////////////////
template <class T>
void MultipleObjectManager<T>::DrawObjects()
{
	if (this->IsEmpty())
	{
		return;
	}

	INT_VECTOR2 ivPos = {};

	// 단일 이미지와 애니메이션인 경우를 나누어서 진행
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
// Desc: Dead 상태 또는 Disable 상태의 적을 제거합니다.
////////////////////////////////////////////////////////////
template <class T>
void MultipleObjectManager<T>::DeleteDeadObjects()
{
	if (this->IsEmpty())
	{
		return;
	}

	// HealthComp or LifeStateComp를 가진 오브젝트 대상으로 수행
	if (this->HasHealthComp())
	{
		auto itObject = this->GetBegin();
		while (itObject != this->GetEnd())
		{
			if ((*itObject)->GetHealthComp()->IsDead())
			{
				itObject = this->DeleteObject(itObject);

				// 적기가 죽었다면 스코어 획득
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
// Desc: 오브젝트 풀을 생성합니다.
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
// Desc: 할당한 데이터를 해제합니다.
////////////////////////////////////////////////////////////
template <class T>
void MultipleObjectManager<T>::Release()
{
	m_sptrObjectPool->Release();
}
