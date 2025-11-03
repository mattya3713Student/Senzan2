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
    };
}