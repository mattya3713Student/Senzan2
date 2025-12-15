#include "BossShoutState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

BossShoutState::BossShoutState(Boss* owner)
	: BossAttackStateBase (owner)
    , m_pBossIdol()
{
}

BossShoutState::~BossShoutState()
{
}

void BossShoutState::Enter()
{
    //アニメーション速度.
    m_pOwner->SetAnimSpeed(0.01);
    m_pOwner->ChangeAnim(Boss::enBossAnim::LaserCharge);
}

void BossShoutState::Update()
{
    if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserCharge))
    {
        m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
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