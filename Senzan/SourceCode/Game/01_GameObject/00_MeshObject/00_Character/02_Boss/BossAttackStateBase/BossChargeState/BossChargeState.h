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

	//ため時間のメンバ変数/
	float m_Timer = 0.0f;
	//遷移の時間のタイム変数.
	float m_TransitionTimer = 120.0f;
	//今ためているのかどうかのフラグ.
	bool ChargeFlag = false;
};