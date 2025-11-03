#pragma once
#include "..//BossAttackStateBase.h"


/**********************************************************
*	ボス攻撃溜め攻撃.
**/

class BossChargeSlashState
	: public BossAttackStateBase
{
public:
	BossChargeSlashState(Boss* owner);
	~BossChargeSlashState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	void BoneDraw();

private:
	void BossAttack() override;
private:
	//フェーズの管理に使う.
	enum class Phase
	{
		Charge,		//ため.
		Attack,		//攻撃(斬り).
		Cooldown	//硬直.
	};
private:
	//攻撃パラメータ.
	//溜めの時間.
	const float m_ChargeDuration;
	//斬りの攻撃時間.
	const float m_SlashDuration;
	//硬直時間.
	const float m_CooldownDuration;

	//斬り攻撃のパラメータ.
	//攻撃範囲の最大.
	const float m_SlashRange;
	//扇形の攻撃角度.
	const float m_SlashAngle;

	//現在のフェーズ.
	Phase m_CurrentPhase;
	//フェーズ開始からの時間.
	float m_PhaseTime;

	//攻撃開始時の位置.
	DirectX::XMFLOAT3 m_StartPos;
	//判定を一度だけ行うためのフラグ.
	bool m_HasHit;
};