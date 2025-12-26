#pragma once

#include "System/Utility/StateMachine/StateBase.h"
#include "BossStateID.h"

// 前方宣言
class Boss;

class BossStateBase : public StateBase<Boss>
{
public:
    // コンストラクタ：必ずBossのポインタを受け取る
    BossStateBase(Boss* pOwner);

    // デストラクタ（継承を前提とするため virtual）
    virtual ~BossStateBase() = default;

    // ボスの情報を取得（親クラスの m_pOwner を利用）
    Boss* GetBoss() const;

protected:
    // プレイヤーの方に常に向くようにする
    void RotateToPlayer(float RotationSpeed = 200.0f);

    // 目的の角度に向かって指定した速度で回転させる
    void RotateToTarget(float TargetRote, float RotationSpeed);
};