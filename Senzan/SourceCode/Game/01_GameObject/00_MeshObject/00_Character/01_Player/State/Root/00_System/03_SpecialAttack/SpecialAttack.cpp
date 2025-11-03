#include "SpecialAttack.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
SpecialAttack::SpecialAttack(Player* owner)
	: System(owner)
{
}
SpecialAttack::~SpecialAttack()
{
}
// ID�̎擾.
constexpr PlayerState::eID SpecialAttack::GetStateID() const
{
	return PlayerState::eID::SpecialAttack;
}
void SpecialAttack::Enter()
{
}
void SpecialAttack::Update()
{
}
void SpecialAttack::LateUpdate()
{
}
void SpecialAttack::Draw()
{
}
void SpecialAttack::Exit()
{
}
} // PlayerState.
