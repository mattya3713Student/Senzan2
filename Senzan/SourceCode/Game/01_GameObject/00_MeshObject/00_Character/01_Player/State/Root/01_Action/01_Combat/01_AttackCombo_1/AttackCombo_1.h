#pragma once
#include "../Combat.h"
#include <vector>

class Player;

/**************************************************
*	プレイヤーの攻撃二段目のステート(派生).
*	担当:淵脇 未来.
**/

namespace PlayerState {
class AttackCombo_1 final : public Combat
{
public:
    AttackCombo_1(Player* owner);
    ~AttackCombo_1();

    // IDの取得.
    constexpr PlayerState::eID GetStateID() const override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;

private:
    DirectX::XMFLOAT3 m_MoveVec;        // 移動方向.
    bool              m_isComboAccepted;// コンボ受付フラグ.

    // 当たり判定制御（ステート経過時間で有効化/無効化）
    bool  m_isAttackColliderEnabled = false;

    struct ColliderWindow {
        float start = 0.0f;    // 開始時刻(ステート秒)
        float duration = 0.1f; // 継続時間(秒)
        bool activated = false; // 既に有効化処理を行ったか
        bool deactivated = false; // 既に無効化処理を行ったか
    };

    std::vector<ColliderWindow> m_ColliderWindows; // 可変長のウィンドウリスト

    // 現在有効になっているウィンドウ数（0ならコライダー無効）
    int m_ActiveWindowCount = 0;

    // 一度だけ切り替えるためのフラグ（稀に入れておくがウィンドウ毎に管理）
    bool m_HasActivatedCollider = false;
    bool m_HasDeactivatedCollider = false;
};
}

