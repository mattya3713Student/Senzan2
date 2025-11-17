#include "BossStompState.h"
#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先
#include "00_MeshObject/00_Character/02_Boss/BossIdleState/BossIdleState.h"

BossStompState::BossStompState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_StopmDuration(10.0f)
	, m_JumpHeigt(30.0f)
	, m_StopmRadius(50.0f)
	, m_InitBossPos()
	, m_HasHitGround(false)

	, m_IdolDuration(50.0f)      // 1.5秒予兆
	, m_AttackDuration(100.0f)      // 1.0秒かけて薙ぎ払う
	, m_CoolDownDuration(50.0f)      // 1.5秒硬直

	, m_CurrentPhase(Phase::Idol)
	, m_PhaseTime(0.0f)

	//, m_pColl(std::make_shared<CapsuleCollider>())

{
	//m_pColl->Create();
	//m_pColl->SetHeight(5.0f);
	//m_pColl->SetRadius(5.0f);
}


BossStompState::~BossStompState()
{
}

void BossStompState::Enter()
{
	m_Attacktime = 0.0f;
	m_HasHitGround = false;

	//ボスのポジションを入手する.
	// 修正: GetPosition() が XMFLOAT3 を返す前提でロード
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	//正しくメンバ変数に保存する.
	//第一引数が、自分で命名した変数な度を書く(DirectX::XMFLOAT3型)を書かないとエラーになる.
	//第二引数には、BossPosやPlayerPosを書かないといけない(DirectX::XMVECTOR3型).
	DirectX::XMStoreFloat3(&m_InitBossPos, BossPosXM);

	//プレイヤーのポジションを入手する.
	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	//ボスをプレイヤーの方向に向かせる.
	//今はStaticMeshだから正面かは分からないが、
	//方向ベクトルを計算.
	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);

	//X,Z平面での向きを取得.
	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);

	//atan2fでY軸回転角度を計算.
	float angle_radian = std::atan2f(dx, dz);

	m_pOwner->SetRotationY(angle_radian);
}

void BossStompState::Update()
{
	const float Time_Rate = 1.0f;

	const float deltaTime = Time::GetInstance().GetDeltaTime();

	//全体の時間とフェーズ内時間を進める.
	m_Attacktime += Time_Rate * Time::GetInstance().GetDeltaTime();
	m_PhaseTime += Time::GetInstance().GetDeltaTime();


	switch (m_CurrentPhase)
	{
	case BossStompState::Phase::Idol:
		if (m_PhaseTime >= m_IdolDuration)
		{
			//予兆から攻撃.
			m_CurrentPhase = Phase::Attack;
			m_PhaseTime = 0.0f;
		}
		break;
	case BossStompState::Phase::Attack:
		BossAttack();
		if (m_PhaseTime >= m_AttackDuration)
		{
			//攻撃からクールダウン.
			m_CurrentPhase = Phase::CoolDown;
			m_PhaseTime = 0.0f;
		}
		break;
	case BossStompState::Phase::CoolDown:
		if (m_PhaseTime >= m_CoolDownDuration)
		{
			//動作クラスへの移動.
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_pOwner));
			return;
		}
		break;
	default:
		break;
	}

	//アニメーション切替
	m_AnimNo = 0;		//登場アニメーション
	m_AnimTimer = 0.0;	//アニメーション経過時間初期化
	m_pOwner->SetAnimSpeed(m_AnimSpeed);

	//アニメーション再生の無限ループ用.
	//m_pOwner->SetIsLoop(true);
	m_pOwner->ChangeAnim(m_AnimNo);
}

void BossStompState::LateUpdate()
{
}

void BossStompState::Draw()
{
	BoneDraw();
}

void BossStompState::Exit()
{
	// 攻撃終了時にボスのY座標を地面(0.0f)に確実に設定
	const DirectX::XMFLOAT3 posF = m_pOwner->GetPosition();
	DirectX::XMVECTOR pos = XMLoadFloat3(&posF);
	pos = DirectX::XMVectorSetY(pos, 0.0f);

	// XMFLOAT3に戻してSetPositionに渡す
	DirectX::XMFLOAT3 posToSet;
	XMStoreFloat3(&posToSet, pos);
	m_pOwner->SetPosition(posToSet);

}

void BossStompState::BoneDraw()
{
	if (m_pOwner->GetAttachMesh().expired()) return;

	std::shared_ptr<SkinMesh> staticMesh = std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
	if (!staticMesh) return;

	const std::string targetBoneName = "Bone002";

	DirectX::XMFLOAT3 BonePos{};
	if (staticMesh->GetPosFromBone(targetBoneName.c_str(), &BonePos))
	{
		// 向きを取得.
		DirectX::XMFLOAT3 ForWard = m_pOwner->GetTransform()->GetForward();

		// オフセット距離.
		float OffSetDist = 0.0f;

		// 方向ベクトルを正規化.
		DirectX::XMVECTOR FwdVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&ForWard));

		// オフセットを計算.
		DirectX::XMVECTOR boneVec = DirectX::XMLoadFloat3(&BonePos);
		DirectX::XMVECTOR offsetPos = DirectX::XMVectorAdd(boneVec, DirectX::XMVectorScale(FwdVec, OffSetDist));

		// 結果を格納.
		XMStoreFloat3(&BonePos, offsetPos);
		BonePos.y -= 2.5f;

		// トランスフォームを更新.
		m_pOwner->GetTransform()->SetPosition(BonePos);
	}
	else
	{
		return;
	}
	//m_pColl->Draw(transfrom);
}

void BossStompState::BossAttack()
{
	// 1. 時間tの計算 (0.0fから1.0f)
	float t = m_Attacktime / m_StopmDuration;

	// 2. 初期位置をXMVECTORにロード
	DirectX::XMVECTOR CurrentPos = XMLoadFloat3(&m_InitBossPos);

	if (m_Attacktime < m_StopmDuration)
	{
		// Y座標の計算（放物線運動）
		float NormalizedT = t;
		// 4 * 高さ * t * (1 - t) で Y=0 -> 最大高さ -> Y=0 の軌道を作る
		float NewY = 4.0f * m_JumpHeigt * NormalizedT * (1.0f - NormalizedT);

		// XMVECTORのY座標のみを更新
		CurrentPos = DirectX::XMVectorSetY(CurrentPos, NewY);
	}
	else
	{
		// 着地後の位置を確定 (Y=0)
		CurrentPos = DirectX::XMVectorSetY(CurrentPos, 0.0f);
	}

	// 3. ボスの位置を更新
	DirectX::XMFLOAT3 posToSet;
	DirectX::XMStoreFloat3(&posToSet, CurrentPos);
	m_pOwner->SetPosition(posToSet);

	//// 4. 当たり判定のチェック (着地時)
	//DirectX::XMFLOAT3 PosFloat3;
	//DirectX::XMStoreFloat3(&PosFloat3, CurrentPos); // 判定用にXMFLOAT3に戻す

	//// Y座標が地面以下に到達し、かつ一度も判定を出していないかチェック
	//if (PosFloat3.y <= 0.0f && m_Attacktime >= m_StopmDuration * 0.5f && !m_HasHitGround)
	//{
	//	// プレイヤーの位置をロード
	//	const DirectX::XMFLOAT3& PlayerPosF = m_pOwner->m_TargetPos;
	//	DirectX::XMVECTOR PlayerPosXM = XMLoadFloat3(&PlayerPosF);

	//	// XZ平面上の距離計算 (Y軸を無視)
	//	DirectX::XMVECTOR BossXZ = DirectX::XMVectorSetY(CurrentPos, 0.0f);
	//	DirectX::XMVECTOR PlayerXZ = DirectX::XMVectorSetY(PlayerPosXM, 0.0f);

	//	DirectX::XMVECTOR DistanceVec = DirectX::XMVectorSubtract(BossXZ, PlayerXZ);
	//	DirectX::XMVECTOR DistanceSqVec = DirectX::XMVector3LengthSq(DistanceVec);
	//	float DistanceSq;
	//	DirectX::XMStoreFloat(&DistanceSq, DistanceSqVec);

	//	// 判定範囲内かチェック
	//	if (DistanceSq <= m_StopmRadius * m_StopmRadius)
	//	{
	//		// ダメージ処理の実行 (ここに m_pOwner->GetPlayer()->TakeDamage(20) など)
	//	}

	//	m_HasHitGround = true;
	//}
}
