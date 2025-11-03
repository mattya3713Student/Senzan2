#pragma once
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossAttackStateBase.h"


class BossLaserState
	: public BossAttackStateBase
{
public:
	BossLaserState(Boss* owner);
	~BossLaserState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;
private:
	void BossAttack() override;
private:
	enum class Phase
	{
		Charge,		//ため時間
		Attack,		//攻撃
		CoolDown	//クールダウン(状態の繊維).
	};

	const float m_ChargeDuration;
	const float m_AttackDuration;
	const float m_CoolDownDuration;

	//レーザー実装のパラメータ.
	const float m_LaserLenght;	//最大の長さ.
	const float m_SweepSpeed;	//下から上えの凪払いの速度.
	const float m_StartAnglePitch;	//攻撃開始時の角度.
	const float m_EndAnglePitch;	//終了字の角度.

	Phase m_CurrentPhase;
	float m_PhaseTime;
	float m_CurrentAngle;	//現在のレーザーの位置.
	DirectX::XMFLOAT3 m_LaserOridin;//レーザーの生成場所.
};
