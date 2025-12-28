#pragma once

#include "..//BossMovement.h"
#include <DirectXMath.h>

class Boss;

namespace BossState
{
    class BossMove final : public BossMovement
    {
    public:
        enum class MovePhase : unsigned char
        {
            Start,
            Run,
            Stop,
            Strafe,
        };

        BossMove(Boss* pOwner);
        ~BossMove();

        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

        constexpr BossState::enID GetStateID() const override;

    private:
        MovePhase m_Phase = MovePhase::Start;

        // --- 調整用変数 ---
        float m_BaseAngle = 0.0f;       // Strafe開始時の基準角度
        float m_Timer = 0.0f;           // 経過時間（サイン波用）

        // 【新】この2つで動きをコントロールします
        float m_StrafeFrequency = 1.5f; // 往復の速さ（テンポ）。値を大きくすると速く往復します。
        float m_StrafeAmplitude = DirectX::XM_PIDIV2; // 移動する幅。現在は90度（左右45度ずつ）。

        // 定数
        static constexpr float STRAFE_RANGE = 20.0f;   // プレイヤーからの距離
        static constexpr float APPROACH_SPEED = 10.0f; // 接近速度
        static constexpr double MOVE_RUN_ANIM_SPEED = 50.0;
        static constexpr double STRAFE_ANIM_SPEED = 12.0;

        // 【重要】ここを大きくすることで、移動速度を上げても円の端まで届くようになります
        static constexpr float TRACKING_DELAY = 15.0f;

        // 古い変数は混乱を防ぐため削除（または未使用）
        float m_RotationAngle = 0.0f;
    };
}