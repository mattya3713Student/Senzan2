#include "BossMoveState.h"

#include "Game/04_Time/Time.h"

BossMoveState::BossMoveState(Boss* owner)
	: StateBase<Boss>(owner)
	, m_RotationAngle(0.0f)
	, m_RotationSpeed(0.005f)
	, m_rotationDirection(1.0f)

	, m_pAttack	(std::make_shared<BossSpecialState>(owner))
	, m_pChage(std::make_shared<BossChargeSlashState>(owner))
	, m_pSlash(std::make_shared<BossSlashState>(owner))
	, m_pCharge(std::make_shared<BossChargeState>(owner))

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

	timer += deltaTime;

	//回転角度のメンバー変数を更新.
	m_RotationAngle += deltaAngle;

	//角度が半径の範囲を超えたら回転方向を反転させる.
	const float SPECIFIED_RANGE_ANGLE = DirectX::XM_PIDIV4;
	if (abs(m_RotationAngle) >= SPECIFIED_RANGE_ANGLE)
	{
		//方向の反転.
		m_rotationDirection *= -1.0f;
	}

	// Y軸周りの回転行列で変換.
	DirectX::XMMATRIX rotatedMatrix = DirectX::XMMatrixRotationY(m_RotationAngle);

	// ベクトルを回転行列で変換.
	// 基準のベクトルを変換.
	DirectX::XMVECTOR initialVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR rotatedVec = DirectX::XMVector3Transform(initialVec, rotatedMatrix);

	// 正規化.
	rotatedVec = DirectX::XMVector3Normalize(rotatedVec);

	constexpr float BOSS_RADIUS = 10.0f;


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



	DirectX::XMVECTOR offsetVec = DirectX::XMVectorScale(rotatedVec, BOSS_RADIUS);

	//問題としてはここが原因の可能性があります.
	//ここ原因
	DirectX::XMVECTOR newBossPosXM = DirectX::XMVectorAdd(PlayerPosXM, offsetVec);
	DirectX::XMFLOAT3 NewBossPos;
	DirectX::XMStoreFloat3(&NewBossPos, newBossPosXM);

	//ベクトル
	DirectX::XMVECTOR v_test = DirectX::XMVectorSubtract(newBossPosXM, BossPosXM);
	DirectX::XMFLOAT3 test = {};

	std::shared_ptr<Transform> transform = std::make_shared<Transform>();
	transform->SetPosition(NewBossPos);
	DirectX::XMFLOAT3 tessss = { 0.05f,0.05f,0.05f };
	transform->SetScale(tessss);

	// 正規化.
	v_test = DirectX::XMVector3Normalize(v_test);
	v_test = DirectX::XMVectorScale(v_test, 0.005f);
	DirectX::XMStoreFloat3(&test, v_test);

	m_pOwner->AddPosition(test);

	if (timer >= 15.0f)
	{
		m_pOwner->GetStateMachine()->ChangeState(std::make_unique<BossChargeState>(m_pOwner));
	}


	//アニメーション切替
	m_AnimNo = 0;		//登場アニメーション
	m_AnimTimer = 0.0;	//アニメーション経過時間初期化
	//m_pOwner->SetAnimSpeed(m_AnimSpeed);

	//アニメーション再生の無限ループ用.
	//m_pOwner->SetIsLoop(true);
	//m_pOwner->ChangeAnim(m_AnimNo);
}

void BossMoveState::LateUpdate()
{
}

void BossMoveState::Draw()
{
	//DrawBone();
}

void BossMoveState::Exit()
{
}

//void BossMoveState::DrawBone()
//{
//	const Transform& ownerTransform = m_pOwner->GetTransform();
//
//	std::shared_ptr<Transform> transform = std::make_shared<Transform>(ownerTransform);
//
//	if (m_pOwner->GetAttachMesh().expired()) return;
//
//	std::shared_ptr<SkinMesh> staticMesh = std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
//	if (!staticMesh) return;
//
//	const std::string TargetBoneName = "Bone002";
//
//	DirectX::XMFLOAT3 BonePos{};
//	if (staticMesh->GetPosFromBone(TargetBoneName.c_str(), &BonePos))
//	{
//		// 向きを取得.
//		DirectX::XMFLOAT3 ForWard = ownerTransform.GetForward();
//
//		float OffSetDist = 0.0f;
//
//		DirectX::XMVECTOR FwdVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&ForWard));
//
//		DirectX::XMVECTOR BoneVec = DirectX::XMLoadFloat3(&BonePos);
//		DirectX::XMVECTOR OffSetPos = DirectX::XMVectorAdd(BoneVec, DirectX::XMVectorScale(FwdVec, OffSetDist));
//
//		DirectX::XMStoreFloat3(&BonePos, OffSetPos);
//		BonePos.y = 2.5f;
//
//		transform->SetPosition(BonePos);
//	}
//	else return;
//
//	//m_pColl->Draw(transform);
//
//}


void BossMoveState::SetInitialAngle(float angle)
{
	m_RotationAngle = angle;
}
