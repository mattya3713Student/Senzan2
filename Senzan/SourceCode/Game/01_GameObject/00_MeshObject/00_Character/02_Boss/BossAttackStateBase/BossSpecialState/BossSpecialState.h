#pragma once
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossAttackStateBase.h"

/********************************************************************************
*	ボスの特殊攻撃クラス.
*	:このクラスではPlayerとBossの位置を取得して放物運動をさせて実装する.
**/

class Boss;
class BossIdolState;

class BossSpecialState
	: public BossAttackStateBase
{
public:
	enum class enSpecial : byte
	{
		None,		//何もしない.
		Jump,		//飛んでいるとき
		Attack,		//Playerに向かっての攻撃.
		CoolTime,	//クールタイム.
		Trans		//Idolに遷移.
	};
public:
	BossSpecialState(Boss* owner);
	~BossSpecialState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;


private:
	//============================================================
	// 特殊攻撃のCharge/Jump/Attackの時の関数
	//============================================================
	//ジャンプ時関数.
	void JumpTime();
	//アタック時関数.
	void BossAttack() override;
private:
	//============================================================
	// 特殊攻撃に必要になるメンバ変数.
	//============================================================
	enSpecial m_List;

	DirectX::XMFLOAT3 m_Velocity;

	//山なりの時の初速.
	float m_SpecialPower;
	//重力加速度.
	float m_Gravity;
	//特殊攻撃中のフラグ.
	bool m_SpecialFrag;
	//着地フラグ.
	bool m_GroundedFrag;

	//時間のメンバ変数.
	//時間加算に使用する.
	float m_Timer;
	//遷移するときに使用する.
	float m_TransitionTimer;
	//攻撃開始用のタイム変数.
	float m_AttackTimer;

	//上がる速度の設定.
	float m_UpSpeed;

	//============================================================
	// 突進攻撃に必要なメンバ変数.
	//============================================================
	DirectX::XMFLOAT3 m_TargetDirection; // 突進開始時に確定した目標方向
	float m_MaxTrackingAngle;			 // 追尾の限界角度 (度数法)
	float m_AttackMoveSpeed;			 // 突進速度
	float m_AttackDistance;				 // 突進する最大距離
	float m_DistanceTraveled;
};