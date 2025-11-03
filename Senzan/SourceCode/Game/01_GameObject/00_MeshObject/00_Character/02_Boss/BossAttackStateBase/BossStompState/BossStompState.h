#pragma once

#include "..//BossAttackStateBase.h"	//基底クラス.
#include "Game\03_Collision\Capsule\CapsuleCollider.h"

class BossStompState final
	: public BossAttackStateBase
{
public:
	BossStompState(Boss* owner);
	~BossStompState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	//当たり判定表示用関数.
	void BoneDraw();

private:
	//攻撃関数.
	void BossAttack() override;
private:

	enum class Phase
	{
		Idol,
		Attack,
		CoolDown
	};

	const float m_IdolDuration;
	const float m_AttackDuration;
	const float m_CoolDownDuration;

	Phase m_CurrentPhase;
	float m_PhaseTime;


	//攻撃モーションの長さ(秒).
	//これはTimeで入手する感じ.
	const float m_StopmDuration;
	//ジャンプの最大の高さ.
	const float m_JumpHeigt;
	//攻撃判定の半径.
	const float m_StopmRadius;

	//ボスの攻撃開始位置.
	//確認のため書いています
	//いらなくなったら違う方法で作成しなおします.
	DirectX::XMFLOAT3 m_InitBossPos;
	bool m_HasHitGround;

};
