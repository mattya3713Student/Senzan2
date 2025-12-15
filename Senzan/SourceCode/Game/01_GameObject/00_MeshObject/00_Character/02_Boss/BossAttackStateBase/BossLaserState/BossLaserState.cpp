#include "BossLaserState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"


#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"


BossLaserState::BossLaserState(Boss* owner)
	: BossAttackStateBase   (owner)
    , m_pBossIdol           ()

    , m_AnimChange            (enAnimChange::none)
{
}

BossLaserState::~BossLaserState()
{
}

void BossLaserState::Enter()
{
    m_pOwner->SetAnimSpeed(0.01);
    m_pOwner->ChangeAnim(Boss::enBossAnim::LaserCharge);
    m_AnimChange = enAnimChange::Charge;
}

void BossLaserState::Update()
{
    switch (m_AnimChange)
    {
    case BossLaserState::enAnimChange::Charge:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserCharge))
        {
            m_pOwner->ChangeAnim(Boss::enBossAnim::Laser);
            m_AnimChange = enAnimChange::Attack;
        }
        break;
    case BossLaserState::enAnimChange::Attack:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Laser))
        {
            m_pOwner->ChangeAnim(Boss::enBossAnim::LaserEnd);
            m_AnimChange = enAnimChange::ChargeEnd;
        }
        break;
    case BossLaserState::enAnimChange::ChargeEnd:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserEnd))
        {
            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
        }
        break;
    case BossLaserState::enAnimChange::none:
        break;
    default:
        break;
    }
}

void BossLaserState::LateUpdate()
{
}

void BossLaserState::Draw()
{
}

void BossLaserState::Exit()
{
}

void BossLaserState::BossAttack()
{
}
