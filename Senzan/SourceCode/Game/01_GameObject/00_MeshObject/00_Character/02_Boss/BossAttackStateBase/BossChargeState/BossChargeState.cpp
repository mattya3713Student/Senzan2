#include "BossChargeState.h"

#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "Game//04_Time//Time.h"

BossChargeState::BossChargeState(Boss* owner)
	: BossAttackStateBase		(owner)
	, m_ChargeSlash				()
{
}

BossChargeState::~BossChargeState()
{
}

void BossChargeState::Enter()
{
	// “–‚½‚è”»’è‚ð—LŒø‰».
	m_pOwner->SetAttackColliderActive(true);
	m_pOwner->SetAnimSpeed(25.0);
	m_pOwner->ChangeAnim(Boss::enBossAnim::Charge);
}

void BossChargeState::Update()
{
	if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Charge))
	{
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossChargeSlashState>(m_pOwner));
	}
}

void BossChargeState::LateUpdate()
{
}

void BossChargeState::Draw()
{
}

void BossChargeState::Exit()
{	// “–‚½‚è”»’è‚ð–³Œø‰».
	m_pOwner->SetAttackColliderActive(false);
}

void BossChargeState::BossAttack()
{
}
