#include "DodgeExecute.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"
#include "../Dodge.h"

namespace PlayerState {
DodgeExecute::DodgeExecute(Player* owner)
	: Dodge(owner)
{
}
DodgeExecute::~DodgeExecute()
{
}

// ID�̎擾.
constexpr PlayerState::eID DodgeExecute::GetStateID() const
{
	return PlayerState::eID::DodgeExecute;
}

void DodgeExecute::Enter()
{
}
void DodgeExecute::Update()
{
}
void DodgeExecute::LateUpdate()
{
}
void DodgeExecute::Draw()
{
}
void DodgeExecute::Exit()
{
}

} // PlayerState.