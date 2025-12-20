#include "BossMoveState.h"
#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSpecialState/BossSpecialState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossLaserState/BossLaserState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossShoutState/BossShoutState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossStompState/BossStompState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include <algorithm>
#include <random>

static constexpr double Move_Run_AnimSpeed = 0.07;

BossMoveState::BossMoveState(Boss* owner)
	: StateBase<Boss>(owner)
	, m_RotationAngle(0.0f)
	, m_RotationSpeed(0.0015f)
	, m_rotationDirection(1.0f)
	, m_AnimNo()
	, m_AnimTimer()
	, m_BonePos()
	, m_InitBossPos()
	, m_Phase(MovePhase::Start) // 走り出しから開始
{
}

BossMoveState::~BossMoveState()
{
}

void BossMoveState::Enter()
{
	m_Timer = 0.0f;
	m_RotationAngle = 0.0f;
	m_rotationDirection = 1.0f;
	m_Phase = MovePhase::Start;

	// 最初のアニメーション設定
	m_pOwner->SetAnimSpeed(Move_Run_AnimSpeed);
	m_pOwner->ChangeAnim(Boss::enBossAnim::IdolToRun);
}

void BossMoveState::Update()
{
	using namespace DirectX;

	float delta = Time::GetInstance().GetDeltaTime();
	m_Timer += delta;

	// 1. 座標情報の取得
	XMVECTOR vBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMFLOAT3 playerPosF = m_pOwner->GetTargetPos();
	XMVECTOR vTarget = XMLoadFloat3(&playerPosF);

	// プレイヤーへの方向ベクトルと距離
	XMVECTOR vToPlayer = XMVectorSubtract(vTarget, vBossPos);
	vToPlayer = XMVectorSetY(vToPlayer, 0.0f); // 高さ無視
	float distanceToPlayer = XMVectorGetX(XMVector3Length(vToPlayer));

	// 判定基準
	constexpr float STRAFE_RANGE = 20.0f; // 左右移動に切り替える距離

	// --------------------------------------------------------
	// 2. フェーズ別移動・アニメーション処理
	// --------------------------------------------------------
	switch (m_Phase)
	{
	case MovePhase::Start: // 【走り出し】
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::IdolToRun))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::Run);
			m_Phase = MovePhase::Run;
		}
		break;

	case MovePhase::Run: // 【まっすぐ走る】
	{
		constexpr float APPROACH_SPEED = 0.15f;
		XMVECTOR vMoveDir = XMVector3Normalize(vToPlayer);
		XMVECTOR vNewPos = XMVectorAdd(vBossPos, XMVectorScale(vMoveDir, APPROACH_SPEED * delta));

		XMFLOAT3 newPosF;
		XMStoreFloat3(&newPosF, vNewPos);
		m_pOwner->SetPosition(newPosF);
		vBossPos = vNewPos;
	}

	if (distanceToPlayer <= STRAFE_RANGE)
	{
		m_Phase = MovePhase::Stop;
		// ここで停止アニメーションを開始（必要に応じて追加）
		m_pOwner->ChangeAnim(Boss::enBossAnim::RunToIdol);
	}
	break;

	case MovePhase::Stop: // 【停止中】
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::RunToIdol))
		{
			m_Phase = MovePhase::Strafe;
			float fixedDistance = 10.0f;
			XMVECTOR vOffset = XMVectorSet(0.0f, 0.0f, fixedDistance, 0.0f);
			XMStoreFloat3(&m_InitBossPos, XMVectorSubtract(vBossPos, vOffset));
		}
		break;

	case MovePhase::Strafe: // 【左右移動】
	{
		XMVECTOR vCurrentCenter = XMLoadFloat3(&m_InitBossPos);
		XMVECTOR vDirToCenterTarget = XMVectorSubtract(vTarget, vCurrentCenter);
		vDirToCenterTarget = XMVectorSetY(vDirToCenterTarget, 0.0f);

		float distToCenter = XMVectorGetX(XMVector3Length(vDirToCenterTarget));
		constexpr float CENTER_MOVE_SPEED = 0.15f;

		if (distToCenter > 0.001f)
		{
			XMVECTOR vMove = XMVectorScale(XMVector3Normalize(vDirToCenterTarget), std::min(CENTER_MOVE_SPEED * delta, distToCenter));
			vCurrentCenter = XMVectorAdd(vCurrentCenter, vMove);
			XMStoreFloat3(&m_InitBossPos, vCurrentCenter);
		}

		m_RotationAngle += m_RotationSpeed * delta * m_rotationDirection;
		const float MAX_ANGLE = XM_PIDIV4;
		if (fabsf(m_RotationAngle) > MAX_ANGLE)
		{
			m_rotationDirection *= -1.0f;
		}

		constexpr float orbitRadius = 10.0f;
		XMVECTOR initialOffset = XMVectorSet(0.0f, 0.0f, orbitRadius, 0.0f);
		XMMATRIX rotMatrix = XMMatrixRotationY(m_RotationAngle);
		XMVECTOR vFinalOffset = XMVector3Transform(initialOffset, rotMatrix);
		XMVECTOR vFinalPos = XMVectorAdd(vCurrentCenter, vFinalOffset);

		XMFLOAT3 finalPosF;
		XMStoreFloat3(&finalPosF, vFinalPos);
		m_pOwner->SetPosition(finalPosF);

		m_pOwner->SetAnimSpeed(Move_Run_AnimSpeed);
		if (m_rotationDirection > 0)
			m_pOwner->ChangeAnim(Boss::enBossAnim::LeftMove);
		else
			m_pOwner->ChangeAnim(Boss::enBossAnim::RightMove);
	}
	break;
	}

	// 常にプレイヤーを向く
	XMVECTOR vFinalBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMVECTOR vLookAt = XMVectorSubtract(vTarget, vFinalBossPos);
	float dx = XMVectorGetX(vLookAt);
	float dz = XMVectorGetZ(vLookAt);
	float angle = atan2f(dx, dz) + DirectX::XM_PI;
	m_pOwner->SetRotationY(angle);

	// --------------------------------------------------------
	// 4. 攻撃判定 (フェーズに関わらず、時間経過のみで判定)
	// --------------------------------------------------------
	constexpr float AttackDelay = 270.0f;
	if (m_Timer >= AttackDelay)
	{
		float dist = XMVectorGetX(XMVector3Length(vLookAt));
		std::vector<std::function<std::unique_ptr<StateBase<Boss>>()>> candidates;

		// 距離に応じた攻撃抽選
		if (dist < 15.0f) {
			candidates = {
				[this]() { return std::make_unique<BossSlashState>(m_pOwner); },
				[this]() { return std::make_unique<BossChargeState>(m_pOwner); },
				[this]() { return std::make_unique<BossStompState>(m_pOwner); }
			};
		}
		else if (dist < 40.0f) {
			candidates = {
				[this]() { return std::make_unique<BossThrowingState>(m_pOwner); },
				[this]() { return std::make_unique<BossShoutState>(m_pOwner); }
			};
		}
		else {
			candidates = {
				[this]() { return std::make_unique<BossSpecialState>(m_pOwner); },
				[this]() { return std::make_unique<BossLaserState>(m_pOwner); }
			};
		}

		if (!candidates.empty())
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, static_cast<int>(candidates.size()) - 1);

			// ステートマシンを切り替えて即座にUpdateを終了
			m_pOwner->GetStateMachine()->ChangeState(std::move(candidates[dis(gen)]()));
			return;
		}
	}
}

void BossMoveState::LateUpdate()
{
}

void BossMoveState::Draw() 
{
}

void BossMoveState::Exit()
{
}

void BossMoveState::SetInitialAngle(float angle)
{
	m_RotationAngle = angle;
}