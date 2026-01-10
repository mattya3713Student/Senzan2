#include "Action.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/05_InputDevice/VirtualPad.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"

namespace PlayerState {
Action::Action(Player* owner)
	: PlayerStateBase(owner)
	, m_AnimSpeedChangedTrigger(std::make_unique<SingleTrigger>())
{
}

Action::~Action()
{
}

void Action::Enter()
{
}

void Action::Update()
{
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
}

void Action::LateUpdate()
{
    RotetToFront();
}

void Action::Draw()
{
}

void Action::Exit()
{
}
} // PlayerState.
