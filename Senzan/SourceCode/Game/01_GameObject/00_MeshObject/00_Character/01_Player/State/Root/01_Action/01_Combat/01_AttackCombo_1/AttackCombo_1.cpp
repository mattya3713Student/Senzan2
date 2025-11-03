#include "AttackCombo_1.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
AttackCombo_1::AttackCombo_1(Player* owner)
	: Combat(owner)
	{
	}
	AttackCombo_1::~AttackCombo_1()
	{
	}
	
	// ID�̎擾.
	constexpr PlayerState::eID AttackCombo_1::GetStateID() const
	{
		return PlayerState::eID::AttackCombo_1;
	}

	void AttackCombo_1::Enter()
	{
	}
	void AttackCombo_1::Update()
	{
	}
	void AttackCombo_1::LateUpdate()
	{
	}
	void AttackCombo_1::Draw()
	{
	}
	void AttackCombo_1::Exit()
	{
	}
} // PlayerState.
