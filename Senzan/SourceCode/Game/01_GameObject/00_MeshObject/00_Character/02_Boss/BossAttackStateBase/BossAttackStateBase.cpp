#include "BossAttackStateBase.h"

#include "Game/04_Time/Time.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"

#include "System/Singleton/ImGui/CImGuiManager.h"

BossAttackStateBase::BossAttackStateBase(Boss* owner)
	: StateBase<Boss>(owner)
	, m_Attacktime(0.0f)
	, m_pTransform(std::make_shared<Transform>())
{
}

void BossAttackStateBase::UpdateBaseLogic(float dt)
{
	m_StateTimer += dt;
	m_pOwner->SetAnimSpeed(m_AnimSpeed);

	for (auto& window : m_ColliderWindows)
	{
		if (!window.IsAct && m_StateTimer >= window.Start)
		{
			m_pOwner->SetColliderActiveByName(window.BoneName, true);
			window.IsAct = true;
		}
		if (window.IsAct && !window.IsEnd && m_StateTimer >= (window.Start + window.Duration))
		{
			m_pOwner->SetColliderActiveByName(window.BoneName, false);
			window.IsEnd = true;
		}
	}

	for (auto& mv : m_MovementWindows)
	{
		if (!mv.IsAct && m_StateTimer >= mv.Start)
		{
			mv.IsAct = true;
		}
		if (mv.IsAct && m_StateTimer < (mv.Start + mv.Duration))
		{
			// NOTE: forward move. Prefer a dedicated movement API if exists.
			auto pos = m_pOwner->GetPosition();
			const auto rot = m_pOwner->GetRotation();
			const float yaw = rot.y;
			const float dx = std::sinf(yaw) * (mv.Speed * dt);
			const float dz = std::cosf(yaw) * (mv.Speed * dt);
			pos.x += dx;
			pos.z += dz;
			m_pOwner->SetPosition(pos);
		}
	}
}

void BossAttackStateBase::DrawImGui()
{
	// Simple ImGui panel for editing attack parameters.
	if (!ImGui::Begin("Boss Attack State")) {
		ImGui::End();
		return;
	}

	// Damage
	CImGuiManager::Input<float>("Damage", m_Damage, true, 0.1f, 1.0f, "%.2f");

	// Width (use AttackRadius as width)
	CImGuiManager::Slider<float>("Width", m_AttackRadius, 0.0f, 20.0f, true);

	// Height
	CImGuiManager::Slider<float>("Height", m_AttackHeight, 0.0f, 10.0f, true);

	// State exit time
	CImGuiManager::Slider<float>("State Exit Time", m_ExitTime, 0.0f, 10.0f, true);

	// Charge time
	CImGuiManager::Slider<float>("Charge Time", m_ChargeTime, 0.0f, 5.0f, true);

	ImGui::Separator();
	ImGui::Text("Animation Speeds (per phase)");
	CImGuiManager::Slider<float>("Anim Speed - Charge", m_AnimSpeedCharge, 0.1f, 3.0f, true);
	CImGuiManager::Slider<float>("Anim Speed - Attack", m_AnimSpeedAttack, 0.1f, 3.0f, true);
	CImGuiManager::Slider<float>("Anim Speed - Exit", m_AnimSpeedExit, 0.1f, 3.0f, true);

	ImGui::End();
}

