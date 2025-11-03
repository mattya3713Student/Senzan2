#include "BossMoveState.h"

#include "Game/04_Time/Time.h"

BossMoveState::BossMoveState(Boss* owner)
	: StateBase<Boss>(owner)
	, m_RotationAngle(0.0f)
	, m_RotationSpeed(0.2f)
	, m_rotationDirection(1.0f)

//	, m_pColl(std::make_shared<CapsuleCollider>())
{
	//m_pColl->Create();
	//m_pColl->SetHeight(100.0f);
	//m_pColl->SetRadius(35.0f);

}

BossMoveState::~BossMoveState()
{
}

void BossMoveState::Enter()
{
}

void BossMoveState::Update()
{
	const DirectX::XMFLOAT3& BossPos = m_pOwner->GetPosition();
	const DirectX::XMFLOAT3& PlayerPos = m_pOwner->m_PlayerPos;

	//デルタタイムを使用して回転角度を更新させる.
	float deltaTime = Time::GetInstance().GetDeltaTime();
	float deltaAngle = m_RotationSpeed * deltaTime * m_rotationDirection;

	////回転角度のメンバー変数を更新.
	//m_RotationAngle += deltaAngle;

	////角度が半径の範囲を超えたら回転方向を反転させる.
	//const float SPECIFIED_RANGE_ANGLE = DirectX::XM_PIDIV4;
	//if (abs(m_RotationAngle) >= SPECIFIED_RANGE_ANGLE)
	//{
	//	//方向の反転.
	//	m_rotationDirection *= -1.0f;
	//}

	// Y軸周りの回転行列で変換.
	DirectX::XMMATRIX rotatedMatrix = DirectX::XMMatrixRotationY(m_RotationAngle);

	// ベクトルを回転行列で変換.
	// 基準のベクトルを変換.
	DirectX::XMVECTOR initialVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR rotatedVec = DirectX::XMVector3Transform(initialVec, rotatedMatrix);

	// 正規化.
	rotatedVec = DirectX::XMVector3Normalize(rotatedVec);

	constexpr float BOSS_RADIUS = 10.0f;

	DirectX::XMVECTOR playerPosXM = XMLoadFloat3(&PlayerPos);
	DirectX::XMVECTOR offsetVec = DirectX::XMVectorScale(rotatedVec, BOSS_RADIUS);
	//問題としてはここが原因の可能性があります.
	DirectX::XMVECTOR newBossPosXM = DirectX::XMVectorAdd(playerPosXM, offsetVec);
	DirectX::XMFLOAT3 NewBossPos;
	DirectX::XMStoreFloat3(&NewBossPos, newBossPosXM); 

	//ボスの位置を更新.
	m_pOwner->SetPosition(NewBossPos);

	//アニメーション切替
	m_AnimNo = 0;		//登場アニメーション
	m_AnimTimer = 0.0;	//アニメーション経過時間初期化
	m_pOwner->SetAnimSpeed(m_AnimSpeed);

	//アニメーション再生の無限ループ用.
	//m_pOwner->SetIsLoop(true);
	m_pOwner->ChangeAnim(m_AnimNo);


	// NewBossPosを直接BOSSの座標に入れるのではなく,
	// その座標を目標(m_TargetPos)として現在のBoss座標から
	// m_TargetPosに向かって進む処理にするとよい.

	//今わかったバクなんだけど、行動からアイドルに入ったときにMoveに入るんだけど左右移動が動かない.
}

void BossMoveState::LateUpdate()
{
}

void BossMoveState::Draw()
{
	DrawBone();
}

void BossMoveState::Exit()
{
}

void BossMoveState::DrawBone()
{
	if (m_pOwner->GetAttachMesh().expired()) return;

	std::shared_ptr<SkinMesh> staticMesh = std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
	if (!staticMesh) return;

	const std::string TargetBoneName = "Bone002";

	DirectX::XMFLOAT3 BonePos{};
	if (staticMesh->GetPosFromBone(TargetBoneName.c_str(), &BonePos))
	{
		// 向きを取得.
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

	//m_pColl->Draw(transform);

}


void BossMoveState::SetInitialAngle(float angle)
{
	m_RotationAngle = angle;
}
