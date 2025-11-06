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

#include "Game/03_Collision/Capsule/CapsuleCollider.h"

#include "Game/04_Time/Time.h"

Player::Player()    
	: Character         ()
	, m_RootState       (std::make_unique<PlayerState::Root>(this))
    , m_RunMoveSpeed    (1.f)
    , m_MoveVec         (0.0f, 0.0f)
{
    // ステートの初期化.
    InitializeStateRefMap();

    // メッシュのアタッチ.
    auto mesh = ResourceManager::GetSkinMesh("player_0");
    _ASSERT_EXPR(mesh != nullptr, "メッシュの取得に失敗");
    AttachMesh(mesh);
    //デバック確認のため.
    DirectX::XMFLOAT3 pos = { 10.05f, 0.05f, 20.05f };
    m_Transform->SetPosition(pos);


    DirectX::XMFLOAT3 scale = { 0.05f, 0.05f, 0.05f };
    m_Transform->SetScale(scale);

    //m_pCollider = std::make_shared<CapsuleCollider>(m_Transform);

}

Player::~Player()
{
}

void Player::Update()
{
	if (!m_RootState) {
		return;
	}
	m_RootState->Update();
}

void Player::LateUpdate()
{
    if (!m_RootState) {
        return;
    }

    m_RootState->LateUpdate();
}

void Player::Draw()
{
    Character::Draw();
}

// スタン中か.
bool Player::IsKnockBack() const noexcept
{
    return false;
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
void Player::ChangeState(PlayerState::eID id) const
{
    m_RootState.get()->ChangeState(id);
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

// Playerの最終的なDeltaTimeの取得.
float Player::GetDelta()
{
    return Time::GetDeltaTime() * m_TimeScale;
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