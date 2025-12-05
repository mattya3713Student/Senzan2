#include "BossIdleState.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossMoveState/BossMoveState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossStompState/TestStomp.h"

BossIdleState::BossIdleState(Boss* owner)
	: StateBase<Boss>(owner)
//	, m_pColl(std::make_shared<CapsuleCollider>())
{
	//m_pColl->Create();
	//m_pColl->SetHeight(100.0f);
	//m_pColl->SetRadius(35.0f);


}

BossIdleState::~BossIdleState()
{
}

void BossIdleState::Enter()
{
	m_pOwner->ChangeAnim(0);
}

void BossIdleState::Update()
{

	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {

		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<TestStomp>(m_pOwner));
		return;
	}

	//ここにプレイヤーとボスの距離を図る(三平方の定理を使用して作成していく)

	//ボスのポジションを入手する.
	// 修正: GetPosition() が XMFLOAT3 を返す前提でロード
	const DirectX::XMFLOAT3& BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	//プレイヤーのポジションを入手する.
	const DirectX::XMFLOAT3& PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	//プレイヤー - ボスの距離を求めていく.
	// D3DXVECTOR3 DistancePos = PlayerPos - BossPos; -> XMVectorSubtract
	DirectX::XMVECTOR DistancePosXM = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);

	//長さの2乗を求める.
	// D3DXVec3LengthSq(&DistancePos); -> XMVector3LengthSq
	// 注意: 戻り値はXMVECTORなので、XMVectorGetXでfloatを取り出す
	float DistanceSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DistancePosXM));

	//ローカル変数.
	constexpr float Ten = 5.0f;

	//距離でアイドルから動作へ変更させたい.
	//今は10.0fに入ったら次の動作に入る.
	//2上しているので10.0f * 10.0fの計算式になる
	const float IDLE_RANGE_SQ = Ten * Ten;

	//atan2を使用してMoveに入る前に角度を取得しておく.
	if (DistanceSq <= IDLE_RANGE_SQ)
	{
		//プレイヤーからボスへのベクトルを取得.
		// D3DXVECTOR3 direction = BossPos - PlayerPos; -> XMVectorSubtract
		DirectX::XMVECTOR directionXM = DirectX::XMVectorSubtract(BossPosXM, PlayerPosXM);

		// 2D平面上の角度の計算(Y軸は無視).
		// XMVECTORのX, Z成分を取得
		float dx = DirectX::XMVectorGetX(directionXM);
		float dz = DirectX::XMVectorGetZ(directionXM);

		// atan2()を使用.
		float initalAngle = atan2f(dx, dz); // DirectXMathでは atan2f を使用

		//今ジョイントのためにここをコメント化している.
		//このコードでBossのState系統を変更している.
		auto MoveState = std::make_shared<BossChargeState>(m_pOwner);
		//MoveState->SetInitialAngle(initalAngle);

		m_pOwner->GetStateMachine()->ChangeState(MoveState);
		return;
	}
	//アニメーション切替
	//m_AnimNo = 0;		//登場アニメーション
	m_AnimTimer = 0.0;	//アニメーション経過時間初期化

	//アニメーション再生の無限ループ用.
	//m_pOwner->SetIsLoop(true);

}

void BossIdleState::LateUpdate()
{
}

void BossIdleState::Draw()
{
	DrawBone();
}

void BossIdleState::Exit()
{
}

void BossIdleState::DrawBone()
{
	// メッシュとボーンのチェック.
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

		XMStoreFloat3(&BonePos, OffSetPos);
		BonePos.y = 2.5f;

		m_pOwner->GetTransform()->SetPosition(BonePos);
	}
	else return;

	// m_pColl->Draw(transform);
}
