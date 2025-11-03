#include "KnockBack.h"

namespace PlayerState {
KnockBack::KnockBack(Player* owner)
	: System(owner)
{
}
	KnockBack::~KnockBack()
{
}

	// ID�̎擾.
	constexpr PlayerState::eID KnockBack::GetStateID() const
	{
		return PlayerState::eID::KnockBack;
	}

void KnockBack::Enter()
{
}
void KnockBack::Update()
{
}
void KnockBack::LateUpdate()
{
}
void KnockBack::Draw()
{
}
void KnockBack::Exit()
{
}
} // PlayerState.
