#pragma once

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"
#include "System/Singleton/BossAttackManager/BossAttackManager.h"

#include "System/Utility/StateMachine/StateMachine.h"
#include "Game\\01_GameObject\\00_MeshObject\\00_Character\01_Player\Player.h"

#include <vector>
#include <unordered_map>
#include <string>

class SkinMesh;

/***********************************************************************
*    ボスキャラクラス.
**/
class Boss
    : public Character
{
    friend class BossIdolState;
    friend class BossMoveState;
    friend class BossStompState;
    friend class BossSlashState;
    friend class BossChargeSlashState;
    friend class BossShoutState;
    friend class BossSpecialState;
    friend class BossLaserState;
    friend class BossDeadState;
    friend class BossChargeState;
    friend class BossThrowingState;

    // ボスのアニメーションの列挙.
    enum class enBossAnim : byte
    {
        Idol = 0,        // 待機.
        RunToIdol,       // 走りから待機.
        Run,             // 走り中.
        IdolToRun,       // 待機から走り.

        Hit,             // 被弾.

        ChargeToIdol,    // ため攻撃から待機.
        ChargeAttack,    // ため攻撃中.
        Charge,          // ためている.

        RightMove,       // 右に進.
        LeftMove,        // 左に進.

        SpecialToIdol,   // 特殊攻撃と踏みつけ終了時に待機.
        Special_1,       // 特殊攻撃と踏みつけ中.
        Special_0,       // 飛ぶ.

        FlinchToIdol,    // 怯みから待機.
        Flinch,          // 怯み中.
        FlinchParis,     // 怯み(パリィ).

        Dead,            // 死亡.

        LaserEnd,        // レーザーから待機.
        Laser,           // レーザー中.
        LaserCharge,     // レーザーのため.

        SlashToIdol,     // 斬る攻撃から待機.
        Slash,           // 斬る攻撃.

        none,            // 何もしない.
    };

public:
    enum class AttackType : Character::AttackTypeId
    {
        Special = 0,
        Throwing,
        Charge,
        Normal,
        Jump,
        Stomp,
        Laser,
        Shout,
        Generic = 255,
    };

    Boss();
    ~Boss() override;

    void Update() override;
    void LateUpdate() override;
    void Draw() override;

    void Init();

    // ステートマシンの初期化等.
    StateMachine<Boss>* GetStateMachine();
    LPD3DXANIMATIONCONTROLLER GetAnimCtrl() const;

    void Hit();
    void SetTargetPos(const DirectX::XMFLOAT3 Player_Pos);

    DirectX::XMFLOAT3 GetTargetPos() { return m_PlayerPos; }

    // 攻撃当たり判定のSetActive.
    void SetAttackCollidersActive(AttackType type, bool active);
    void SetAttackColliderActive(AttackType type, size_t index, bool active);
    void SetAttackColliderActive(bool active);

    // ボーン名をステートから設定する（ステートが追跡したいボーンを渡す）
    void SetSlashBoneName(const std::string& name);
    void SetStompBoneName(const std::string& name);

protected:

    // 衝突_被ダメージ.
    void HandleDamageDetection() override;
    // 衝突_攻撃判定.
    void HandleAttackDetection() override;
    // 衝突_回避.
    void HandleDodgeDetection() override;

    // 衝突_回避.
    void HandleParryDetection();

    // 叫び攻撃.
    ColliderBase* GetShoutCollider() const;
    // 通常攻撃 / ジャンプ 改め: 便宜上アクセス用関数を公開
    ColliderBase* GetSlashCollider() const;
    ColliderBase* GetStompCollider() const;
    // 通常攻撃(ボーンの位置設定).
    void UpdateSlashColliderTransform();
    // ジャンプ(ボーンの位置設定).
    void UpdateStompColliderTransform();
protected:
    // ステートマシンのメンバ変数.
    std::unique_ptr<StateMachine<Boss>> m_State;

    DirectX::XMFLOAT3            m_PlayerPos;
    DirectX::XMFLOAT3            m_PlayerVelocity;

    float m_MoveSped = 0.0f;

    float m_TurnSpeed;
    float m_MoveSpeed;

    D3DXVECTOR3 m_vCurrentMoveVelocity;

    std::unordered_map<Character::AttackTypeId, std::vector<ColliderSpec>> m_AttackColliderDefs;
    AttackType m_CurrentAttackType = AttackType::Normal;

    // ボーン行列のキャッシュ(生ポインタは危険なので値で管理).
    DirectX::XMMATRIX m_slashBoneMatrix = DirectX::XMMatrixIdentity();
    bool m_hasSlashBoneMatrix = false;
    std::string m_slashBoneName;

    DirectX::XMMATRIX m_stompBoneMatrix = DirectX::XMMatrixIdentity();
    bool m_hasStompBoneMatrix = false;
    std::string m_stompBoneName;
};

