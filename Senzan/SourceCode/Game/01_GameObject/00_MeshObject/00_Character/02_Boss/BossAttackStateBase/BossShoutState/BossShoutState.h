#pragma once
#include "..//BossAttackStateBase.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"

/************************************************************************************
*	叫び攻撃：叫びの攻撃は当たり判定でノックバックさせるのでここでは時間でのIdolの遷移を書く.
**/


class BossIdolState;

class BossShoutState
	: public BossAttackStateBase
{
public:
	BossShoutState(Boss* owner);
	~BossShoutState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	void BoneDraw();

private:
	void BossAttack() override;
private:
	std::shared_ptr<BossIdolState> m_pBossIdol;

	//遷移のタイミング取得用のタイム変数.
	float m_Timer = 0.0f;

	//遷移の時間のタイム変数.
	float m_TransitionTimer = 60.0f;
};