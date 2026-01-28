#include "BossSpecialDamageState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

BossSpecialDamageState::BossSpecialDamageState(Boss* owner)
    : BossAttackStateBase(owner)
{
    try { LoadSettings(); } catch (...) {}
}

BossSpecialDamageState::~BossSpecialDamageState()
{
}

void BossSpecialDamageState::Enter()
{
    try { LoadSettings(); } catch (...) {}
    BossAttackStateBase::Enter();

    m_Phase = Phase::TakeDamage;
    m_PhaseTimer = 0.0f;
    m_ReplayTimer = 0.0f;

    m_pOwner->SetAnimSpeed(m_TakeDamageAnimSpeed);
    m_pOwner->ChangeAnim(Boss::enBossAnim::Hit);
}

void BossSpecialDamageState::Update()
{
    BossAttackStateBase::Update();
    float dt = m_pOwner->GetDelta();

    switch (m_Phase)
    {
    case Phase::TakeDamage:
    {
        m_PhaseTimer += dt;
        m_ReplayTimer += dt;

        if (m_TakeDamageReplayEnabled && m_ReplayTimer >= m_TakeDamageReplayTime)
        {
            m_pOwner->SetAnimSpeed(m_TakeDamageAnimSpeed);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Hit);
            m_ReplayTimer = 0.0f;
        }

        if (m_PhaseTimer >= m_TakeDamageDuration)
        {
            m_Phase = Phase::Down;
            m_PhaseTimer = 0.0f;
            m_pOwner->SetAnimSpeed(m_DownAnimSpeed);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Dead);
        }
    }
    break;

    case Phase::Down:
    {
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Dead))
        {
            m_Phase = Phase::DownContinue;
            m_PhaseTimer = 0.0f;
            m_pOwner->SetIsLoop(false);
        }
    }
    break;

    case Phase::DownContinue:
    {
        m_PhaseTimer += dt;
        if (m_PhaseTimer >= m_DownContinueDuration)
        {
            m_Phase = Phase::GetUp;
            m_PhaseTimer = 0.0f;
            m_pOwner->SetAnimSpeed(m_GetUpAnimSpeed);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Idol);
        }
    }
    break;

    case Phase::GetUp:
    {
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Idol))
        {
            m_Phase = Phase::End;
        }
    }
    break;

    case Phase::End:
    {
        m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
    }
    break;
    }
}

void BossSpecialDamageState::LateUpdate()
{
}

void BossSpecialDamageState::Draw()
{
}

void BossSpecialDamageState::Exit()
{
    try { SaveSettings(); } catch (...) {}
}

void BossSpecialDamageState::DrawImGui()
{
#if _DEBUG
    ImGui::Begin(IMGUI_JP("BossSpecialDamage State"));
    ImGui::Text(IMGUI_JP("Phase: %d"), static_cast<int>(m_Phase));
    ImGui::Text(IMGUI_JP("PhaseTimer: %.2f"), m_PhaseTimer);
    ImGui::Text(IMGUI_JP("ReplayTimer: %.2f"), m_ReplayTimer);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("--- TakeDamage ---"));
    CImGuiManager::Slider<float>(IMGUI_JP("Duration"), m_TakeDamageDuration, 0.0f, 10.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("AnimSpeed"), m_TakeDamageAnimSpeed, 0.1f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("ReplayTime"), m_TakeDamageReplayTime, 0.1f, 5.0f, true);
    ImGui::Checkbox(IMGUI_JP("ReplayEnabled"), &m_TakeDamageReplayEnabled);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("--- Down ---"));
    CImGuiManager::Slider<float>(IMGUI_JP("DownAnimSpeed"), m_DownAnimSpeed, 0.1f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("DownTransitionTime"), m_DownTransitionTime, 0.0f, 2.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("DownContinueDuration"), m_DownContinueDuration, 0.0f, 10.0f, true);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("--- GetUp ---"));
    CImGuiManager::Slider<float>(IMGUI_JP("GetUpAnimSpeed"), m_GetUpAnimSpeed, 0.1f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("GetUpTransitionTime"), m_GetUpTransitionTime, 0.0f, 2.0f, true);

    ImGui::Separator();
    if (ImGui::Button(IMGUI_JP("Load"))) { try { LoadSettings(); } catch (...) {} }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("Save"))) { try { SaveSettings(); } catch (...) {} }
    BossAttackStateBase::DrawImGui();
    ImGui::End();
#endif
}

void BossSpecialDamageState::LoadSettings()
{
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        filePath = std::filesystem::current_path() / "Data" / "Json" / "Boss" / filePath;
    }
    if (!std::filesystem::exists(filePath)) return;

    json j = FileManager::JsonLoad(filePath);
    if (j.contains("TakeDamageDuration")) m_TakeDamageDuration = j["TakeDamageDuration"].get<float>();
    if (j.contains("TakeDamageAnimSpeed")) m_TakeDamageAnimSpeed = j["TakeDamageAnimSpeed"].get<float>();
    if (j.contains("TakeDamageReplayTime")) m_TakeDamageReplayTime = j["TakeDamageReplayTime"].get<float>();
    if (j.contains("TakeDamageReplayEnabled")) m_TakeDamageReplayEnabled = j["TakeDamageReplayEnabled"].get<bool>();
    if (j.contains("DownAnimSpeed")) m_DownAnimSpeed = j["DownAnimSpeed"].get<float>();
    if (j.contains("DownTransitionTime")) m_DownTransitionTime = j["DownTransitionTime"].get<float>();
    if (j.contains("DownContinueDuration")) m_DownContinueDuration = j["DownContinueDuration"].get<float>();
    if (j.contains("GetUpAnimSpeed")) m_GetUpAnimSpeed = j["GetUpAnimSpeed"].get<float>();
    if (j.contains("GetUpTransitionTime")) m_GetUpTransitionTime = j["GetUpTransitionTime"].get<float>();
}

void BossSpecialDamageState::SaveSettings() const
{
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        auto dir = std::filesystem::current_path() / "Data" / "Json" / "Boss";
        std::filesystem::create_directories(dir);
        filePath = dir / filePath;
    }
    json j = SerializeSettings();
    j["TakeDamageDuration"] = m_TakeDamageDuration;
    j["TakeDamageAnimSpeed"] = m_TakeDamageAnimSpeed;
    j["TakeDamageReplayTime"] = m_TakeDamageReplayTime;
    j["TakeDamageReplayEnabled"] = m_TakeDamageReplayEnabled;
    j["DownAnimSpeed"] = m_DownAnimSpeed;
    j["DownTransitionTime"] = m_DownTransitionTime;
    j["DownContinueDuration"] = m_DownContinueDuration;
    j["GetUpAnimSpeed"] = m_GetUpAnimSpeed;
    j["GetUpTransitionTime"] = m_GetUpTransitionTime;
    FileManager::JsonSave(filePath, j);
}

