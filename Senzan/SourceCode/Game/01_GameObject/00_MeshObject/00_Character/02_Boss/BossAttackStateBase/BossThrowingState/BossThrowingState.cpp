#include "BossThrowingState.h"

BossThrowingState::BossThrowingState(Boss* owner)
	: BossAttackStateBase	(owner)
	, m_Timer				(0.0f)
	, m_TransitionTimer		(60.0f)
{
}

BossThrowingState::~BossThrowingState()
{
}

void BossThrowingState::Enter()
{
}

void BossThrowingState::Update()
{
}

void BossThrowingState::LateUpdate()
{
}

void BossThrowingState::Draw()
{
}

void BossThrowingState::Exit()
{
}
