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
	enum class enShout : byte
	{
		none,
		Shout,
		ShoutTime,
		ShoutToIdol,
	};
public:
	BossShoutState(Boss* owner);
	~BossShoutState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

private:
	void BossAttack() override;
private:
	std::shared_ptr<BossIdolState> m_pBossIdol;

	// 変換用タイマー
	float m_TransitionTimer = 60.0f;

	enShout m_List;
};
