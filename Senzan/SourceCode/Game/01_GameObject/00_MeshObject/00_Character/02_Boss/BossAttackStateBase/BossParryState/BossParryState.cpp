#include "BossParryState.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//BossIdolState//BossIdolState.h"

BossParryState::BossParryState(Boss* owner)
    : BossAttackStateBase(owner)
    , m_Phase(ParryPhase::None)
{
}

BossParryState::~BossParryState()
{
}

void BossParryState::Enter()
{
    m_currentTimer = 0.0f;

    // パリィ中は攻撃判定を止める
    m_pOwner->SetAttackColliderActive(false);

    m_pOwner->SetAnimSpeed(3.0);
    m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
    m_Phase = ParryPhase::Flinch;
}

void BossParryState::Update()
{
    switch (m_Phase)
    {
    case ParryPhase::Flinch:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
        {
            m_pOwner->SetAnimSpeed(2.5);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
            m_Phase = ParryPhase::FlinchTimer;
        }
        break;

    case ParryPhase::FlinchTimer:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
        {
            m_pOwner->SetAnimSpeed(5.0);
            m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
            m_Phase = ParryPhase::FlinchToIdol;
        }
        break;

    case ParryPhase::FlinchToIdol:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchToIdol))
        {
            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
        }
        break;

    default:
        break;
    }
}

void BossParryState::LateUpdate()
{
}

void BossParryState::Draw()
{
}

void BossParryState::Exit()
{
    // 念のため攻撃判定はOFFのまま
    m_pOwner->SetAttackColliderActive(false);
}
