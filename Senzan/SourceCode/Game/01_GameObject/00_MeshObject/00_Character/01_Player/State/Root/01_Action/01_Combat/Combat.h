#pragma once
#include "../Action.h"

class Player;

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

    private:
    };
}