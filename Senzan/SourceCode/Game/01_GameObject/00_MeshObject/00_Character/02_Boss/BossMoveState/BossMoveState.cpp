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

static constexpr double Move_Run_AnimSpeed = 5.0;

BossMoveState::BossMoveState(Boss* owner)
	: StateBase<Boss>(owner)
	, m_RotationAngle(0.0f)
	, m_RotationSpeed(0.1f) 
	, m_rotationDirection(1.0f)
	, m_AnimNo()
	, m_AnimTimer()
	, m_BonePos()
	, m_InitBossPos()
	, m_Phase(MovePhase::Start)
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

	m_pOwner->SetAnimSpeed(Move_Run_AnimSpeed);
	m_pOwner->ChangeAnim(Boss::enBossAnim::IdolToRun);
}

void BossMoveState::Update()
{
	using namespace DirectX;

	float delta = Time::GetInstance().GetDeltaTime();

	// 攻撃時間の加算
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
	constexpr float STRAFE_RANGE = 20.0f;

	// --------------------------------------------------------
	// 2. フェーズ別移動・アニメーション処理
	// --------------------------------------------------------
	switch (m_Phase)
	{
	case MovePhase::Start:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::IdolToRun))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::Run);
			m_Phase = MovePhase::Run;
		}
		break;

	case MovePhase::Run:
	{
		constexpr float APPROACH_SPEED = 10.0f;
		XMVECTOR vMoveDir = XMVector3Normalize(vToPlayer);
		XMVECTOR vNewPos = XMVectorAdd(vBossPos, XMVectorScale(vMoveDir, APPROACH_SPEED * delta));

		XMFLOAT3 newPosF;
		XMStoreFloat3(&newPosF, vNewPos);
		m_pOwner->SetPosition(newPosF);
		vBossPos = vNewPos;

		if (distanceToPlayer <= STRAFE_RANGE)
		{
			m_Phase = MovePhase::Stop;
			m_pOwner->ChangeAnim(Boss::enBossAnim::RunToIdol);
		}
	}
	break;

	case MovePhase::Stop:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::RunToIdol))
		{
			m_Phase = MovePhase::Strafe;
			XMVECTOR vDirFromPlayer = XMVectorSubtract(vBossPos, vTarget);
			m_BaseAngle = atan2f(XMVectorGetX(vDirFromPlayer), XMVectorGetZ(vDirFromPlayer));
			m_RotationAngle = 0.0f;
		}
		break;

	case MovePhase::Strafe:
	{
		// --- 1. ボス自身の理想の角度更新 ---
		m_RotationAngle += m_RotationSpeed * delta * m_rotationDirection;
		const float MAX_SWAY = XM_PIDIV4;
		if (fabsf(m_RotationAngle) > MAX_SWAY)
		{
			m_rotationDirection *= -1.0f;
			m_RotationAngle = std::clamp(m_RotationAngle, -MAX_SWAY, MAX_SWAY);
		}

		// --- 2. 理想の座標計算（プレイヤー位置に即座に同期した地点） ---
		float finalAngle = m_BaseAngle + m_RotationAngle;
		XMVECTOR vOffset = XMVectorSet(
			sinf(finalAngle) * STRAFE_RANGE,
			0.0f,
			cosf(finalAngle) * STRAFE_RANGE,
			0.0f
		);
		XMVECTOR vIdealPos = XMVectorAdd(vTarget, vOffset);

		// --- 3. 【追尾を遅らせる】補間処理 ---
		// 【調整】1.5f -> 0.7f (プレイヤーの動きにわざと遅れてついていく)
		constexpr float TRACKING_DELAY = 0.7f;

		XMVECTOR vCurrentPos = XMLoadFloat3(&m_pOwner->GetPosition());
		float lerpFactor = TRACKING_DELAY * delta;
		if (lerpFactor > 1.0f) lerpFactor = 1.0f;

		XMVECTOR vNextPos = XMVectorLerp(vCurrentPos, vIdealPos, lerpFactor);

		// 座標更新（高さ維持）
		XMFLOAT3 finalPosF;
		XMStoreFloat3(&finalPosF, vNextPos);
		finalPosF.y = m_pOwner->GetPosition().y;
		m_pOwner->SetPosition(finalPosF);

		// --- 4. アニメーション速度の調整 ---
		// 【調整】20.0 -> 12.0 (移動が遅いので足踏みもゆっくりにする)
		m_pOwner->SetAnimSpeed(3.0);
		if (m_rotationDirection > 0)
			m_pOwner->ChangeAnim(Boss::enBossAnim::LeftMove);
		else
			m_pOwner->ChangeAnim(Boss::enBossAnim::RightMove);
	}
	break;
	}

	// --------------------------------------------------------
	// 3. 常にプレイヤーを向く
	// --------------------------------------------------------
	XMVECTOR vFinalBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMVECTOR vLookAt = XMVectorSubtract(vTarget, vFinalBossPos);
	float dx = XMVectorGetX(vLookAt);
	float dz = XMVectorGetZ(vLookAt);
	float angle = atan2f(dx, dz) + DirectX::XM_PI;
	m_pOwner->SetRotationY(angle);

	// --------------------------------------------------------
	// 4. 攻撃判定：距離に基づいて攻撃を選択
	// --------------------------------------------------------
	constexpr float AttackDelay = 2.0f;  // クールダウン時間を2秒に延長
	if (m_Timer >= AttackDelay)
	{
		float dist = XMVectorGetX(XMVector3Length(vLookAt));
		std::vector<std::function<std::unique_ptr<StateBase<Boss>>()>> candidates;

		// 近距離攻撃 (15m以内)
		if (dist < 15.0f) {
			candidates = {
				[this]() { return std::make_unique<BossSlashState>(m_pOwner); },
				[this]() { return std::make_unique<BossChargeState>(m_pOwner); },
				[this]() { return std::make_unique<BossStompState>(m_pOwner); },
				[this]() { return std::make_unique<BossShoutState>(m_pOwner); }
			};
		}
		// 中距離攻撃 (15m～40m)
		else if (dist < 40.0f) {
			candidates = {
				[this]() { return std::make_unique<BossThrowingState>(m_pOwner); },
				[this]() { return std::make_unique<BossShoutState>(m_pOwner); },
				[this]() { return std::make_unique<BossChargeSlashState>(m_pOwner); }
			};
		}
		// 遠距離攻撃 (40m以上)
		else {
			candidates = {
				[this]() { return std::make_unique<BossSpecialState>(m_pOwner); },
				[this]() { return std::make_unique<BossThrowingState>(m_pOwner); }
			};
		}

		if (!candidates.empty())
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, static_cast<int>(candidates.size()) - 1);

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
