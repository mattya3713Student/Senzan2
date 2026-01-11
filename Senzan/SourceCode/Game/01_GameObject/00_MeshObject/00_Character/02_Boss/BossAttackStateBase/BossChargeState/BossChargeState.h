#pragma once
#include "..//BossAttackStateBase.h"

/******************************************************************************
*	ため斬り攻撃用のChargeStateクラス.
**/

class Boss;
class BossChargeSlashState;

class BossChargeState final
	: public BossAttackStateBase
{
public:
	BossChargeState(Boss* owner);
	~BossChargeState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	//攻撃を実行させる関数.
	void BossAttack() override;
private:
	std::shared_ptr<BossChargeSlashState> m_ChargeSlash;

	// timers moved to base: float m_Timer, m_TransitionTimer
	// ためフラグ
	bool ChargeFlag = false;
};
