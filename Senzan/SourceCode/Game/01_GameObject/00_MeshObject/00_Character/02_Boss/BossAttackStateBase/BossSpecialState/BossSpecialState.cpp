#include "BossSpecialState.h"
#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

BossSpecialState::BossSpecialState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_List(enSpecial::None)
	, m_Flinch(enFlinch::none)
	, m_Velocity(0.0f, 0.0f, 0.0f)
	, m_SpecialPower(1.5f)
	, m_Gravity(0.089f)
	, m_SpecialFrag(false)
	, m_GroundedFrag(true)
	, m_Timer(0.0f)
	, m_TransitionTimer(5.0f)
	, m_AttackTimer(0.0f)
	, m_UpSpeed(0.15f)
	, m_TargetDirection(0.0f, 0.0f, 0.0f)
	, m_MaxTrackingAngle(3.0f)
	, m_AttackMoveSpeed(80.5f)
	, m_AttackDistance(9999.0f)
	, m_DistanceTraveled(0.0f)
{
}

BossSpecialState::~BossSpecialState()
{
}

void BossSpecialState::Enter()
{
	m_Timer = 0.0f;
	m_Velocity = {};
	m_DistanceTraveled = 0.0f;
	m_GroundedFrag = true;
	m_pOwner->SetAnimSpeed(2.0);
	// 最初の待機モーションへ（必要に応じて追加）
	m_pOwner->ChangeAnim(Boss::enBossAnim::Idol);
}

void BossSpecialState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	switch (m_List)
	{
	case BossSpecialState::enSpecial::None:
		m_Velocity = {};
		// 当たり判定を有効化.
		m_pOwner->SetAttackColliderActive(true);
		m_List = enSpecial::Charge;
		break;

	case BossSpecialState::enSpecial::Charge:
		ChargeTime();
		break;

	case BossSpecialState::enSpecial::Jump:
		JumpTime();
		break;

	case BossSpecialState::enSpecial::Attack:
		BossAttack();
		break;

	case BossSpecialState::enSpecial::CoolTime:
		m_Timer += deltaTime;
		// SpecialToIdolアニメーションが終了したら遷移
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SpecialToIdol))
		{
			m_List = enSpecial::Trans;
		}
		break;

	case BossSpecialState::enSpecial::Trans:
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;

	default:
		break;
	}
}

void BossSpecialState::LateUpdate()
{
}

void BossSpecialState::Draw()
{
}

void BossSpecialState::Exit()
{
	m_GroundedFrag = true;
	m_SpecialFrag = false;
	m_AttackTimer = 0.0f;

	// 終了時にプレイヤーの方を向き直す
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->GetTargetPos();
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	if (XMVectorGetX(XMVector3LengthSq(Direction)) > 0.0001f)
	{
		float dx = DirectX::XMVectorGetX(Direction);
		float dz = DirectX::XMVectorGetZ(Direction);
		// プレイヤーの方向を正面にする
		float angle_radian = std::atan2f(dx, dz) + XM_PI;
		m_pOwner->SetRotationY(angle_radian);
	}

	// 当たり判定を有効化.
	m_pOwner->SetAttackColliderActive(false);
}

void BossSpecialState::ParryTime()
{
	switch (m_Flinch)
	{
	case BossSpecialState::enFlinch::none:
		m_Flinch = enFlinch::Flinch;
		break;
	case BossSpecialState::enFlinch::Flinch:
		m_pOwner->SetAnimSpeed(3.0);
		m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
		{
			m_pOwner->SetAnimSpeed(1.0);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
			m_Flinch = enFlinch::FlinchTimer;
		}
		break;
	case BossSpecialState::enFlinch::FlinchTimer:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
		{
			m_pOwner->SetAnimSpeed(3.0);
			m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
			m_Flinch = enFlinch::FlinchToIdol;
		}
		break;
	case BossSpecialState::enFlinch::FlinchToIdol:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchToIdol))
		{
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		}
		break;
	}
}

void BossSpecialState::ChargeTime()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();
	m_Timer += deltaTime;

	if (m_Timer > m_AttackTimer)
	{
		m_Timer = 0.0f;
		m_Velocity = { 0.0f, 0.0f, 0.0f };
		m_pOwner->SetAnimSpeed(3.0);
		m_pOwner->ChangeAnim(Boss::enBossAnim::Special_0);
		m_List = enSpecial::Jump;
	}
}

void BossSpecialState::JumpTime()
{
	if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0))
	{
		m_DistanceTraveled = 0.0f;

		// 空中にワープ
		XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
		CurrentPos.y = 10.0f;
		m_pOwner->SetPosition(CurrentPos);

		// ターゲット計算（プレイヤーの手前を狙う）
		XMFLOAT3 PlayerPos = m_pOwner->GetTargetPos();
		XMVECTOR bPos = XMLoadFloat3(&CurrentPos);
		XMVECTOR pPos = XMLoadFloat3(&PlayerPos);

		XMVECTOR toPlayerVec = XMVectorSubtract(pPos, bPos);
		XMVECTOR toPlayerDir = XMVector3Normalize(toPlayerVec);

		// プレイヤーの 3.0f 手前を着地目標にする（通り過ぎ防止）
		float offsetDist = 3.0f;
		XMVECTOR targetPos = XMVectorSubtract(pPos, XMVectorScale(toPlayerDir, offsetDist));

		XMVECTOR finalTargetVec = XMVectorSubtract(targetPos, bPos);
		finalTargetVec = XMVector3Normalize(finalTargetVec);

		DirectX::XMStoreFloat3(&m_TargetDirection, finalTargetVec);

		m_pOwner->SetAnimSpeed(1.0);
		m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
		m_List = enSpecial::Attack;
	}
}

void BossSpecialState::BossAttack()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();
	const float floorY = 0.0f;
	const float PlayerYOffset = 1.0f;
	const float One = 1.0f;
	const float MinusOne = -1.0f;

	XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
	XMFLOAT3 TargetPosF = m_pOwner->GetTargetPos();
	TargetPosF.y += PlayerYOffset;

	XMVECTOR BossPosVec = XMLoadFloat3(&CurrentPos);
	XMVECTOR PlayerPosVec = XMLoadFloat3(&TargetPosF);

	// 距離判定と高さ判定による終了処理
	float DistanceToPlayer = XMVectorGetX(XMVector3Length(XMVectorSubtract(PlayerPosVec, BossPosVec)));

	// 修正：プレイヤーとの距離が近い(2.5f以下)か、地面に付いたら攻撃終了
	if (CurrentPos.y <= floorY + 0.1f || DistanceToPlayer <= 2.5f)
	{
		CurrentPos.y = floorY;
		m_pOwner->SetPosition(CurrentPos);
		m_Timer = 0.0f;
		m_pOwner->SetAnimSpeed(3.0);
		m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
		m_List = enSpecial::CoolTime;
		return;
	}

	// 追尾（ホーミング）計算
	XMVECTOR CurrentDir = XMLoadFloat3(&m_TargetDirection);
	XMVECTOR finalMoveDir;

	float trackingThreshold = 15.0f;

	if (DistanceToPlayer > trackingThreshold)
	{
		XMVECTOR ToPlayerDir = XMVector3Normalize(XMVectorSubtract(PlayerPosVec, BossPosVec));

		float lerpFactor = 1.0f * deltaTime;
		XMVECTOR TargetStepDir = XMVectorLerp(CurrentDir, ToPlayerDir, lerpFactor);
		TargetStepDir = XMVector3Normalize(TargetStepDir);

		XMVECTOR DotProduct = XMVector3Dot(CurrentDir, TargetStepDir);
		float cosTheta = XMVectorGetX(DotProduct);
		float AngleRad = acosf(std::max(MinusOne, std::min(One, cosTheta)));

		float maxTurnRadians = XMConvertToRadians(m_MaxTrackingAngle) * deltaTime;

		if (AngleRad > maxTurnRadians)
		{
			XMVECTOR rotationAxis = XMVector3Cross(CurrentDir, TargetStepDir);
			if (XMVectorGetX(XMVector3LengthSq(rotationAxis)) > 0.0001f)
			{
				rotationAxis = XMVector3Normalize(rotationAxis);
				XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, maxTurnRadians);
				finalMoveDir = XMVector3TransformNormal(CurrentDir, rotationMatrix);
			}
			else { finalMoveDir = CurrentDir; }
		}
		else { finalMoveDir = TargetStepDir; }
	}
	else
	{
		// 一定距離内ならホーミングを停止し、慣性で突っ込む
		finalMoveDir = CurrentDir;
	}

	finalMoveDir = XMVector3Normalize(finalMoveDir);
	XMStoreFloat3(&m_TargetDirection, finalMoveDir);

	// 移動実行
	XMVECTOR moveVector = XMVectorScale(finalMoveDir, m_AttackMoveSpeed * deltaTime);
	XMVECTOR newBossPosVec = XMVectorAdd(BossPosVec, moveVector);

	XMFLOAT3 newBossPos;
	XMStoreFloat3(&newBossPos, newBossPosVec);

	// 地面を突き抜けないように制限
	if (newBossPos.y < floorY) newBossPos.y = floorY;

	m_pOwner->SetPosition(newBossPos);
	m_DistanceTraveled += m_AttackMoveSpeed * deltaTime;
}
