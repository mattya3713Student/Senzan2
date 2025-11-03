#include "BossShoutState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

BossShoutState::BossShoutState(Boss* owner)
	: BossAttackStateBase (owner)
    , m_ChargeDuration(1.0f)     
    , m_AttackDuration(0.2f)     
    , m_CoolDownDuration(0.8f)      
    , m_ShoutRadius(5.0f)      
    , m_KnockBackForce(500.0f)   
    , m_KnockBackUpForce(100.0f)   
    , m_CurrentPhase(Phase::Charge)
    , m_PhaseTime(0.0f)
    , m_HasHit(false)

    //, m_pColl  (std::make_shared<CapsuleCollider>())
{
    //m_pColl->Create();
    //m_pColl->SetHeight(100.0f);
    //m_pColl->SetRadius(50.0f);
}

BossShoutState::~BossShoutState()
{
}

void BossShoutState::Enter()
{
    //攻撃系統の初期化.
    m_Attacktime = 0.0f;
    m_PhaseTime = 0.0f;
    m_CurrentPhase = Phase::Charge;
    m_HasHit = false;

    //叫び攻撃全方位から攻撃可能(ダメージはなし).
}

void BossShoutState::Update()
{
    const float Time_Rate = 1.0f;

    //全体の時間とフェーズ内時間を進める.
    m_Attacktime += Time_Rate * Time::GetInstance().GetDeltaTime();
    m_PhaseTime += Time::GetInstance().GetDeltaTime();

    //switch (m_CurrentPhase)
    //{
    //case BossShoutState::Phase::Charge:
    //    if (m_PhaseTime >= m_ChargeDuration)
    //    {
    //        //予兆から攻撃.
    //        m_CurrentPhase = Phase::Attack;
    //        m_PhaseTime = 0.0f;

    //        //いったん確認のために作成しているだけです.
    //        //最終的にはBossAttackに導入します.
    //        //今ここでは、プレイヤーとボスの位置を取得している.
    //        DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
    //        DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
    //    }
    //    break;
    //case BossShoutState::Phase::Attack:
    //    BossAttack();
    //    if (m_PhaseTime >= m_AttackDuration)
    //    {
    //        //攻撃からクールダウン.
    //        m_CurrentPhase = Phase::CoolDown;
    //        m_PhaseTime = 0.0f;
    //    }
    //    break;
    //case BossShoutState::Phase::CoolDown:
    //    if (m_PhaseTime >= m_CoolDownDuration)
    //    {
    //        //動作クラスへの移動.
    //        m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_pOwner));
    //        return;
    //    }
    //    break;
    //default:
    //    break;
    //}

    //アニメーション切替
    m_AnimNo = 0;		//登場アニメーション
    m_AnimTimer = 0.0;	//アニメーション経過時間初期化
    m_pOwner->SetAnimSpeed(m_AnimSpeed);

    //アニメーション再生の無限ループ用.
    //m_pOwner->SetIsLoop(true);
    m_pOwner->ChangeAnim(m_AnimNo);

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
    Transform def = m_pOwner->GetTransform();
    std::shared_ptr<Transform> transform = std::make_shared<Transform>(def);

    if (m_pOwner->GetAttachMesh().expired()) return;

    std::shared_ptr<SkinMesh> staticMesh = std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
    if (!staticMesh) return;

    const std::string TargetBoneName = "Bone002";

    DirectX::XMFLOAT3 BonePos{};
    if (staticMesh->GetPosFromBone(TargetBoneName.c_str(), &BonePos))
    {
        DirectX::XMFLOAT3 ForWard = def.GetForward();

        float OffSetDist = 0.0f;

        DirectX::XMVECTOR FwdVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&ForWard));

        DirectX::XMVECTOR BoneVec = DirectX::XMLoadFloat3(&BonePos);
        DirectX::XMVECTOR OffSetPos = DirectX::XMVectorAdd(BoneVec, DirectX::XMVectorScale(FwdVec, OffSetDist));

        DirectX::XMStoreFloat3(&BonePos, OffSetPos);
        BonePos.y = 2.5f;

        transform->SetPosition(BonePos);
    }
    else return;

  //  m_pColl->Draw(transform);

}

void BossShoutState::BossAttack()
{
    //Attackフェーズで、まだ判定を出していないときは実行.
    if (m_CurrentPhase != Phase::Attack || m_HasHit)
    {
        return;
    }

    DirectX::XMFLOAT3 PlayerPosF = m_pOwner->GetTargetPos();
    DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();

    //ノックバック判定の基準.
    DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);
    DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

    //ボスからプレイヤーへのベクトル（XZ平面）
    DirectX::XMVECTOR toPlayerVec = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
    DirectX::XMVECTOR toPlayerVecXZ = DirectX::XMVectorSetY(toPlayerVec, 0.0f); // XZ平面のみで計算

    //距離のチェック
    float DistSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(toPlayerVecXZ));
    if (DistSq <= m_ShoutRadius * m_ShoutRadius)
    {
        //ノックバックの計算と適用

        //ノックバックの方向ベクトル（ボスから離れる方向）
        DirectX::XMVECTOR KnockBackDir = DirectX::XMVector3Normalize(toPlayerVecXZ);

        //水平方向のノックバック力ベクトル
        DirectX::XMVECTOR HoriZontalForce = DirectX::XMVectorScale(KnockBackDir, m_KnockBackForce);

        //垂直方向のノックバック力ベクトル
        DirectX::XMVECTOR VertiCalForce = DirectX::XMVectorSet(0.0f, m_KnockBackUpForce, 0.0f, 0.0f);

        //合計の力ベクトル
        DirectX::XMVECTOR toTalForce = DirectX::XMVectorAdd(HoriZontalForce, VertiCalForce);
    }

    //ここにノックバック用のプレイヤークラスの関数を定義する.


    m_HasHit = true;
}