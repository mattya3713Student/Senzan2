#pragma once
#include "../Combat.h"

class Player;

/**************************************************
*	プレイヤーの攻撃一段目のステート(派生).
*	担当:淵脇 未来.
**/

namespace PlayerState {
class AttackCombo_0 final : public Combat
{
public:
    AttackCombo_0(Player* owner);
    ~AttackCombo_0();

    // IDの取得.
    constexpr PlayerState::eID GetStateID() const override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;

private:    

private:    
    DirectX::XMFLOAT3 m_MoveVec;        // 移動方向.
	bool              m_isComboAccepted;// コンボ受付フラグ.
	bool              m_isAttackColliderEnabled; // 当たり判定の有効化フラグ

	// デバッグ: ImGuiで制御するための設定値（常にステート経過時間で制御）
	float m_EnableByAnimTime = 0.5f;   // ステート経過時間で有効化する開始時刻(秒)
	float m_EnableDurationAnim = 0.5f; // ステート経過時間で有効にする継続時間(秒)

	// コライダーの一度だけ呼び出すためのフラグ
	bool m_HasActivatedCollider = false;
	bool m_HasDeactivatedCollider = false;
};
}

