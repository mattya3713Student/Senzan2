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
}

void BossChargeState::Update()
{
	//‚±‚±‚ÅdeltaTime‚ÌŽžŠÔ‚ÌŒv‘ª‚ð‚·‚é.
	float deltaTime = Time::GetInstance().GetDeltaTime();
	//Œo‰ßŽžŠÔ‚Ì‰ÁŽZ.
	m_Timer += deltaTime;
	if (!ChargeFlag && m_Timer >= m_TransitionTimer)
	{
		m_pOwner->ChangeAnim(6);
		ChargeFlag = true;
	}
	if (ChargeFlag && m_Timer >= m_TransitionTimer + m_pOwner->GetAnimPeriod(6))
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
{
}

void BossChargeState::BossAttack()
{
}
