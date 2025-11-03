#include "Dead.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
Dead::Dead(Player* owner)
	: System(owner)
{
}
Dead::~Dead()
{
}

// ID�̎擾.
constexpr PlayerState::eID Dead::GetStateID() const
{
	return PlayerState::eID::Dead;
}

void Dead::Enter()
{
}
void Dead::Update()
{
}
void Dead::LateUpdate()
{
}
void Dead::Draw()
{
}
void Dead::Exit()
{
}
} // PlayerState.
