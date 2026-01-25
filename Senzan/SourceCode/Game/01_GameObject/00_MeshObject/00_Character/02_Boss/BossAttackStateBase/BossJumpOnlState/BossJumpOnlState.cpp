#include "BossJumpOnlState.h"
#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

BossJumpOnlState::BossJumpOnlState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_List(enSpecial::None)
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

BossJumpOnlState::~BossJumpOnlState()
{
}

void BossJumpOnlState::Enter()
{
	m_Timer = 0.0f;
	m_Velocity = {};
	m_DistanceTraveled = 0.0f;
	m_GroundedFrag = true;
	m_pOwner->SetAnimSpeed(2.0);
	// 最初の待機モーションへ（必要に応じて追加）
	m_pOwner->ChangeAnim(Boss::enBossAnim::Idol);
}

void BossJumpOnlState::Update()
{
	float deltaTime = m_pOwner->GetDelta();

	switch (m_List)
	{
	case BossJumpOnlState::enSpecial::None:
		m_Velocity = {};
		// 当たり判定を有効化.
		m_List = enSpecial::Charge;
		break;

	case BossJumpOnlState::enSpecial::Charge:
		ChargeTime();
		break;

	case BossJumpOnlState::enSpecial::Jump:
		JumpTime();
		break;

	case BossJumpOnlState::enSpecial::Attack:
		BossAttack();
		break;

	case BossJumpOnlState::enSpecial::CoolTime:
		m_Timer += deltaTime;
		// SpecialToIdolアニメーションが終了したら遷移
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SpecialToIdol))
		{
			m_List = enSpecial::Trans;
		}
		break;

	case BossJumpOnlState::enSpecial::Trans:
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;

	default:
		break;
	}
}

void BossJumpOnlState::LateUpdate()
{
}

void BossJumpOnlState::Draw()
{
}

void BossJumpOnlState::Exit()
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
}

std::pair<Boss::enBossAnim, float> BossJumpOnlState::GetParryAnimPair()
{
    return std::pair(Boss::enBossAnim::none, 0.0f);
}

void BossJumpOnlState::ChargeTime()
{
	float deltaTime = m_pOwner->GetDelta();
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

void BossJumpOnlState::JumpTime()
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

void BossJumpOnlState::BossAttack()
{
	float deltaTime = m_pOwner->GetDelta();
	const float floorY = 0.0f;
	const float PlayerYOffset = 1.0f;
	const float One = 1.0f;
	const float MinusOne = -1.0f;

    DirectX::XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
    DirectX::XMFLOAT3 TargetPosF = m_pOwner->GetTargetPos();
	TargetPosF.y += PlayerYOffset;

    DirectX::XMVECTOR BossPosVec = XMLoadFloat3(&CurrentPos);
    DirectX::XMVECTOR PlayerPosVec = XMLoadFloat3(&TargetPosF);

	// 距離判定と高さ判定による終了処理
	float DistanceToPlayer = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(PlayerPosVec, BossPosVec)));

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
    DirectX::XMVECTOR CurrentDir = DirectX::XMLoadFloat3(&m_TargetDirection);
    DirectX::XMVECTOR finalMoveDir;

	float trackingThreshold = 15.0f;

	if (DistanceToPlayer > trackingThreshold)
	{
        DirectX::XMVECTOR ToPlayerDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(PlayerPosVec, BossPosVec));

		float lerpFactor = 1.0f * deltaTime;
        DirectX::XMVECTOR TargetStepDir = DirectX::XMVectorLerp(CurrentDir, ToPlayerDir, lerpFactor);
		TargetStepDir = DirectX::XMVector3Normalize(TargetStepDir);

        DirectX::XMVECTOR DotProduct = DirectX::XMVector3Dot(CurrentDir, TargetStepDir);
		float cosTheta = DirectX::XMVectorGetX(DotProduct);
		float AngleRad = acosf(std::max(MinusOne, std::min(One, cosTheta)));

		float maxTurnRadians = DirectX::XMConvertToRadians(m_MaxTrackingAngle) * deltaTime;

		if (AngleRad > maxTurnRadians)
		{
			XMVECTOR rotationAxis = DirectX::XMVector3Cross(CurrentDir, TargetStepDir);
			if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(rotationAxis)) > 0.0001f)
			{
				rotationAxis = DirectX::XMVector3Normalize(rotationAxis);
                DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, maxTurnRadians);
				finalMoveDir = DirectX::XMVector3TransformNormal(CurrentDir, rotationMatrix);
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

	finalMoveDir = DirectX::XMVector3Normalize(finalMoveDir);
	DirectX::XMStoreFloat3(&m_TargetDirection, finalMoveDir);

	// 移動実行
    DirectX::XMVECTOR moveVector = DirectX::XMVectorScale(finalMoveDir, m_AttackMoveSpeed * deltaTime);
    DirectX::XMVECTOR newBossPosVec = DirectX::XMVectorAdd(BossPosVec, moveVector);

    DirectX::XMFLOAT3 newBossPos;
    DirectX::XMStoreFloat3(&newBossPos, newBossPosVec);

	// 地面を突き抜けないように制限
	if (newBossPos.y < floorY) newBossPos.y = floorY;

	m_pOwner->SetPosition(newBossPos);
	m_DistanceTraveled += m_AttackMoveSpeed * deltaTime;
}
