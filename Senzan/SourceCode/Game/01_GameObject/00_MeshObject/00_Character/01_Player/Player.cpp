#include "Player.h"
#include "System/Utility/StateMachine/StateMachine.h"	

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

Player::Player()    
	: Character         ()
	, m_RootState       ( std::make_unique<PlayerState::Root>(this) )
    , m_StateRefMap     ( )
    , m_NextStateID     ( PlayerState::eID::None )
    , m_IsStateChangeRequest    ( false )
    , m_MoveVec         ( { 0.f,0.f,0.f } )
    , m_IsKnockBack     ( false )
    , m_KnockBackVec    ( { 0.f,0.f,0.f } )
    , m_KnockBackPower  ( 0.f )
    , m_RunMoveSpeed    ( 1.f )
    , m_IsJustDodgeTiming( false )
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

    // 被ダメの追加.
    std::unique_ptr<CapsuleCollider> damage_collider = std::make_unique<CapsuleCollider>(m_spTransform);

    damage_collider->SetColor(Color::eColor::Yellow);
    damage_collider->SetHeight(2.0f);
    damage_collider->SetRadius(0.5f);
    damage_collider->SetPositionOffset(0.f, 1.5f, 0.f);
    damage_collider->SetMyMask(eCollisionGroup::Player_Damage);
    damage_collider->SetTarGetTargetMask(eCollisionGroup::Enemy_Attack);

    m_upColliders->AddCollider(std::move(damage_collider));

    // ジャスト回避の追加.
    std::unique_ptr<CapsuleCollider> justdodge_collider = std::make_unique<CapsuleCollider>(m_spTransform);

    justdodge_collider->SetColor(Color::eColor::Gray);
    justdodge_collider->SetHeight(45.0f);
    justdodge_collider->SetRadius(45.0f);
    justdodge_collider->SetPositionOffset(0.f, 1.5f, 0.f);
    justdodge_collider->SetMyMask(eCollisionGroup::Player_JustDodge);
    justdodge_collider->SetTarGetTargetMask(eCollisionGroup::Enemy_Attack);

    m_upColliders->AddCollider(std::move(justdodge_collider));

    CollisionDetector::GetInstance().RegisterCollider(*m_upColliders);

    // 各ステートの初期化.
    m_RootState.get()->Enter();
}

Player::~Player()
{
    CollisionDetector::GetInstance().UnregisterCollider(*m_upColliders);
}

void Player::Update()
{
    Character::Update();

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

    m_IsJustDodgeTiming = false;
}

void Player::LateUpdate()
{
    Character::LateUpdate();

    if (!m_RootState) {
        return;
    }

    // ステートマシーンの最終更新を実行.
    m_RootState->LateUpdate();

    // 押し戻し.
    HandleCollisionResponse();

    // 衝突イベント処理を実行
    HandleDamageDetection();
    HandleAttackDetection();
    HandleDodgeDetection();

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
    return false;
}

// ポーズ中か.
bool Player::IsPaused() const noexcept
{
    return false;
}

// ステートの変更.
void Player::ChangeState(PlayerState::eID id)
{
    m_NextStateID = id;
    //m_RootState.get()->ChangeState(id);
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
                if (IsKnockBack() || IsDead()) { continue; }

                // ダメージを適用 
                // ApplyDamage(info.DamageAmount);

                m_KnockBackVec = info.Normal;
                m_KnockBackPower = 10.f;

                // 状態をノックバックに遷移させる
                ChangeState(PlayerState::eID::KnockBack);

				CameraManager::GetInstance().ShakeCamera(2.5f, 4.5f); // カメラを少し揺らす.

                // 1フレームに1回.
                return;
            }
        }
    }
}

void Player::HandleAttackDetection()
{
}

void Player::HandleDodgeDetection()
{
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
            m_IsJustDodgeTiming = true;
        }
    }
}
