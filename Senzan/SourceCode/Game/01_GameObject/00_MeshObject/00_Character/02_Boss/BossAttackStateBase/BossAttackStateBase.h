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

    //攻撃を実行させる関数.
    virtual void BossAttack() {};

    virtual void ParryTime() {};

protected:
    //メンバ変数を作成.
    //攻撃開始からの経過時間取得
    float m_Attacktime;
    //ダメージを一度与えたかどうかのフラグ.
    //bool m_isDamageDealt; 
    //パリィー可能かどうか.
    //bool m_canBeParried;       
    // 

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
    float m_Knockback = 0.0f;
    float m_ChargeTime = 0.0f;
    int   m_AttackPriority = 0;

    float m_AttackDuration = 0.0f;
    float m_Cooldown = 0.0f;
    float m_Timer = 0.0f;                // 共通タイマー
    float m_TransitionTimer = 0.0f;      // 共通トランジション用タイマー
    bool  m_IsAttacking = false;
    bool  m_HasHitTarget = false;
    bool  m_IsInterruptible = true;

    std::shared_ptr<ColliderBase> m_pHitCollider;
    std::vector<GameObject*>      m_HitTargets;

    DirectX::XMFLOAT3 m_TargetDir{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 m_MotionVelocity{0.0f, 0.0f, 0.0f};

    DirectX::XMFLOAT3 m_StartPos{0.0f, 0.0f, 0.0f}; // 攻撃開始位置（共通）

    GameObject* m_pTarget = nullptr;

    std::string m_SoundId;
    std::string m_VFXId;

    bool m_ShowDebugGizmos = false;

};
