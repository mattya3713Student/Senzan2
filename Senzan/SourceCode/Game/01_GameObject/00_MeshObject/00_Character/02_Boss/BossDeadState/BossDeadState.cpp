#include "BossDeadState.h"

#include "00_MeshObject//00_Character//02_Boss//Boss.h"

static constexpr double AnimSpeed = 15.0;

BossDeadState::BossDeadState(Boss* owner)
	: StateBase<Boss>(owner)
{
}

BossDeadState::~BossDeadState()
{
}

//‘JˆÚ‚Éˆê‰ñ‚¾‚¯“ü‚éŠÖ”.
void BossDeadState::Enter()
{
	m_pOwner->SetIsLoop(false);
	m_pOwner->SetAnimSpeed(AnimSpeed);
	//€–SƒAƒjƒƒVƒ‡ƒ“‚ğÄ¶.
	m_pOwner->ChangeAnim(Boss::enBossAnim::Dead);

	if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Dead))
	{
		//ƒQ[ƒ€ƒNƒŠƒA‚Ö‘JˆÚ‚³‚¹‚é
		return;
	}
}

void BossDeadState::Update()
{
}

void BossDeadState::LateUpdate()
{
}

void BossDeadState::Draw()
{
}

void BossDeadState::Exit()
{
}
