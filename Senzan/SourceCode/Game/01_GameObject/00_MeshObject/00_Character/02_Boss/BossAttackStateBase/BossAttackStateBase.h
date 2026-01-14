#pragma once

#include "System//Utility//StateMachine//StateBase.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"

#include <memory>
#include <string>
#include <vector>

/*****************************************************************
*    ボスの攻撃ベースクラス(基底クラス).
**/

//前方宣言.
class Boss;
class Time;
class ColliderBase;
class GameObject;
class Animator;

// 当たり判定の発生時間を制御する
struct ColliderWindow
{
    std::string BoneName;
    float Start = 0.0f;    // 開始秒
    float Duration = 0.1f; // 持続秒
    bool IsAct = false;    // 内部フラグ（判定開始済みか）
    bool IsEnd = false;    // 内部フラグ（判定終了済みか）

    void Reset() { IsAct = false; IsEnd = false; }
};

// 攻撃中の移動（踏み込み）を制御する
struct MovementWindow
{
    float Start = 0.0f;
    float Duration = 0.2f;
    float Speed = 10.0f;   // 移動速度
    bool IsAct = false;

    void Reset() { IsAct = false; }
};

class BossAttackStateBase
    : public StateBase<Boss>

{
public:
    BossAttackStateBase(Boss* owner);
    virtual ~BossAttackStateBase() = default;

    //最初に入る.
    virtual void Enter() override {};
    //動作.
    virtual void Update() override {};
    //かかなくていい.
    virtual void LateUpdate() override {};
    //描画.
    virtual void Draw() override {};
    //終わるときに一回だけ入る.
    virtual void Exit() override {};

    // 攻撃ステート調整用UI（必要なステートで呼ぶ）
    virtual void DrawImGui();

    //攻撃を実行させる関数.
    virtual void BossAttack() {};

protected:
    // 共通タイムベース制御
    float m_StateTimer = 0.0f; // ステート開始からの経過時間(秒)
    float m_AnimSpeed = 1.0f;  // アニメーション再生速度
    float m_ExitTime = 2.0f;   // ステート終了時間(秒)

    std::vector<ColliderWindow>  m_ColliderWindows;
    std::vector<MovementWindow>  m_MovementWindows;

    void UpdateBaseLogic(float dt); // 共通更新ロジック

protected:
    //メンバ変数を作成.
    //攻撃開始からの経過時間取得
    float m_Attacktime;

    //アニメーションに必要なメンバ変数を設定している.
    DirectX::XMFLOAT3            m_BonePos;  // ボーン座標.

    LPD3DXANIMATIONCONTROLLER   AnimCtrl;

    std::shared_ptr<Transform> m_pTransform;

    float m_currentTimer;
    static constexpr float m_currentAnimSpeed = 0.001f;

    // --- 共通攻撃パラメータ (基底に持たせる候補) ---
    float m_Damage = 0.0f;
    float m_AttackRange = 0.0f;
    float m_AttackRadius = 0.0f;
    float m_AttackHeight = 0.0f; // 追加: コライダーの高さ
    float m_ChargeTime = 0.0f;

    // per-phase animation speeds (溜め, 攻撃中, 余波)
    float m_AnimSpeedCharge = 1.0f;
    float m_AnimSpeedAttack = 1.0f;
    float m_AnimSpeedExit = 1.0f;

    float m_AttackDuration = 0.0f;
    float m_Cooldown = 0.0f;
    float m_Timer = 0.0f;                // 共通タイマー
    float m_TransitionTimer = 0.0f;      // 共通トランジション用タイマー
    bool  m_IsAttacking = false;
    bool  m_HasHitTarget = false;
    bool  m_IsInterruptible = true;

    std::shared_ptr<ColliderBase> m_pHitCollider;
    std::vector<GameObject*>      m_HitTargets;

    DirectX::XMFLOAT3 m_TargetDir{ 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_MotionVelocity{ 0.0f, 0.0f, 0.0f };

    DirectX::XMFLOAT3 m_StartPos{ 0.0f, 0.0f, 0.0f }; // 攻撃開始位置（共通）
};
