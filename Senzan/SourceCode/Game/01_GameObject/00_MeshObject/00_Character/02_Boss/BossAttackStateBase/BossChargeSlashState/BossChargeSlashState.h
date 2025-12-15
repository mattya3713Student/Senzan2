#pragma once
#include "..//BossAttackStateBase.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"


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

	//初期角度を設定する関数.
	void SetInitialAngle(float angle)
	{
		m_RotationAngle = angle;
	};
private:
	//フェーズの管理に使う.
	enum class Phase
	{
		Charge,		//ため.
		Attack,		//攻撃(斬り).
		Cooldown	//硬直.
	};
private:
	//攻撃開始時の位置.
	DirectX::XMFLOAT3 m_StartPos;
	float m_RotationAngle;

	//加算していくタイマー変数.
	float m_Timer = 0.0f;
	float m_Timers = 60.0f;

	float AnimChange;
};