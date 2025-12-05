#include "BossLaserState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"


#include "00_MeshObject/00_Character/02_Boss/BossIdleState/BossIdleState.h"


BossLaserState::BossLaserState(Boss* owner)
	: BossAttackStateBase   (owner)
    , m_pBossIdol           ()
{
}

BossLaserState::~BossLaserState()
{
}

void BossLaserState::Enter()
{
    //DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
}

void BossLaserState::Update()
{
    const float deltaTime = Time::GetInstance().GetDeltaTime();

    //ŽžŠÔ‚Ì‰ÁŽZ.
    m_Timer += deltaTime;

    if (m_Timer > m_TransitionTimer)
    {
        m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdleState>(m_pOwner));
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
