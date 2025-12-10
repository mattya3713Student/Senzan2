#pragma once

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"
#include "System/Singleton/BossAttackManager/BossAttackManager.h"	

#include "System/Utility/StateMachine/StateMachine.h"
#include "Game\\01_GameObject\\00_MeshObject\\00_Character\01_Player\Player.h"

////ステートマシンクラスの前方宣言.
//template<typename FSM_Owner> class StateMachine;

//=====================================================================
// ボスクラスにプレイヤーの位置を入手させるためにここに前方宣言を書く.
// Update関数の中にプレイヤーのポジションを設定する.
//=====================================================================

class SlashState;		//斬撃ステートクラス.

class SlashCharge;		//チャージ斬撃クラス.
class ChargeSlashState;	//チャージ斬撃ステートクラス.

class Shout;			//叫びクラス.
class ShoutState;		//叫び攻撃ステートクラス.

//ボスの行動関係を書く.
class BossIdleState;		//待機状態.
class BossMoveState;		//左右移動動状態.
class BossAttackStateBase;	//攻撃ベースクラス.
class BossStompState;		//踏みつけ攻撃
class BossSlashState;		//斬る攻撃.
class BossChargeSlashState;	//溜め攻撃.
class BossShoutState;		//叫び攻撃.
class BossSpecialState;
class BossLaserState;
class BossDeadState;
class TestSpecial;

class BossChargeState;

class SkinMesh;

/***********************************************************************
*	ボスキャラクラス.
**/
class Boss
	: public Character
{
	friend BossIdleState;
	friend BossMoveState;
	friend BossStompState;
	friend BossSlashState;
	friend BossChargeSlashState;
	friend BossShoutState;
	friend BossSpecialState;
	friend BossLaserState;
	friend BossDeadState;
	friend BossChargeState;

	friend TestSpecial;
public:
	Boss();
	~Boss() override;

	void Update() override;
	void Draw() override;

	void Init();

	//ステートクラスからStateMachineにアクセスする.
	StateMachine<Boss>* GetStateMachine();
	//ステートクラスから攻撃オブジェクトにアクセスする.
	//Slash* GetSlash();
	//SlashCharge* GetChargeSlsh();
	//アニメーション再生時に必要になるGet関数になっている.
	LPD3DXANIMATIONCONTROLLER GetAnimCtrl() const;

	float boss_x = 0.f;
	float boss_y = 0.f;
	float boss_z = 0.f;

	void Hit();


public:
	//プレイヤーの位置を取得するためにここにSetPlayer()を作成する.
	void SetTargetPos(const DirectX::XMFLOAT3 Player_Pos);

	DirectX::XMFLOAT3 GetTargetPos() { return m_PlayerPos; }

protected:
	//ステートマシンのメンバ変数.
	std::unique_ptr<StateMachine<Boss>> m_State;

	DirectX::XMFLOAT3					m_PlayerPos;

	float m_MoveSpped = 0.0f;

	float m_TurnSpeed;
	float m_MoveSpeed;

	D3DXVECTOR3 m_vCurrentMoveVelocity;


	float deleta_time;

	float m_HitPoint;

};