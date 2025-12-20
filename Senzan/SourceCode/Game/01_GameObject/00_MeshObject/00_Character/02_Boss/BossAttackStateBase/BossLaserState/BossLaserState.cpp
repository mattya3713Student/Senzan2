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
    //ボスの向きを設定.
    const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
    DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

    const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
    DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

    DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
    //X,Z平面の方向.
    Direction = DirectX::XMVectorSetY(Direction, 0.0f);

    //Y軸回転角度を計算し、ボスをプレイヤーに向かせる.
    float dx = DirectX::XMVectorGetX(Direction);
    float dz = DirectX::XMVectorGetZ(Direction);
    float angle_radian = std::atan2f(-dx, -dz);
    m_pOwner->SetRotationY(angle_radian);

    //攻撃開始位置.
    DirectX::XMFLOAT3 m_StartPos;

    //初期位置を保存.
    DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);


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
