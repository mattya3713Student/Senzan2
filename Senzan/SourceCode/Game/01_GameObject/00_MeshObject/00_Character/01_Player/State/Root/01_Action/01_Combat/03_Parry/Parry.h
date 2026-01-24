#pragma once
#include "../Combat.h"

class Player;

/**************************************************
*	プレイヤーのパリィのステート(派生).
*	担当:淵脇 未来.
**/

namespace PlayerState {
    class Parry final : public Combat
    {
    public:
        Parry(Player* owner);
        ~Parry();

        // IDの取得.
        constexpr PlayerState::eID GetStateID() const override;

        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

    private:
        bool m_IsParrySuccessful;
        // パリィ成功を待つための一時停止制御
        float m_ElapsedTime = 0.0f;
        float m_PauseThreshold = 0.12f; // この時間経過後にアニメ速度を0にする
        bool m_IsPaused = false;
    };
}
