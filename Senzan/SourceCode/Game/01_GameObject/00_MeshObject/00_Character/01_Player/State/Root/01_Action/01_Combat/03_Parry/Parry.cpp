#include "Parry.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
Parry::Parry(Player* owner)
	: Combat(owner)
{
}
Parry::~Parry()
{
}

// ID�̎擾.
constexpr PlayerState::eID Parry::GetStateID() const
{
	return PlayerState::eID::Parry;
}

void Parry::Enter()
{
}
void Parry::Update()
{
}
void Parry::LateUpdate()
{
}
void Parry::Draw()
{
}
void Parry::Exit()
{
}
} // PlayerState.
