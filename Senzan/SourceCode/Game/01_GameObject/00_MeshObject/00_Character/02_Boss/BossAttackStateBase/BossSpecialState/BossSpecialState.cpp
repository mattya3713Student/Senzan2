#include "BossSpecialState.h"
#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include <DirectXMath.h>

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
	, m_AttackTimer(3.0f)
	, m_UpSpeed(0.15f)
	, m_TargetDirection(0.0f, 0.0f, 0.0f)
	, m_MaxTrackingAngle(3.0f)
	, m_AttackMoveSpeed(1.5f)
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
	m_pOwner->SetAnimSpeed(0.01);
}

void BossSpecialState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	switch (m_List)
	{
	case BossSpecialState::enSpecial::None:
		m_Velocity = {};
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

void BossSpecialState::LateUpdate() {}
void BossSpecialState::Draw() {}

void BossSpecialState::Exit()
{
	m_GroundedFrag = true;
	m_SpecialFrag = false;
	m_AttackTimer = 0.0f;
}

void BossSpecialState::ParryTime()
{
	switch (m_Flinch)
	{
	case BossSpecialState::enFlinch::none:
		//ひるんだ時のアニメーションの再生へ入る.
		m_Flinch = enFlinch::Flinch;
		break;
	case BossSpecialState::enFlinch::Flinch:
		//アニメーションの再生.
		m_pOwner->SetAnimSpeed(0.03);
		m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
		//アニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
		{
			m_pOwner->SetAnimSpeed(0.01);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
			//怯み中のコードに入る.
			m_Flinch = enFlinch::FlinchTimer;
		}
		break;
	case BossSpecialState::enFlinch::FlinchTimer:
		//怯み中のアニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
		{
			m_pOwner->SetAnimSpeed(0.03);
			m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
			//待機にもどるアニメーションの再生.
			m_Flinch = enFlinch::FlinchToIdol;
		}
		break;
	case BossSpecialState::enFlinch::FlinchToIdol:
		//待機へ戻るアニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchToIdol))
		{
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		}
		break;
	default:
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
		m_pOwner->SetAnimSpeed(0.03);
		m_pOwner->ChangeAnim(Boss::enBossAnim::Special_0);
		m_List = enSpecial::Jump;
	}
}

void BossSpecialState::JumpTime()
{
	if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0))
	{
		m_DistanceTraveled = 0.0f;

		//座標を空中にワープ
		XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
		CurrentPos.y = 10.0f;
		m_pOwner->SetPosition(CurrentPos);

		XMFLOAT3 PlayerPos = m_pOwner->GetTargetPos();
		XMVECTOR bPos = XMLoadFloat3(&CurrentPos);
		XMVECTOR pPos = XMLoadFloat3(&PlayerPos);

		XMVECTOR targetVec = XMVectorSubtract(pPos, bPos);
		targetVec = XMVector3Normalize(targetVec);
		DirectX::XMStoreFloat3(&m_TargetDirection, targetVec);

		m_pOwner->SetAnimSpeed(0.01);
		m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
		m_List = enSpecial::Attack;
	}
}

void BossSpecialState::BossAttack()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();
	const float floorY = 0.0f;
	const float PlayerYOffset = 1.0f;
	const float Tolerance = 2.0f;
	const float One = 1.0f;
	const float MinusOne = -1.0f;

	XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
	XMFLOAT3 TargetPos = m_pOwner->GetTargetPos();
	TargetPos.y += PlayerYOffset;

	XMVECTOR BossPosVec = XMLoadFloat3(&CurrentPos);
	XMVECTOR PlayerPosVec = XMLoadFloat3(&TargetPos);

	//終了判定
	float Distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(PlayerPosVec, BossPosVec)));
	//if (Distance <= Tolerance || CurrentPos.y <= floorY + 0.1f)
	if (CurrentPos.y <= floorY + 0.1f)
	{
		CurrentPos.y = floorY;
		m_pOwner->SetPosition(CurrentPos);
		m_Timer = 0.0f;
		m_pOwner->SetAnimSpeed(0.03);
		m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
		m_List = enSpecial::CoolTime;
		return;
	}

	//追尾（ホーミング）計算
	XMVECTOR CurrentDir = XMLoadFloat3(&m_TargetDirection);
	XMVECTOR finalMoveDir;

	float trackingThreshold = 15.0f;

	if (Distance > trackingThreshold)
	{
		//プレイヤーへの最新方向
		XMVECTOR ToPlayerDir = XMVector3Normalize(XMVectorSubtract(PlayerPosVec, BossPosVec));

		//徐々にターゲット方向へ近づける
		float lerpFactor = 1.0f * deltaTime;
		XMVECTOR TargetStepDir = XMVectorLerp(CurrentDir, ToPlayerDir, lerpFactor);
		TargetStepDir = XMVector3Normalize(TargetStepDir);

		//角度制限の適用
		XMVECTOR DotProduct = XMVector3Dot(CurrentDir, TargetStepDir);
		float cosTheta = XMVectorGetX(DotProduct);
		float AngleRad = acosf(XMMin(One, XMMax(MinusOne, cosTheta)));

		float maxTurnRadians = XMConvertToRadians(m_MaxTrackingAngle) * deltaTime;

		if (AngleRad > maxTurnRadians)
		{
			XMVECTOR rotationAxis = XMVector3Cross(CurrentDir, TargetStepDir);
			if (XMVector3LengthSq(rotationAxis).m128_f32[0] > 0.0001f)
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
		// 近づいたら追尾を更新せず、真っ直ぐ進む
		finalMoveDir = CurrentDir;
	}

	finalMoveDir = XMVector3Normalize(finalMoveDir);
	XMStoreFloat3(&m_TargetDirection, finalMoveDir);

	//移動実行
	XMVECTOR moveVector = XMVectorScale(finalMoveDir, m_AttackMoveSpeed * deltaTime);
	XMVECTOR newBossPosVec = XMVectorAdd(BossPosVec, moveVector);

	XMFLOAT3 newBossPos;
	XMStoreFloat3(&newBossPos, newBossPosVec);
	if (newBossPos.y < floorY) newBossPos.y = floorY;

	m_pOwner->SetPosition(newBossPos);
	m_DistanceTraveled += m_AttackMoveSpeed * deltaTime;
}