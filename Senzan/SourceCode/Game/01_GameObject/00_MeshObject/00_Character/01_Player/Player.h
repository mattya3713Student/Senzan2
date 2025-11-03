#pragma once

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"

template<typename FSM_Owner> class StateMachine;

class PlayerStateBase;

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

	// Playerの最終的なDeltaTimeの取得.
	float GetDelta();

	// マッピングを初期化.
	void InitializeStateRefMap();

protected:
	std::unique_ptr<PlayerState::Root> m_RootState;	// ステートマシーン.

	// IDをキーとし、ステート参照を返すラムダ関数を値とするマップ.
	using StateRefGetter = std::function<std::reference_wrapper<PlayerStateBase>()>;
	std::map<PlayerState::eID, StateRefGetter> m_StateRefMap;

protected:

	//---MoveMent関連---.
	float m_RunMoveSpeed;		// 移動速度.
	DirectX::XMFLOAT2 m_MoveVec;// 一時保存の移動ベクトル,yをz座標として使用.

};