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

	// UŒ‚ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½‚ç.
	if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Attack))
	{
		m_pOwner->ChangeState(PlayerState::eID::AttackCombo_0);
		return;
	}
}

void Action::LateUpdate()
{
}

void Action::Draw()
{
}

void Action::Exit()
{
}
} // PlayerState.
