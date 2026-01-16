#include "BossChargeState.h"

#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "Game//04_Time//Time.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

BossChargeState::BossChargeState(Boss* owner)
	: BossAttackStateBase		(owner)
{
}

BossChargeState::~BossChargeState()
{
}

void BossChargeState::Enter()
{
	// 当たり判定を有効化.
	m_pOwner->SetAttackColliderActive(true);
	m_pOwner->SetAnimSpeed(1.0);
	m_pOwner->ChangeAnim(Boss::enBossAnim::Charge);
}

void BossChargeState::Update()
{
	if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Charge))
	{
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossChargeSlashState>(m_pOwner));
	}
}

void BossChargeState::LateUpdate()
{
}

void BossChargeState::Draw()
{
}

void BossChargeState::Exit()
{	// 当たり判定を無効化.
	m_pOwner->SetAttackColliderActive(false);
}

void BossChargeState::DrawImGui()
{
    ImGui::Begin(IMGUI_JP("BossCharge State"));
    BossAttackStateBase::DrawImGui();
    ImGui::End();
}

void BossChargeState::LoadSettings()
{
    // 基底設定読み込み
    BossAttackStateBase::LoadSettings();
    // ソースフォルダの設定で上書きする
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    // 現在 ChargeState 固有の設定項目はなし
}

void BossChargeState::SaveSettings() const
{
    json j = SerializeSettings();
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    FileManager::JsonSave(filePath, j);
}
