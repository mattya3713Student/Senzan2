#pragma once
#include "../System.h"

class Player;

/**************************************************
*	プレイヤーの必殺技のステート(派生).
*	担当:淵脇 未来.
**/

namespace PlayerState {
    class SpecialAttack final : public System
    {
    public:
        SpecialAttack(Player* owner);
        ~SpecialAttack();

        // IDの取得.
        constexpr  PlayerState::eID GetStateID() const override;

        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

    private:
        float m_CurrentTime = 0.0f;       // 経過時間
        float m_DurationTime = 0.06f;       // 攻撃間隔
        float m_DurationTimer = 0.0f;       // 攻撃間隔タイマー

        float m_AttackDuration;    // 攻撃演出時間
        float m_OraOraDamage = 25.0f;     // 連続ダメージ
        float m_AttackDamage = 300.0f;     // 必殺技ダメージ
        bool m_HasActivated = false;       // 攻撃判定発生済みか
    };
}
