#pragma once
#include "../System.h"

class Player;

/**************************************************
*	�v���C���[�̕K�E�Z�̃X�e�[�g(�h��).
*	�S��:���e ����.
**/

namespace PlayerState {
    class SpecialAttack final : public System
    {
    public:
        SpecialAttack(Player* owner);
        ~SpecialAttack();

        // ID�̎擾.
        constexpr  PlayerState::eID GetStateID() const override;

        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

    private:
        float m_CurrentTime = 0.0f;      // 経過時間
        bool m_IsHitboxActive = false;   // ヒットボックスがアクティブか
    };
}