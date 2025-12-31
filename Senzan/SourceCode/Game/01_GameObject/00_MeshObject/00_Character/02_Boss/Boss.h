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
class BossIdolState;		//待機状態.
class BossMoveState;		//左右移動動状態.
class BossAttackStateBase;	//攻撃ベースクラス.
class BossStompState;		//踏みつけ攻撃
class BossSlashState;		//斬る攻撃.
class BossChargeSlashState;	//溜め攻撃.
class BossShoutState;		//叫び攻撃.
class BossSpecialState;
class BossLaserState;
class BossDeadState;

class BossThrowingState;

class BossChargeState;

class SkinMesh;

/***********************************************************************
*	ボスキャラクラス.
**/
class Boss
	: public Character
{
	friend BossIdolState;
	friend BossMoveState;
	friend BossStompState;
	friend BossSlashState;
	friend BossChargeSlashState;
	friend BossShoutState;
	friend BossSpecialState;
	friend BossLaserState;
	friend BossDeadState;
	friend BossChargeState;
	friend BossThrowingState;

	//ボスのアニメーションの列挙.
	enum class enBossAnim : byte
	{
		Idol = 0,		//待機.

		RunToIdol,		//走りから待機.
		Run,			//走り中.
		IdolToRun,		//待機から走り.

		Hit,			//被弾.

		ChargeToIdol,	//ため攻撃から待機.
		ChargeAttack,	//ため攻撃中.
		Charge,			//ためている.

		RightMove,		//右に進.
		LeftMove,		//左に進.

		SpecialToIdol,	//特殊攻撃と踏みつけ終了時に待機.
		Special_1,		//特殊攻撃と踏みつけ中.
		Special_0,		//飛ぶ.

		FlinchToIdol,	//怯みから待機.
		Flinch,			//怯み中.
		FlinchParis,	//怯み(パリィ).

		Dead,			//死亡.

		LaserEnd,		//レーザーから待機.
		Laser,			//レーザー中.
		LaserCharge,	//レーザーのため.

		SlashToIdol,	//斬る攻撃から待機.
		Slash,			//斬る攻撃.

		none,			//何もしない.
	};

public:
	Boss();
	~Boss() override;

	void Update() override;
	void LateUpdate() override;
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

	// 衝突_被ダメージ.
	void HandleDamageDetection() override;
	// 衝突_攻撃判定.
	void HandleAttackDetection() override;
	// 衝突_回避.
	void HandleDodgeDetection() override;

	// 衝突_回避.
	void HandleParryDetection();


	// 攻撃判定のActive
	inline void SetAttackColliderActive(bool Active) const noexcept { m_pAttackCollider->SetActive(Active); }

	//当たり判定を取得する.
	ColliderBase* GetSlashCollider() const;
	ColliderBase* GetStompCollider() const;

	void UpdateSlashColliderTransform();

	void UpdateStompColliderTransform();
protected:
	//ステートマシンのメンバ変数.
	std::unique_ptr<StateMachine<Boss>> m_State;

	DirectX::XMFLOAT3					m_PlayerPos;
	DirectX::XMFLOAT3					m_PlayerVelocity;

	float m_MoveSpped = 0.0f;

	float m_TurnSpeed;
	float m_MoveSpeed;

	D3DXVECTOR3 m_vCurrentMoveVelocity;

	ColliderBase* m_pAttackCollider;	// 攻撃判定.

	float deleta_time;

	float m_HitPoint;

	ColliderBase* m_pSlashCollider;
	ColliderBase* m_pStompCollider;
};

/*
* void Boss::UpdateSlashColliderTransform()
{
	if (GetAttachMesh().expired() || !m_pSlashCollider) return;
	auto skinMesh = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock());
	if (!skinMesh) return;

	// 1. ボーンのローカル行列を取得
	const std::string targetBoneName = "boss_Hand_R";
	DirectX::XMMATRIX bone_local_matrix;
	if (!skinMesh->GetMatrixFromBone(targetBoneName.c_str(), &bone_local_matrix)) return;

	// 2. ボスのワールド行列を取得
	DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();

	// 3. ボーンの現在のワールド位置を計算
	DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

	// 行列からワールド座標・回転を抽出
	DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
	DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);

	// 【重要】コライダーがボスの子(Transformを共有)である場合、
	// ボス自身のワールド座標を引いて「相対座標」にする必要があります。
	// ※ Transformに GetPosition() がない場合、GetWorldMatrix() から抽出するのが確実です。
	DirectX::XMVECTOR b_pos, b_quat, b_scale;
	DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);

	// 相対位置と相対回転の計算
	DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
	DirectX::XMVECTOR relative_quat = DirectX::XMQuaternionMultiply(v_final_quat, DirectX::XMQuaternionInverse(b_quat));

	DirectX::XMFLOAT3 f_relative_pos;
	DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);

	// コライダーに反映
	m_pSlashCollider->SetPositionOffset(f_relative_pos.x, f_relative_pos.y, f_relative_pos.z);

	// 回転のオフセット設定関数が ColliderBase にあれば追加
	// m_pSlashCollider->SetRotationOffset(relative_quat); 
}

*/