#include "Combat.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "Game/05_InputDevice/VirtualPad.h"

namespace PlayerState {
Combat::Combat(Player* owner)
	: Action		(owner)
	, m_MaxTime		()
	, m_currentTime	()
{
}

Combat::~Combat()
{
}

void Combat::Enter()
{
	Action::Enter();
	m_currentTime = 0.f;

	// clear collider windows by default
	m_ColliderWindows.clear();
	m_isAttackColliderEnabled = false;
	m_ActiveWindowCount = 0;
}

void Combat::Update()
{
	Action::Update();
}

void Combat::LateUpdate()
{
	Action::LateUpdate();
}

void Combat::Draw()
{
	Action::Draw();
}

void Combat::Exit()
{
	Action::Exit();
}

void Combat::ClearColliderWindows()
{
	m_ColliderWindows.clear();
	m_ActiveWindowCount = 0;
	m_isAttackColliderEnabled = false;
}

void Combat::AddColliderWindow(float start, float duration)
{
	m_ColliderWindows.push_back({start, duration, false, false});
}

void Combat::ProcessColliderWindows(float currentTime)
{
	// count-based activation: increment when window enters, decrement when it exits
	for (auto &w : m_ColliderWindows)
	{
		// 有効化トリガー: start を越えたら一度だけカウント増加
		if (!w.activated && currentTime >= w.start)
		{
			w.activated = true;
			w.deactivated = false;
			++m_ActiveWindowCount;
			if (m_ActiveWindowCount == 1) {
				m_isAttackColliderEnabled = true;
				m_pOwner->SetAttackColliderActive(true);
			}
		}

		// 無効化トリガー: start+duration を越えたら一度だけカウント減少
		if (!w.deactivated && currentTime >= (w.start + w.duration))
		{
			w.deactivated = true;
			if (w.activated) {
				w.activated = false;
				--m_ActiveWindowCount;
				if (m_ActiveWindowCount <= 0) {
					m_ActiveWindowCount = 0;
					m_isAttackColliderEnabled = false;
					m_pOwner->SetAttackColliderActive(false);
				}
			}
		}
	}
}

void Combat::RenderColliderWindowsUI(const char* title)
{
	ImGui::Separator();
	ImGui::Text("%s", title);

	for (size_t i = 0; i < m_ColliderWindows.size(); ++i)
	{
		auto &w = m_ColliderWindows[i];
		ImGui::PushID((int)i);
		ImGui::DragFloat("開始時刻 (秒)", &w.start, 0.01f, 0.0f, m_MaxTime);
		ImGui::SameLine();
		ImGui::DragFloat("継続時間 (秒)", &w.duration, 0.01f, 0.0f, m_MaxTime);
		ImGui::SameLine();
		if (ImGui::Button("削除")) { m_ColliderWindows.erase(m_ColliderWindows.begin() + i); ImGui::PopID(); break; }
		ImGui::PopID();
	}

	if (ImGui::Button("ウィンドウを追加")) { m_ColliderWindows.push_back({0.0f, 0.1f, false, false}); }
}

} // PlayerState.

