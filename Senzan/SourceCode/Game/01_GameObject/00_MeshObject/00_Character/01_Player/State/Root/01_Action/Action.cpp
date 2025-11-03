#include "Action.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
Action::Action(Player* owner)
	: PlayerStateBase(owner)
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
