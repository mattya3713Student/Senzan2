#include "JustDodge.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"
#include "../Dodge.h"

namespace PlayerState {
JustDodge::JustDodge(Player* owner)
	: Dodge(owner)
{
}
JustDodge::~JustDodge()
{
}

// ID�̎擾.
constexpr PlayerState::eID JustDodge::GetStateID() const
{
	return PlayerState::eID::JustDodge;
}

void JustDodge::Enter()
{
}
void JustDodge::Update()
{
}
void JustDodge::LateUpdate()
{
}
void JustDodge::Draw()
{
}
void JustDodge::Exit()
{
}

} // PlayerState.