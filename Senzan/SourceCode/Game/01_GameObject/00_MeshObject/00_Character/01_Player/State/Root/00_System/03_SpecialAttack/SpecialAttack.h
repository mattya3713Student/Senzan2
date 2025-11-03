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
    };
}