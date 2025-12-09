#include "Movement.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

#include "Game/05_InputDevice/VirtualPad.h"    
#include "Game/04_Time/Time.h"   

namespace PlayerState {

// ----------------------------------------------------
// コンストラクタ
// ----------------------------------------------------
Movement::Movement(Player* owner)
	: Action(owner)
	, m_Collision_BodyHit(std::make_shared<CapsuleCollider>(owner->GetTransform()))
{
}

Movement::~Movement()
{
}

void Movement::Enter()
{
	Action::Enter();
}

void Movement::Update()
{
	if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Attack))
	{
		m_pOwner->ChangeState(PlayerState::eID::AttackCombo_0);
	}

	// 回避ボタンが押されたら.
	if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Dodge))
	{
		// ジャスト回避に派生.
		if (m_pOwner->m_IsJustDodgeTiming)
		{
			m_pOwner->ChangeState(PlayerState::eID::JustDodge);
		}
		// 回避に派生.	
		else
		{
			m_pOwner->ChangeState(PlayerState::eID::DodgeExecute);
		}
		return;
	}
	Action::Update();
}

void Movement::LateUpdate()
{
	Action::LateUpdate();

	RotetToFront();
}

void Movement::Draw()
{
	Action::Draw();
}

void Movement::Exit()
{
	Action::Exit();
}
} // namespace PlayerState