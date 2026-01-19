#include "BossMoveState.h"
#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossJumpOnlState/BossJumpOnlState.h"
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

	// === ImGui Debug Window ===
	ImGui::Begin(IMGUI_JP("Boss Attack Debug"));
	
	ImGui::Text(IMGUI_JP("=== Distance Settings ==="));
	ImGui::SliderFloat(IMGUI_JP("Near Range"), &s_NearRange, 5.0f, 50.0f);
	ImGui::SliderFloat(IMGUI_JP("Mid Range"), &s_MidRange, 20.0f, 80.0f);
	ImGui::SliderFloat(IMGUI_JP("Attack Delay(sec)"), &s_AttackDelay, 0.1f, 5.0f);
	
	ImGui::Separator();
	ImGui::Text(IMGUI_JP("=== Attack ON/OFF ==="));
	ImGui::Checkbox(IMGUI_JP("Slash (Near)"), &s_EnableSlash);
	ImGui::Checkbox(IMGUI_JP("Stomp (Near/Far)"), &s_EnableStomp);
	ImGui::Checkbox(IMGUI_JP("Charge (Mid)"), &s_EnableCharge);
	ImGui::Checkbox(IMGUI_JP("Shout (Mid)"), &s_EnableShout);
	ImGui::Checkbox(IMGUI_JP("Throwing (Far)"), &s_EnableThrowing);
	
	ImGui::Separator();
	ImGui::Text(IMGUI_JP("=== Force Attack ==="));
	const char* attackNames[] = { "Random", "Slash", "Stomp", "Charge", "Shout", "Throwing" };
	ImGui::Combo(IMGUI_JP("Force Attack"), &s_ForceAttackIndex, attackNames, IM_ARRAYSIZE(attackNames));
	s_ForceAttackIndex -= 1; // -1 = Random, 0-4 = Each attack

	ImGui::Separator();
	XMFLOAT3 debugTargetPos = m_pOwner->GetTargetPos();
	XMFLOAT3 debugBossPos = m_pOwner->GetPosition();
	float debugDist = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&debugTargetPos), XMLoadFloat3(&debugBossPos))));
	ImGui::Text(IMGUI_JP("Current Distance: %.2f"), debugDist);
	if (debugDist < s_NearRange) ImGui::TextColored(ImVec4(1,0,0,1), "-> Near");
	else if (debugDist < s_MidRange) ImGui::TextColored(ImVec4(1,1,0,1), "-> Mid");
	else ImGui::TextColored(ImVec4(0,1,0,1), "-> Far");
	
	ImGui::End();

	float delta = Time::GetInstance().GetDeltaTime();

	// Attack timer
	m_Timer += delta;

	// 1. Get position info
	XMVECTOR vBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMFLOAT3 playerPosF = m_pOwner->GetTargetPos();
	XMVECTOR vTarget = XMLoadFloat3(&playerPosF);

	// Direction vector and distance to player
	XMVECTOR vToPlayer = XMVectorSubtract(vTarget, vBossPos);
	vToPlayer = XMVectorSetY(vToPlayer, 0.0f);
	float distanceToPlayer = XMVectorGetX(XMVector3Length(vToPlayer));

	constexpr float STRAFE_RANGE = 20.0f;

	// --------------------------------------------------------
	// 2. Phase-based movement and animation
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
		m_RotationAngle += static_cast<float>(m_RotationSpeed) * delta * m_rotationDirection;
		const float MAX_SWAY = XM_PIDIV4;
		if (fabsf(m_RotationAngle) > MAX_SWAY)
		{
			m_rotationDirection *= -1.0f;
			m_RotationAngle = std::clamp(m_RotationAngle, -MAX_SWAY, MAX_SWAY);
		}

		float finalAngle = m_BaseAngle + m_RotationAngle;
		XMVECTOR vOffset = XMVectorSet(
			sinf(finalAngle) * STRAFE_RANGE,
			0.0f,
			cosf(finalAngle) * STRAFE_RANGE,
			0.0f
		);
		XMVECTOR vIdealPos = XMVectorAdd(vTarget, vOffset);

		constexpr float TRACKING_DELAY = 0.7f;

		XMVECTOR vCurrentPos = XMLoadFloat3(&m_pOwner->GetPosition());
		float lerpFactor = TRACKING_DELAY * delta;
		if (lerpFactor > 1.0f) lerpFactor = 1.0f;

		XMVECTOR vNextPos = XMVectorLerp(vCurrentPos, vIdealPos, lerpFactor);

		XMFLOAT3 finalPosF;
		XMStoreFloat3(&finalPosF, vNextPos);
		finalPosF.y = m_pOwner->GetPosition().y;
		m_pOwner->SetPosition(finalPosF);

		m_pOwner->SetAnimSpeed(3.0);
		if (m_rotationDirection > 0)
			m_pOwner->ChangeAnim(Boss::enBossAnim::LeftMove);
		else
			m_pOwner->ChangeAnim(Boss::enBossAnim::RightMove);
	}
	break;
	}

	// --------------------------------------------------------
	// 3. Always face player
	// --------------------------------------------------------
	XMVECTOR vFinalBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMVECTOR vLookAt = XMVectorSubtract(vTarget, vFinalBossPos);
	float dx = XMVectorGetX(vLookAt);
	float dz = XMVectorGetZ(vLookAt);
	float angle = atan2f(dx, dz) + DirectX::XM_PI;
	m_pOwner->SetRotationY(angle);

	// --------------------------------------------------------
	// 4. Attack selection (distance-based)
	// --------------------------------------------------------
	if (m_Timer >= s_AttackDelay)
	{
		float dist = XMVectorGetX(XMVector3Length(vLookAt));
		std::vector<std::function<std::unique_ptr<StateBase<Boss>>()>> candidates;

		// Force attack mode
		if (s_ForceAttackIndex >= 0 && s_ForceAttackIndex <= 4)
		{
			switch (s_ForceAttackIndex)
			{
			case 0: if (s_EnableSlash) candidates = { [this]() { return std::make_unique<BossSlashState>(m_pOwner); } }; break;
			case 1: if (s_EnableStomp) candidates = { [this]() { return std::make_unique<BossStompState>(m_pOwner); } }; break;
			case 2: if (s_EnableCharge) candidates = { [this]() { return std::make_unique<BossChargeState>(m_pOwner); } }; break;
			case 3: if (s_EnableShout) candidates = { [this]() { return std::make_unique<BossShoutState>(m_pOwner); } }; break;
			case 4: if (s_EnableThrowing) candidates = { [this]() { return std::make_unique<BossThrowingState>(m_pOwner); } }; break;
			}
		}
		else
		{
			// Near: Slash or Stomp
			if (dist < s_NearRange) {
				if (s_EnableSlash) candidates.push_back([this]() { return std::make_unique<BossSlashState>(m_pOwner); });
				if (s_EnableStomp) candidates.push_back([this]() { return std::make_unique<BossStompState>(m_pOwner); });
			}
			// Mid: Charge or Shout
			else if (dist < s_MidRange) {
				if (s_EnableCharge) candidates.push_back([this]() { return std::make_unique<BossChargeState>(m_pOwner); });
				if (s_EnableShout) candidates.push_back([this]() { return std::make_unique<BossShoutState>(m_pOwner); });
			}
			// Far: Throwing or Stomp
			else {
				if (s_EnableThrowing) candidates.push_back([this]() { return std::make_unique<BossThrowingState>(m_pOwner); });
				if (s_EnableStomp) candidates.push_back([this]() { return std::make_unique<BossStompState>(m_pOwner); });
			}
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
