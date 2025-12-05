#pragma once
#include "..//BossAttackStateBase.h"

/*******************************************************************************
*	投擲攻撃：雪だるまを投げる攻撃.
**/

class BossIdolState;

class BossThrowingState final
	: public BossAttackStateBase
{
public:
	BossThrowingState(Boss* owner);
	~BossThrowingState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;
private:
	std::shared_ptr<BossIdolState> m_pBossIdol;

	//================================================
	// 投擲のメンバ変数を作成.
	//================================================
	//遷移のタイミング取得用のタイム変数.
	float m_Timer;
	//遷移の時間のタイム変数.
	float m_TransitionTimer;
};