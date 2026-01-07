#include "Player.h"
#include "System/Utility/StateMachine/StateMachine.h"

#include <filesystem>

#include "System/Utility/FileManager/FileManager.h"

#include "State/PlayerStateID.h"
#include "State/PlayerStateBase.h"

#include "State/Root/Root.h"

#include "State/Root/00_System/System.h"
#include "State/Root/00_System/00_Pause/Pause.h"
#include "State/Root/00_System/01_KnockBack/KnockBack.h"
#include "State/Root/00_System/02_Dead/Dead.h"
#include "State/Root/00_System/03_SpecialAttack/SpecialAttack.h"

#include "State/Root/01_Action/Action.h"
#include "State/Root/01_Action/00_Movement/Movement.h"
#include "State/Root/01_Action/00_Movement/00_Idle/Idle.h"
#include "State/Root/01_Action/00_Movement/01_Run/Run.h"

#include "State/Root/01_Action/01_Combat/Combat.h"
#include "State/Root/01_Action/01_Combat/00_AttackCombo_0/AttackCombo_0.h"
#include "State/Root/01_Action/01_Combat/01_AttackCombo_1/AttackCombo_1.h"
#include "State/Root/01_Action/01_Combat/02_AttackCombo_2/AttackCombo_2.h"

#include "State/Root/01_Action/02_Dodge/Dodge.h"
#include "State/Root/01_Action/02_Dodge/00_DodgeExecute/DodgeExecute.h"
#include "State/Root/01_Action/02_Dodge/01_JustDodge/JustDodge.h"

#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"

#include "Game/04_Time/Time.h"

#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/CameraManager/CameraManager.h"

// Include for initial repeat animation state (visual only)
#include "Game/01_GameObject/00_MeshObject/00_Character/RepeatAnimationState.h"

Player::Player()
	: Character		()
	, m_RootState		( std::make_unique<PlayerState::Root>(this) )
    , m_StateRefMap     ( )
    , m_NextStateID     ( PlayerState::eID::None )
    , m_IsStateChangeRequest    ( false )
    , m_MoveVec         ( { 0.f,0.f,0.f } )
    , m_Combo           ( 0 )
    , m_CurrentUltValue ( 0.0f )
    , m_MaxUltValue     ( 10000.0f )
    , m_IsKnockBack     ( false )
    , m_KnockBackVec    ( { 0.f,0.f,0.f } )
    , m_KnockBackPower  ( 0.f )
    , m_IsDead          ( false )
    , m_RunMoveSpeed    ( 50.f )
    , m_IsJustDodgeTiming( false )
    , m_TargetPos        ( { 0.f, 0.f, 0.f } )
{
    // ステートの初期化.
    InitializeStateRefMap();

    // メッシュのアタッチ.
    auto mesh = ResourceManager::GetSkinMesh("player");
    _ASSERT_EXPR(mesh != nullptr, "メッシュの取得に失敗");
    AttachMesh(mesh);

    //デバック確認のため.
    DirectX::XMFLOAT3 pos = { 0.f, 0.f, -20.f };
    m_spTransform->SetPosition(pos);

    DirectX::XMFLOAT3 scale = { 3.f, 3.f, 3.f };
    m_spTransform->SetScale(scale);

    m_MaxHP = 100.f;
    m_HP = m_MaxHP;

    CollisionDetector::GetInstance().RegisterCollider(*m_upColliders);

    // 各ステートの初期化.
    // NOTE: For now we disable running the full state machine and only use the visual initial repeat animation.
#if 0
    // Normally we would enter the root state here to enable gameplay states.
    m_RootState.get()->Enter();
#endif

    // 初期状態をリピートアニメに設定 (visual only)
    m_pInitialRepeatState = std::make_unique<RepeatAnimationState<Player>>(this, "Idle", static_cast<int>(Player::eAnim::Idle), 1.0f);
    m_pInitialRepeatState->Enter();
}

Player::~Player()
{
    CollisionDetector::GetInstance().UnregisterCollider(*m_upColliders);
}

ActionStateDefinition* Player::GetActionDefinition(const std::string& stateName)
{
    auto it = m_ActionDefinitions.find(stateName);
    if (it != m_ActionDefinitions.end()) return &it->second;

    std::filesystem::path path = "ActionTimeline_" + stateName + ".json";
    if (!std::filesystem::exists(path))
    {
        return nullptr;
    }

    auto j = FileManager::JsonLoad(path);
    if (j.is_null() || !j.is_object()) return nullptr;

    ActionStateDefinition def = j.get<ActionStateDefinition>();
    if (def.stateName.empty()) def.stateName = stateName;
    auto& stored = m_ActionDefinitions[stateName];
    stored = std::move(def);
    return &stored;
}

int Player::ResolveAnimIndex(const std::string& animName, int defaultIndex) const
{
    std::string lower = animName;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "attack_0") return static_cast<int>(Player::eAnim::Attack_0);
    if (lower == "attack_1") return static_cast<int>(Player::eAnim::Attack_1);
    if (lower == "attack_2") return static_cast<int>(Player::eAnim::Attack_2);
    if (lower == "idle") return static_cast<int>(Player::eAnim::Idle);
    if (lower == "run") return static_cast<int>(Player::eAnim::Run);
    if (lower == "parry") return static_cast<int>(Player::eAnim::Parry);
    if (lower == "dodge") return static_cast<int>(Player::eAnim::Dodge);
    if (lower == "knockback") return static_cast<int>(Player::eAnim::KnockBack);
    if (lower == "dead") return static_cast<int>(Player::eAnim::Dead);
    if (lower == "specialattack_0") return static_cast<int>(Player::eAnim::SpecialAttack_0);
    if (lower == "specialattack_1") return static_cast<int>(Player::eAnim::SpecialAttack_1);
    if (lower == "specialattack_2") return static_cast<int>(Player::eAnim::SpecialAttack_2);
    return defaultIndex;
}

void Player::ApplyActionDefinition(const ActionStateDefinition& def)
{
    // アニメ設定
    int animIndex = ResolveAnimIndex(def.animationName, static_cast<int>(Player::eAnim::Attack_0));
    SetIsLoop(false);
    SetAnimTime(0.0);
    ChangeAnim(animIndex);

    // 既存スケジュールをクリア
    m_ScheduledColliders.clear();

    // コライダー定義を再構築
    std::unordered_map<Character::AttackTypeId, std::vector<ColliderSpec>> defs;
    for (const auto& evt : def.colliderEvents)
    {
        ColliderSpec spec = evt.spec;
        defs[evt.type].push_back(spec);
    }
    if (!defs.empty())
    {
        CreateCollidersFromDefs(defs);
    }

    // スケジューリング
    for (const auto& evt : def.colliderEvents)
    {
        ScheduleCollider(evt.type, evt.index, evt.startTime, evt.duration);
    }
}

void Player::Update()
{
    Character::Update();

    m_IsSuccessParry = false;

    // For debugging: only run the visual initial repeat state and skip the normal state machine.
    if (m_pInitialRepeatState)
    {
        m_pInitialRepeatState->Update();
        return;
    }

#if 0
    // Original state-machine flow (disabled for now). Re-enable by changing #if 0 to #if 1 or removing the block.
    // 初期リピートアニメを優先的に再生（ステート遷移要求が来たら FSM に移行）
    if (m_pInitialRepeatState && m_NextStateID == PlayerState::eID::None)
    {
        m_pInitialRepeatState->Update();
        return;
    }
    m_pInitialRepeatState.reset();

    // ステート遷移のチェック.
    if (m_NextStateID != PlayerState::eID::None)
    {
        m_RootState->ChangeState(m_NextStateID);
        m_NextStateID = PlayerState::eID::None;
    }
    if (!m_RootState) {
        return;
    }
    m_RootState->Update();
#endif

    m_IsJustDodgeTiming = false;
}

void Player::LateUpdate()
{
    Character::LateUpdate();

#if 0
    if (!m_RootState) {
        return;
    }

    // ステートマシーンの最終更新を実行.
    m_RootState->LateUpdate();

    // 押し戻し.
    HandleCollisionResponse();

    // 衝突イベント処理を実行
    HandleParryDetection();
    HandleDamageDetection();
    HandleAttackDetection();
    HandleDodgeDetection();
#endif
}

void Player::Draw()
{
    // モデルの関係で前後反転.
    m_spTransform->SetRotationY(GetRotation().y + D3DXToRadian(180.0f));

    Character::Draw();

    m_spTransform->SetRotationY(GetRotation().y - D3DXToRadian(180.0f));
}

// ノック中か.
bool Player::IsKnockBack() const noexcept
{
    return m_IsKnockBack;
}

// 死亡中か.
bool Player::IsDead() const noexcept
{
    return m_IsDead;
}

bool Player::IsParry() const noexcept
{
    return m_IsSuccessParry;
}

// ポーズ中か.
bool Player::IsPaused() const noexcept
{
    return false;
}

// ステートの変更.
void Player::ChangeState(PlayerState::eID id)
{
#if 0
    m_NextStateID = id;
    //m_RootState.get()->ChangeState(id);
#else
    // Disabled while debugging with only visual repeat state active.
    (void)id;
#endif
}

std::reference_wrapper<PlayerStateBase> Player::GetStateReference(PlayerState::eID id)
{
    try
    {
        auto it = m_StateRefMap.find(id);

        if (it == m_StateRefMap.end())
        {
            throw std::out_of_range("PlayerStateID not mapped (ID: " + std::to_string(static_cast<int>(id)) + ")");
        }

        return it->second();
    }
    catch (const std::exception& e)
    {
        std::string message = "Player::GetStateReference failed (ID: " + std::to_string(static_cast<int>(id)) + "): " + e.what();
        //Debug::Warning(message.c_str());
        throw;
    }
}

// マッピングを初期化するヘルパー関数.
void Player::InitializeStateRefMap()
{
    // --- Systemステート ---.
    m_StateRefMap[PlayerState::eID::Pause]          = [this] { return m_RootState->GetPauseStateRef(); };
    m_StateRefMap[PlayerState::eID::KnockBack]      = [this] { return m_RootState->GetKnockBackStateRef(); };
    m_StateRefMap[PlayerState::eID::Dead]           = [this] { return m_RootState->GetDeadStateRef(); };
    m_StateRefMap[PlayerState::eID::SpecialAttack]  = [this] { return m_RootState->GetSpecialAttackStateRef(); };

    // --- Movementステート ---.
    m_StateRefMap[PlayerState::eID::Idle]           = [this] { return m_RootState->GetIdleStateRef(); };
    m_StateRefMap[PlayerState::eID::Run]            = [this] { return m_RootState->GetRunStateRef(); };

    // --- Dodgeステート ---.
    m_StateRefMap[PlayerState::eID::DodgeExecute]   = [this] { return m_RootState->GetDodgeExecuteStateRef(); };
    m_StateRefMap[PlayerState::eID::JustDodge]      = [this] { return m_RootState->GetJustDodgeStateRef(); };

    // --- Combatステート ---.
    m_StateRefMap[PlayerState::eID::AttackCombo_0]  = [this] { return m_RootState->GetCombo0StateRef(); };
    m_StateRefMap[PlayerState::eID::AttackCombo_1]  = [this] { return m_RootState->GetCombo1StateRef(); };
    m_StateRefMap[PlayerState::eID::AttackCombo_2]  = [this] { return m_RootState->GetCombo2StateRef(); };
    m_StateRefMap[PlayerState::eID::Parry]          = [this] { return m_RootState->GetParryStateRef(); };
}

// 衝突_被ダメージ.
void Player::HandleDamageDetection()
{
#if 0
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Player_Damage) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Enemy_Attack) != eCollisionGroup::None)
            {
                // 既にスタン中や無敵時間であれば処理を中断
                if (IsKnockBack() || IsDead() || IsDead()) { continue; }

                m_Combo = 0;

                // ダメージを適用 
                ApplyDamage(info.AttackAmount);

                m_KnockBackVec = info.Normal;
                m_KnockBackPower = 100.f;

                // 状態をノックバックに遷移させる
                ChangeState(PlayerState::eID::KnockBack);

			CameraManager::GetInstance().ShakeCamera(0.5f, 4.5f); // カメラを少し揺らす.

                // 1フレームに1回.
                return;
            }
        }
    }
#endif
}

void Player::HandleAttackDetection()
{
#if 0
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Player_Attack) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Enemy_Damage) != eCollisionGroup::None)
            {
                ++m_Combo;
                m_CurrentUltValue += static_cast<float>(m_Combo) * 0.0f;
                SetColliderActive(static_cast<Character::AttackTypeId>(Player::AttackType::Normal), 0, false);

                // 一フレーム1回.
                return;
            }
        }
    }
#endif
}

void Player::HandleDodgeDetection()
{
#if 0
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Player_JustDodge) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            // MEMO : EnemyAttackに触れたとき.
            m_IsJustDodgeTiming = true;;
            m_CurrentUltValue += static_cast<float>(m_Combo) * 0.0f;
        }
    }
#endif
}

void Player::HandleParryDetection()
{
#if 0
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Player_Parry) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Enemy_Attack) != eCollisionGroup::None)
            {
                SoundManager::GetInstance().Play("Parry");
                m_IsSuccessParry = true;
                
                m_CurrentUltValue += static_cast<float>(m_Combo) * 0.0f;
                // 一フレーム1回.
                return;
            }
        }
    }
#endif
}
