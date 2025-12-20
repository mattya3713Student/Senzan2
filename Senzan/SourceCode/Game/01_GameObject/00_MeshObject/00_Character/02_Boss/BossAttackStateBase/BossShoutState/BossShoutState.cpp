#include "BossShoutState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

BossShoutState::BossShoutState(Boss* owner)
	: BossAttackStateBase (owner)
    , m_pBossIdol()

    , m_StartPos()

    , m_List(enShout::none)
{
}

BossShoutState::~BossShoutState()
{
}

void BossShoutState::Enter()
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

    //初期位置を保存.
    DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);

    //アニメーション速度.
    m_pOwner->SetAnimSpeed(15.0);
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
            m_pOwner->SetAnimSpeed(10.0);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Laser);
            m_List = enShout::ShoutTime;
        }
        break;
    case BossShoutState::enShout::ShoutTime:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Laser))
        {
            m_pOwner->SetAnimSpeed(15.0);
            m_pOwner->ChangeAnim(Boss::enBossAnim::LaserEnd);
            m_List = enShout::ShoutToIdol;
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
    BoneDraw();
}

void BossShoutState::Exit()
{
  
}

void BossShoutState::BoneDraw()
{
    if (m_pOwner->GetAttachMesh().expired()) return;

    std::shared_ptr<SkinMesh> staticMesh = std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
    if (!staticMesh) return;

    const std::string TargetBoneName = "Bone002";

    DirectX::XMFLOAT3 BonePos{};
    if (staticMesh->GetPosFromBone(TargetBoneName.c_str(), &BonePos))
    {
        DirectX::XMFLOAT3 ForWard = m_pOwner->GetTransform()->GetForward();

        float OffSetDist = 0.0f;

        DirectX::XMVECTOR FwdVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&ForWard));

        DirectX::XMVECTOR BoneVec = DirectX::XMLoadFloat3(&BonePos);
        DirectX::XMVECTOR OffSetPos = DirectX::XMVectorAdd(BoneVec, DirectX::XMVectorScale(FwdVec, OffSetDist));

        DirectX::XMStoreFloat3(&BonePos, OffSetPos);
        BonePos.y = 2.5f;

        m_pOwner->GetTransform()->SetPosition(BonePos);
    }
    else return;

  //  m_pColl->Draw(transform);

}

void BossShoutState::BossAttack()
{
}