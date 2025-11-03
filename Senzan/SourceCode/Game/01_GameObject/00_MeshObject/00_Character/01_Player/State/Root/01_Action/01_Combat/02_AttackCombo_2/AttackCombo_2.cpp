#include "AttackCombo_2.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
AttackCombo_2::AttackCombo_2(Player* owner)
	: Combat(owner)
{
}
AttackCombo_2::~AttackCombo_2()
{
}

// ID�̎擾.
constexpr PlayerState::eID AttackCombo_2::GetStateID() const
{
	return PlayerState::eID::AttackCombo_2;
}

void AttackCombo_2::Enter()
{
}
void AttackCombo_2::Update()
{
}
void AttackCombo_2::LateUpdate()
{
}
void AttackCombo_2::Draw()
{
}
void AttackCombo_2::Exit()
{
}

} // PlayerState.
