#pragma once

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"
#include "System/Utility/Transform/Transform.h"

#include "System/Utility/StateMachine/StateMachine.h"
#include "Game\\01_GameObject\\00_MeshObject\\00_Character\\01_Player\\Player.h"

#include <string>

////ステートマシンクラスの前方宣言.
//template<typename FSM_Owner> class StateMachine;

//=====================================================================
// ボスクラスにプレイヤーの位置を入手させるためにここに前方宣言を書く.
// Update関数の中にプレイヤーのポジションを設定する.
//=====================================================================

class SlashState;        //斬撃ステートクラス.

class SlashCharge;       //チャージ斬撃クラス.
class ChargeSlashState;  //チャージ斬撃ステートクラス.

class Shout;             //叫びクラス.
class ShoutState;        //叫び攻撃ステートクラス.

//ボスの行動関係を書く.
class BossIdolState;        //待機状態.
class BossMoveState;        //左右移動動状態.
class BossAttackStateBase;  //攻撃ベースクラス.
class BossStompState;       //踏みつけ攻撃
class BossSlashState;       //斬る攻撃.
class BossChargeSlashState; //溜め攻撃.
class BossShoutState;       //叫び攻撃.
class BossJumpOnlState;
class BossLaserState;
class BossDeadState;

class BossThrowingState;

class BossChargeState;
class BossParryState;

class SkinMesh;

/***********************************************************************
*    ボスキャラクラス.
**/
class Boss
    : public Character
{
    friend BossIdolState;
    friend BossMoveState;
    friend BossStompState;
    friend BossSlashState;
    friend BossAttackStateBase;
    friend BossChargeSlashState;
    friend BossShoutState;
    friend BossJumpOnlState;
    friend BossLaserState;
    friend BossDeadState;
    friend BossChargeState;
    friend BossThrowingState;
    friend BossParryState;

    //ボスのアニメーションの列挙.
    enum class enBossAnim : byte
    {
        Idol = 0,        //待機.

        RunToIdol,       //走りから待機.
        Run,             //走り中.
        IdolToRun,       //待機から走り.

        Hit,             //被弾.

        ChargeToIdol,    //ため攻撃から待機.
        ChargeAttack,    //ため攻撃中.
        Charge,          //ためている.

        RightMove,       //右に進.
        LeftMove,        //左に進.

        SpecialToIdol,   //特殊攻撃と踏みつけ終了時に待機.
        Special_1,       //特殊攻撃と踏みつけ中.
        Special_0,       //飛ぶ.

        FlinchToIdol,    //怯みから待機.
        Flinch,          //怯み中.
        FlinchParis,     //怯み(パリィ).

        Dead,            //死亡.

        LaserEnd,        //レーザーから待機.
        Laser,           //レーザー中.
        LaserCharge,     //レーザーのため.

        SlashToIdol,     //斬る攻撃から待機.
        Slash,           //斬る攻撃.

        none,            //何もしない.
    };

public:
Boss();
~Boss() override;

void SetAnyAttackJustWindow(bool v) { m_IsAnyAttackJustWindow = v; }
bool IsAnyAttackJustWindow() const { return m_IsAnyAttackJustWindow; }

// パリィ被弾通知（外部から呼び出す）.
void OnParried();
// パリィ被弾フラグを取得.
bool IsParried() const { return m_IsParried; }

void Update() override;
    void LateUpdate() override;
    void Draw() override;

    void Init();

    //ステートクラスからStateMachineにアクセスする.
    StateMachine<Boss>* GetStateMachine();

    //アニメーション再生時に必要になるGet関数になっている.
    LPD3DXANIMATIONCONTROLLER GetAnimCtrl() const;

    void Hit();

    // 文字列でコライダーを操作できるようにする
    void SetColliderActiveByName(const std::string& name, bool active);

    /*************************************************************
    * @brief    エフェクトを指定位置に生成する.
    * @param[in]    effectName  ：エフェクトリソース名.
    * @param[in]    offset      ：ボス位置からのオフセット（デフォルト: 0,0,0）.
    * @param[in]    scale       ：エフェクトのスケール（デフォルト: 1.0f）.
    * ************************************************************/
    void SpawnEffect(const std::string& effectName,
                     const DirectX::XMFLOAT3& offset = DirectX::XMFLOAT3(0.f, 0.f, 0.f),
                     float scale = 1.0f);

public:
    //プレイヤーの位置を取得するためにここにSetPlayer()を作成する.
    void SetTargetPos(const DirectX::XMFLOAT3 Player_Pos);

    DirectX::XMFLOAT3 GetTargetPos() { return m_PlayerPos; }

protected:
    // 全攻撃判定オフ.
    void OffAttackCollider();

    // 衝突_被ダメージ.
    void HandleDamageDetection() override;
    // 衝突_攻撃判定.
    void HandleAttackDetection() override;
    // 衝突_回避.
    void HandleDodgeDetection() override;

    // 衝突_パリィ.
    void HandleParryDetection();

    //当たり判定を取得する.
    //通常攻撃.
    ColliderBase* GetSlashCollider() const;
    //ジャンプ攻撃.
    ColliderBase* GetStompCollider() const;
    //叫び攻撃.
    ColliderBase* GetShoutCollider() const;

    // 統合された攻撃コライダー（各ステートがボーンを指定して使用）
    ColliderBase* GetAttackCollider() const { return m_spAttackCollider; }

    // 攻撃コライダーが追従するボーンを設定
    void SetAttackBone(const std::string& boneName);
    // 攻撃コライダーのサイズを設定
    void SetAttackColliderSize(float radius, float height);
    // 名前で指定したコライダーのサイズを設定
    void SetColliderSizeByName(const std::string& name, float radius, float height);
    // 名前で指定したコライダーの与えるダメージを設定
    void SetColliderDamageByName(const std::string& name, float damage);
    // 攻撃コライダーのダメージを設定
    void SetAttackColliderDamage(float damage);

    /*************************************************************
    * @brief	ボスのワールド行列と掛け合わせてボーンのワールド行列を作成し、
    *           指定コライダーの位置オフセットと外部 Transform ポインタを更新.
    *           回転情報が必要ない場合は updateRotation=false を渡してください。
    *           rotationOffset を渡すとボーン回転へ追加の回転を適用できます。
    * @param[in]	boneName	：取得するボーン名.
    * @param[in]	collider	：更新対象のコライダー.
    * @param[in]	outTransform：ワールド Transform を格納するキャッシュ参照.
    * @param[in]	updateRotation：true の場合 outTransform の回転/スケールも更新する（デフォルト true）
    * @param[in]	rotationOffset：ボーン回転に乗算するクォータニオン回転オフセット（デフォルト: 単位クォータニオン）
    * @return	true = 成功, false = 取得失敗または引数不正
    * ************************************************************/
    bool UpdateColliderFromBone(
        const std::string& boneName,
        ColliderBase* collider,
        Transform& outTransform,
        bool updateRotation = true,
        const DirectX::XMFLOAT4& rotationOffset = DirectX::XMFLOAT4{0.0f,0.0f,0.0f,1.0f});

protected:
    //ステートマシンのメンバ変数.
    std::unique_ptr<StateMachine<Boss>> m_State;

    DirectX::XMFLOAT3               m_PlayerPos;
    DirectX::XMFLOAT3               m_PlayerVelocity;

    float m_MoveSpped = 0.0f;

    float m_TurnSpeed;
    float m_MoveSpeed;

    D3DXVECTOR3 m_vCurrentMoveVelocity;

    ColliderBase* m_pAttackCollider;     // 攻撃判定.

    float deleta_time;

    float m_HitPoint;

    //当たり判定のメンバ変数.
    ColliderBase* m_pSlashCollider;
    ColliderBase* m_pStompCollider;
    ColliderBase* m_pShoutCollider;

    // runtime flag indicating any attack's just window is active
    bool m_IsAnyAttackJustWindow = false;

    // パリィ被弾フラグ.
    bool m_IsParried = false;

    // 統合された攻撃コライダー（各ステートがボーンを指定）
    ColliderBase* m_spAttackCollider = nullptr;
    std::string m_AttackBoneName;  // 現在追従するボーン名
    LPD3DXFRAME m_pAttackBoneFrame = nullptr;  // ボーンフレームキャッシュ
    Transform m_AttackBoneWorldTransform;      // ワールドTransformキャッシュ

    // 現在再生中のエフェクトハンドル（-1 = none）
    int m_EffectHandle = -1;
};

