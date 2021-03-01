////////////////////////////////////////////
// ScoreManager.h
// 게임 점수 관련 기능을 관리합니다. (싱글톤) 
////////////////////////////////////////////

#pragma once

class ScoreManager final
{
public:
	~ScoreManager();

	static ScoreManager* GetInstance();
	void Initialize(int maxScore);

	void GainScore(int value);
	void LostScore(int value);

	inline int GetScore() const { return m_iCurScore; }
	inline void SetMaxScore(int maxScore) { m_iMaxScore = maxScore; }

private:
	ScoreManager();
	void DeleteScoreManager();

	static ScoreManager* m_pScoreManager; // 싱글톤 인스턴스
	int m_iCurScore;					  // 현재 점수
	int m_iMaxScore;					  // 얻을 수 있는 최고 점수
};