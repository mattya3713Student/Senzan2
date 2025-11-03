#pragma once
#include "..//BossAttackStateBase.h"
#include "Game\03_Collision\Capsule\CapsuleCollider.h"

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
	//フェーズ管理用列挙体.
	enum class Phase
	{
		Charge,		//予兆(溜め時間).
		Attack,		//叫び(ノックバック).
		CoolDown	//硬直.
	};

	//攻撃パラメータ.
	const float m_ChargeDuration;	//予兆の時間.
	const float m_AttackDuration;	//攻撃判定の時間.
	const float m_CoolDownDuration;	//硬直時間.

	const float m_ShoutRadius;		//叫び攻撃の範囲.
	const float m_KnockBackForce;	//プレイヤーを飛ばす水平方向の強さ.
	const float m_KnockBackUpForce;	//プレイヤーを飛ばす垂直方向の強さ.

	Phase m_CurrentPhase;			//現在のフェーズ.
	float m_PhaseTime;				//フェーズ開始からの時間.
	bool m_HasHit;					//判定を一度だけ行うためのフラグ.

	//std::shared_ptr<CapsuleCollider> m_pColl;
};