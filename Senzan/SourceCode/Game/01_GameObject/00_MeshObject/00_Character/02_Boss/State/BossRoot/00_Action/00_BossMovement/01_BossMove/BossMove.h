#pragma once

#include "..//BossMovement.h"
#include <DirectXMath.h>

class Boss;

namespace BossState
{
    class BossMove final
        : public BossMovement
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
        //列挙の変数名.
        MovePhase m_Phase;

        //変数.
        float m_BaseAngle;       //Strafe開始時の基準角度.
        float m_Timer;           //経過時間.

        //ToDo : この2つで動きをコントロールする.
        //往復の速さ、値を大きくすると速く往復する.
        float m_StrafeFrequency;
        //移動する幅.
        float m_StrafeAmplitude; 

        //定数.
        static constexpr float STRAFE_RANGE         = 20.0f; //プレイヤーからの距離.
        static constexpr float APPROACH_SPEED       = 10.0f; //接近速度.
        static constexpr double MOVE_RUN_ANIM_SPEED = 50.0;
        static constexpr double STRAFE_ANIM_SPEED   = 12.0;

        //ここを大きくすることで、移動速度を上げても円の端まで届くようになります
        static constexpr float TRACKING_DELAY = 15.0f;
    };
}