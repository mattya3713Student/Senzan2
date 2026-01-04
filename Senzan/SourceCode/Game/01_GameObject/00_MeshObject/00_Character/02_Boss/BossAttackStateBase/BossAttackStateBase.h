#pragma once

#include "System//Utility//StateMachine//StateBase.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"
#include <string>

/*****************************************************************
*    ボスの攻撃ベースクラス(基底クラス).
**/

//前方宣言.
class Boss;
class Time;

class BossAttackStateBase
    : public StateBase<Boss>

{
public:
    BossAttackStateBase(Boss* owner);
    virtual ~BossAttackStateBase() = default;

    // 最初に入る処理.
    virtual void Enter() override {};
    // 更新処理.
    virtual void Update() override {};
    // 後処理.
    virtual void LateUpdate() override {};
    // 描画処理.
    virtual void Draw() override {};
    // 終了時に一度だけ呼ばれる処理.
    virtual void Exit() override {};

    // 攻撃実行関数.
    virtual void BossAttack() {};

    // パリィ.
    virtual void ParryTime() {};

    // 斬撃用のコライダー配置に使うボーンを追跡する.
    void TrackSlashBone(const std::string& boneName);
    void UntrackSlashBone();

    // 踏みつけ用のコライダー配置に使うボーンを追跡する.
    void TrackStompBone(const std::string& boneName);
    void UntrackStompBone();

    // 特定の攻撃タイプのコライダーActive設定.
    void EnableAttackColliders(int attackTypeId, bool active);
    // 攻撃タイプのコライダーActive設定.
    void EnableCurrentAttackColliders(bool active);

protected:
    // 攻撃開始からの経過時間.
    float m_Attacktime;

    // アニメーションで使用するメンバ変数.
    DirectX::XMFLOAT3           m_BonePos;    // ボーン座標.

    LPD3DXANIMATIONCONTROLLER   m_AnimCtrl;

    std::shared_ptr<Transform>  m_spTransform;

    float m_currentTimer;
    static constexpr float m_currentAnimSpeed = 0.001f;

    // 派生クラスで情報参照やデバッグ表示に使える追跡中のボーン名
    std::string m_trackedSlashBoneName;
    std::string m_trackedStompBoneName;

};
