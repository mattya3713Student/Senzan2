#pragma once

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"

template<typename FSM_Owner> class StateMachine;

class PlayerStateBase;
class CameraManager;

// 前方宣言.
namespace PlayerState {
class Root;
enum class eID;

class Pause;
class KnockBack;
class Dead;
class SpecialAttack;
class Idle;
class Run;
class AttackCombo_0;
class AttackCombo_1;
class AttackCombo_2;
class Parry;
class DodgeExecute;
class JustDodge;
}


/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/10/5.
* @brief     : プレイヤークラス.
**********************************************************************************/

class Player
	: public Character
{
	friend PlayerStateBase;
	friend PlayerState::Root;
	friend PlayerState::Pause;
	friend PlayerState::KnockBack;
	friend PlayerState::Dead;
	friend PlayerState::SpecialAttack;
	friend PlayerState::Idle;
	friend PlayerState::Run;
	friend PlayerState::AttackCombo_0;
	friend PlayerState::AttackCombo_1;
	friend PlayerState::AttackCombo_2;
	friend PlayerState::Parry;
	friend PlayerState::DodgeExecute;
	friend PlayerState::JustDodge;

	enum class eAnim : uint8_t
	{
		Idle = 0,
		Run,
		Attack_0,
		Attack_1,
		Attack_2,
		SpecialAttack,
		Parry,
		Dodge,
		KnockBack,
		Dead,
	};

	// 定数.
protected:

	// 減速.
	static constexpr float m_MoveVecDeceleration = 0.05f;

public:
	Player();
	virtual ~Player() override;

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Draw() override;

	bool IsKnockBack() const noexcept;	// スタン中か.
	bool IsDead() const noexcept;		// 死亡中か.
	bool IsPaused() const noexcept;		// ポーズ中か.

	// ステートの変更.
	void ChangeState(PlayerState::eID id) const;

	std::reference_wrapper<PlayerStateBase> GetStateReference(PlayerState::eID id);

private:

	// マッピングを初期化.
	void InitializeStateRefMap();

	// 衝突_被ダメージ.
	void HandleDamageDetection() override;
	// 衝突_攻撃判定.
	void HandleAttackDetection() override;
	// 衝突_回避.
	void HandleDodgeDetection() override;

protected:

	// Playerの最終的なDeltaTimeの取得.
	float GetDelta();

protected:
	std::unique_ptr<PlayerState::Root> m_RootState;	// ステートマシーン.

	// IDをキーとし、ステート参照を返すラムダ関数を値とするマップ.
	using StateRefGetter = std::function<std::reference_wrapper<PlayerStateBase>()>;
	std::map<PlayerState::eID, StateRefGetter> m_StateRefMap;

protected:


	//---共有---.
	PlayerState::eID	m_NextStateID;		// 次遷移ステート.
	bool m_IsStateChangeRequest;			// 次遷移ステートフラグ.

	DirectX::XMFLOAT3	m_MoveVec;			// 一時保存の移動ベクトル.

	//---System関連---.
	bool				m_IsKnockBack;		// ノックバック中か否か.
	DirectX::XMFLOAT3	m_KnockBackVec;		// ノックバックのベクトル.
	float				m_KnockBackPower;	// ノックバックの強さ(被ダメの量に比例する予定).

	//---MoveMent関連---.
	float				m_RunMoveSpeed;		// 移動速度.

};