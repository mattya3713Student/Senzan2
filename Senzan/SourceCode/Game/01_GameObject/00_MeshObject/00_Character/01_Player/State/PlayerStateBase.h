#pragma once

#include "System/Utility/StateMachine/StateBase.h"
#include "PlayerStateID.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/ActionData.h"

// TODO : 前方宣言でよい
//        が各ステートに書くのめんどくさいからinclude. 
class Player;

namespace PlayerState { 
class Root;
}


/**************************************************
* \tプレイヤーの状態の基底(Base).
*   StateBaseを継承してPlayer独自の関数を隠蔽する.
*\t担当:淵脇 未来.
**/
class PlayerStateBase : public StateBase<Player>
{
public:
    PlayerStateBase(Player* owner);

    // 取得系.
    constexpr virtual PlayerState::eID GetStateID() const = 0;    // ステートIDの取得.

    Player* GetPlayer() const;   // オーナー(プレイヤー)の取得.
    
    // タイムラインデータの設定.
    void SetActionDefinition(const ActionStateDefinition* def);
    void ResetActionTimeline();
    void UpdateActionTimeline(float dt);
 

protected:

    // 正面へラープ回転.
    void RotetToFront();

    // 目標の角度へラープ回転.
    void RotetToTarget(float TargetRote, float RotetionSpeed);
    const ActionStateDefinition* m_ActionDefinition = nullptr;
    float m_ActionElapsed = 0.0f;
         
private:

};
