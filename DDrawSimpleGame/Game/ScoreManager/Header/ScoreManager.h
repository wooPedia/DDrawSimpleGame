////////////////////////////////////////////
// ScoreManager.h
// ���� ���� ���� ����� �����մϴ�. (�̱���) 
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

	static ScoreManager* m_pScoreManager; // �̱��� �ν��Ͻ�
	int m_iCurScore;					  // ���� ����
	int m_iMaxScore;					  // ���� �� �ִ� �ְ� ����
};