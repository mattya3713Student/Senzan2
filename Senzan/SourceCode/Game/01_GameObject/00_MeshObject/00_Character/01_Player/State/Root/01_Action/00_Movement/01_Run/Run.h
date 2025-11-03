#pragma once
#include "../Movement.h"

class Player;

/**************************************************
*	プレイヤーの走るのステート(派生).
*	担当:淵脇 未来.
**/

namespace PlayerState {
    class Run final : public Movement
    {
    public:
        Run(Player* owner);
        ~Run();

        // IDの取得.
        constexpr PlayerState::eID GetStateID() const  override;

        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

    private:
        // 移動方向を算出.
        void CalculateMoveVec();

    private:
    };
}