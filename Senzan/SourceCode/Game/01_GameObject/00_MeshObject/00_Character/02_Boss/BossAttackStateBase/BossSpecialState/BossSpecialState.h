#pragma once
#include "00_MeshObject//00_Character//02_Boss//BossAttackStateBase//BossAttackStateBase.h"

/*******************************************************************
*	ボスの特殊攻撃.
**/

class BossSpecialState final
	: public BossAttackStateBase
{
public:
	BossSpecialState(Boss* owner);
	~BossSpecialState() override;

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
		Charge,		//ため.
		Jump,		//飛んでプレイヤーの方へ行く.
		Attack,		//突進ぎり.
		Cooldown	//硬直.
	};

	Phase m_CurrentPhase;
	float m_PhaseTime;
	float m_AttackTime;
	bool m_HasHit;

	//攻撃パラメーター.
	const float m_ChargeDuration;
	const float m_JumpDuration;
	const float m_AttackDuration;
	const float m_CoolDownDuration;

	const float m_JumpHeght;

	const float m_AttackSpeed;
	const float m_SlashRange;

	DirectX::XMFLOAT3 m_StartPos;       // 攻撃開始時の位置（Jumpの始点）
	DirectX::XMFLOAT3 m_JumpTargetPos;  // Jumpフェーズの目標地点（突進の始点）
	DirectX::XMFLOAT3 m_AttackDir;  // 突進方向（Attackフェーズの開始時に決定）
	
};