#include "BossDeadState.h"

#include "00_MeshObject//00_Character//02_Boss//Boss.h"

static constexpr double AnimSpeed = 0.03;

BossDeadState::BossDeadState(Boss* owner)
	: StateBase<Boss>(owner)
{
}

BossDeadState::~BossDeadState()
{
}

void BossDeadState::Enter()
{
	m_pOwner->SetIsLoop(true);
	m_pOwner->SetAnimSpeed(AnimSpeed);
	//Ž€–SƒAƒjƒƒVƒ‡ƒ“‚ðÄ¶.
	m_pOwner->ChangeAnim(Boss::enBossAnim::Dead);
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
