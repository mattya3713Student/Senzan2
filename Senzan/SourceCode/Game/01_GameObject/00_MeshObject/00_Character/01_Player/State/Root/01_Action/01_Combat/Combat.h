#pragma once
#include "../Action.h"

class Player;
class SingleTrigger;

/**************************************************
*	プレイヤーの攻撃の共通処理ステート(基底).
*   このステート自体がインスタンス化されることはない.
*	担当:淵脇 未来.
**/

namespace PlayerState 
{
    class Combat : public Action
    {
    public:
        Combat(Player* owner);
        ~Combat();

        virtual void Enter() override;
        virtual void Update() override;
        virtual void LateUpdate() override;
        virtual void Draw() override;
        virtual void Exit() override;

    protected:
        float               m_Distance; // 各移動距離.
        float               m_MaxTime;  // 各攻撃時間.
        float               m_currentTime;  // 各経過時間.
    };
}