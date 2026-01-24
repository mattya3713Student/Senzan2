#include "BossParryState.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//BossIdolState//BossIdolState.h"
#include "Game/04_Time/Time.h"

BossParryState::BossParryState(Boss* owner)
    : BossAttackStateBase(owner)
    , m_Phase(ParryPhase::None)
    , m_WithDelay(false)
    , m_DelaySeconds(0.0f)
    , m_DelayElapsed(0.0f)
{
}

BossParryState::BossParryState(Boss* owner, bool withDelay, float delaySeconds)
    : BossAttackStateBase(owner)
    , m_Phase(ParryPhase::None)
    , m_WithDelay(withDelay)
    , m_DelaySeconds(delaySeconds)
    , m_DelayElapsed(0.0f)
{
}

BossParryState::~BossParryState()
{
}

void BossParryState::Enter()
{
    m_CurrentTime = 0.0f;
    if (m_WithDelay)
    {
        // Suc の場合: 指定の順番で再生 (bottom-up): FlinchToIdol -> Flinch -> FlinchParis
        m_pOwner->SetAnimSpeed(3.0f);
        m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
        m_Phase = ParryPhase::PlayFlinchToIdol;
    }
    else
    {
        // Fai の場合: Hit を再生して終了したら Idol へ
        m_pOwner->SetAnimSpeed(2.0f);
        m_pOwner->ChangeAnim(Boss::enBossAnim::Hit);
        m_Phase = ParryPhase::PlayHit;
    }
}

void BossParryState::Update()
{
    switch (m_Phase)
    {
    case ParryPhase::PlayFlinchToIdol:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchToIdol))
        {
            m_pOwner->SetAnimSpeed(2.5f);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
            m_Phase = ParryPhase::PlayFlinch;
        }
        break;

    case ParryPhase::PlayFlinch:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
        {
            m_pOwner->SetAnimSpeed(3.0f);
            m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
            m_Phase = ParryPhase::PlayFlinchParis;
        }
        break;

    case ParryPhase::PlayFlinchParis:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
        {
            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
        }
        break;

    case ParryPhase::PlayHit:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Hit))
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
    // パリィフラグをリセット.
    m_pOwner->m_IsParried = false;
}
