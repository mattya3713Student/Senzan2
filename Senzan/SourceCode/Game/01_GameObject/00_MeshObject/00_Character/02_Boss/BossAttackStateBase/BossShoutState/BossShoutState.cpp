#include "BossShoutState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

BossShoutState::BossShoutState(Boss* owner)
	: BossAttackStateBase (owner)
    , m_pBossIdol()
    , m_List(enShout::none)
{
}

BossShoutState::~BossShoutState()
{
}

void BossShoutState::Enter()
{

    if (auto* shoutCol = m_pOwner->GetShoutCollider()) 
    {
        shoutCol->SetActive(true);
    }

    // 当たり判定を有効化.
    m_pOwner->SetAttackColliderActive(true);

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

    //初期位置を保存.
    DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);

    //アニメーション速度.
    m_pOwner->SetAnimSpeed(1.5);
    m_pOwner->ChangeAnim(Boss::enBossAnim::LaserCharge);
}

void BossShoutState::Update()
{
    switch (m_List)
    {
    case BossShoutState::enShout::none:
        m_List = enShout::Shout;
        break;

    case BossShoutState::enShout::Shout:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserCharge))
        {
            m_pOwner->SetAnimSpeed(1.0);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Laser);
            m_List = enShout::ShoutTime;
        }
        break;

    case BossShoutState::enShout::ShoutTime:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Laser))
        {
            m_pOwner->SetAnimSpeed(1.5);
            m_pOwner->ChangeAnim(Boss::enBossAnim::LaserEnd);
            m_List = enShout::ShoutToIdol;

            if (auto* shoutCol = m_pOwner->GetShoutCollider()) 
            {
                shoutCol->SetActive(false);
            }
        }
        break;

    case BossShoutState::enShout::ShoutToIdol:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserEnd))
        {
            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
        }
        break;

    default:
        break;
    }
}

void BossShoutState::LateUpdate()
{
}

void BossShoutState::Draw()
{
}

void BossShoutState::Exit()
{	// 当たり判定を無効化.
    m_pOwner->SetAttackColliderActive(false);
  
}

void BossShoutState::BossAttack()
{
}
