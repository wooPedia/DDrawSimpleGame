//////////////////////////////////////
// ScoreManager.cpp
// ���� ���� ���� ����� �����մϴ�. 
//////////////////////////////////////

#include "../Header/ScoreManager.h"

// Static ���� �ʱ�ȭ
ScoreManager* ScoreManager::m_pScoreManager = nullptr;

ScoreManager::ScoreManager()
	: m_iCurScore(0)
	, m_iMaxScore(0)
{
}

ScoreManager::~ScoreManager()
{
	DeleteScoreManager();
}

ScoreManager* ScoreManager::GetInstance()
{
	static bool bExist = false;

	if (!bExist)
	{
		m_pScoreManager = new ScoreManager();
		if (m_pScoreManager != nullptr)
		{
			bExist = true;
		}
		else
		{
			return nullptr;
		}
	}

	return m_pScoreManager;
}

void ScoreManager::Initialize(int maxScore)
{
	SetMaxScore(maxScore);
}

void ScoreManager::GainScore(int value)
{
	int score = m_iCurScore + value;

#ifdef _DEBUG
	if (score <= 0)
		__debugbreak();
#endif // _DEBUG

	if (score > 0 && score <= m_iMaxScore)
	{
		m_iCurScore = score;
	}
	else if (score > m_iMaxScore)
	{
		m_iCurScore = m_iMaxScore;
	}
}

void ScoreManager::LostScore(int value)
{
	int score = m_iCurScore - value;

#ifdef _DEBUG
	if (score > m_iMaxScore)
		__debugbreak();
#endif // _DEBUG

	if (score > 0 && score <= m_iMaxScore)
	{
		m_iCurScore = score;
	}
	else
	{
		// ���� ������ 0��
		m_iCurScore = 0;
	}
}

void ScoreManager::DeleteScoreManager()
{
	delete m_pScoreManager;
	m_pScoreManager = nullptr;
	m_iCurScore = 0;
	m_iMaxScore = 0;
}
