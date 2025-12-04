#include "AttackCombo_0.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
AttackCombo_0::AttackCombo_0(Player* owner)
	: Combat(owner)
{
}
AttackCombo_0::~AttackCombo_0()
{
}

// ID�̎擾.
constexpr PlayerState::eID AttackCombo_0::GetStateID() const
{
	return PlayerState::eID::AttackCombo_0;
}

void AttackCombo_0::Enter()
{
}
void AttackCombo_0::Update()
{

	m_pOwner->ChangeState(PlayerState::eID::Idle);
}
void AttackCombo_0::LateUpdate()
{
}
void AttackCombo_0::Draw()
{
}
void AttackCombo_0::Exit()
{
}

} // PlayerState.