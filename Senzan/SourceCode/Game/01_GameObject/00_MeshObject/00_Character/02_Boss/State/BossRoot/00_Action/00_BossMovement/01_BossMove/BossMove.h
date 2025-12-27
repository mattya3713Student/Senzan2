#pragma once

#include "..//BossMovement.h"
#include <DirectXMath.h>

class Boss;

namespace BossState
{
    class BossMove final : public BossMovement
    {
    public:
        // フェーズの定義
        enum class MovePhase : unsigned char
        {
            Start,
            Run,
            Stop,
            Strafe,
        };

        BossMove(Boss* pOwner);
        ~BossMove();

        // 毎フレームの更新処理（BossMovement クラスの仮想関数をオーバーライド想定）
        // ※関数名は基底クラスに合わせて調整してください
        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

        constexpr BossState::enID GetStateID() const override;

    private:
        MovePhase m_Phase = MovePhase::Start;

        // --- 変数名を統一 ---
        float m_Angle = 0.0f;          // 円運動用の現在の角度
        float m_StrafeSpeed = 0.8f;    // 左右移動（回り込み）の速度

        float m_RotationAngle = 0.0f;
        double m_RotationSpeed = 50.0f;
        float m_rotationDirection = 1.0f; // 1.0f:順方向, -1.0:逆方向

        float m_BaseAngle = 0.0f;
        float m_Timer = 0.0f;

        // 定数
        static constexpr float STRAFE_RANGE = 20.0f;
        static constexpr float APPROACH_SPEED = 10.0f;
        static constexpr double MOVE_RUN_ANIM_SPEED = 50.0;
        static constexpr double STRAFE_ANIM_SPEED = 12.0;
        static constexpr float TRACKING_DELAY = 0.3f;


    };
}