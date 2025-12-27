#pragma once

//基底クラス.
#include "System/Utility/StateMachine/StateBase.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"

//前方宣言.
class Time;

//---------------------
//ボスの攻撃の前方宣言.
//---------------------
class BossSlashState;
class BossChargeState;
class BossChargeSlashState;
class BossLaserState;
class BossShoutState;
class BossSlashState;
class BossSpecialState;
class BossStompState;
class BossThrowingState;

#include <random>

/******************************************************************************
*	ボスの動作(左右移動・プレイヤーを囲むように半円を描く).
**/

class BossMoveState final
	: public StateBase<Boss>
{
public:
	enum class DistanceAttack : byte
	{
		Melee,	//近距離.
		Mid,	//中距離.
		Long,	//遠距離.
		None,	//何もしない.
	};

	enum class MovePhase : byte
	{
		Start,
		Run,
		Stop,
		Strafe,
	};

	MovePhase m_Phase = MovePhase::Start;
public:
	BossMoveState(Boss* owner);
	~BossMoveState();

	//最初に入る.
	void Enter() override;
	//動作.
	void Update() override;
	//かかなくていい.
	void LateUpdate() override;
	//描画.
	void Draw() override;
	//終わるときに一回だけ入る.
	void Exit() override;

	//void DrawBone();

public:
	//初期角度を設定する関数.
	void SetInitialAngle(float angle);
private:
	//現在のボスの回転度を確認する.
	float m_RotationAngle;
	double m_RotationSpeed;
	float m_rotationDirection;	//1.0f:順方向, -1.0:逆方向.

	int							m_AnimNo;	//アニメーション番号.
	double						m_AnimSpeed = 0.0001;// アニメーション速度.
	double						m_AnimTimer;// アニメ―ション経過時間.
	DirectX::XMFLOAT3			m_BonePos;	// ボーン座標.

	float m_BaseAngle;

	DirectX::XMFLOAT3 m_InitBossPos;

	float m_Timer = 0.0f;
	float m_SecondTimer = 120.0f;

	std::unique_ptr<BossSpecialState>		m_pAttack;
	std::unique_ptr<BossSlashState>			m_pSlash;
	std::unique_ptr<BossChargeState>		m_pCharge;
	std::unique_ptr<BossLaserState>			m_pLaser;
	std::unique_ptr<BossStompState>			m_pStomp;
	std::unique_ptr<BossThrowingState>		m_pThrowing;
	std::unique_ptr<BossShoutState>			m_pShout;

};