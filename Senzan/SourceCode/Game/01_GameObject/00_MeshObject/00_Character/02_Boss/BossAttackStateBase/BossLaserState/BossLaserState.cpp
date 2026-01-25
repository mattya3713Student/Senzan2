#include "BossLaserState.h"

#include "Game/04_Time/Time.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

BossLaserState::BossLaserState(Boss* owner)
    : BossAttackStateBase(owner)
    , m_State(enLaser::None)
    , m_ChargeDuration(1.0f)
    , m_ChargeElapsed(0.0f)
    , m_FireDuration(0.6f)
    , m_FireElapsed(0.0f)
    , m_LaserDamage(10.0f)
    , m_LaserRadius(5.0f)
    , m_LaserRange(200.0f)
    , m_EffectPlayed(false)
{
    try { LoadSettings(); } catch (...) {}
}

BossLaserState::~BossLaserState() {}

void BossLaserState::Enter()
{
    try { LoadSettings(); } catch (...) {}
    BossAttackStateBase::Enter();

    m_State = enLaser::Charge;
    m_ChargeElapsed = 0.0f;
    m_FireElapsed = 0.0f;
    m_EffectPlayed = false;

    // face player and play charge anim
    FacePlayerInstantYaw();
    m_pOwner->SetAnimSpeed(1.5);
    m_pOwner->ChangeAnim(Boss::enBossAnim::LaserCharge);
}

void BossLaserState::Update()
{
    BossAttackStateBase::Update();
    float dt = m_pOwner->GetDelta();

    switch (m_State) {
    case enLaser::Charge:
        m_ChargeElapsed += dt;
        if (m_ChargeElapsed >= m_ChargeDuration || m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserCharge)) {
            m_State = enLaser::Fire;
            m_pOwner->SetAnimSpeed(1.0);
            m_pOwner->ChangeAnim(Boss::enBossAnim::Laser);
            // enable shout/laser collider if available
            if (auto* col = m_pOwner->GetShoutCollider()) {
                col->SetActive(true);
                col->SetAttackAmount(m_LaserDamage);
                col->SetRadius(m_LaserRadius);
            }
        }
        break;
    case enLaser::Fire:
        m_FireElapsed += dt;
        if (!m_EffectPlayed) {
            if (m_pOwner) {
                m_pOwner->PlayEffect("Laser", DirectX::XMFLOAT3{0,0,0}, 1.0f);
            }
            m_EffectPlayed = true;
        }
        // expand collider over duration
        if (auto* col = m_pOwner->GetShoutCollider()) {
            if (col->GetActive()) {
                float t = (m_FireElapsed / (m_FireDuration > 0.0f ? m_FireDuration : 1.0f));
                if (t > 1.0f) t = 1.0f;
                float radius = m_LaserRadius * t;
                col->SetRadius(radius);
            }
        }
        if (m_FireElapsed >= m_FireDuration || m_pOwner->IsAnimEnd(Boss::enBossAnim::Laser)) {
            if (auto* col = m_pOwner->GetShoutCollider()) {
                col->SetActive(false);
            }
            m_pOwner->SetAnimSpeed(1.5);
            m_pOwner->ChangeAnim(Boss::enBossAnim::LaserEnd);
            m_State = enLaser::Cool;
        }
        break;
    case enLaser::Cool:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserEnd)) {
            m_State = enLaser::Trans;
        }
        break;
    case enLaser::Trans:
        m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
        break;
    default:
        break;
    }
}

void BossLaserState::LateUpdate() {}
void BossLaserState::Draw() {}

void BossLaserState::Exit()
{
    try { SaveSettings(); } catch (...) {}
    if (auto* col = m_pOwner->GetShoutCollider()) {
        col->SetActive(false);
    }
}

std::pair<Boss::enBossAnim, float> BossLaserState::GetParryAnimPair()
{
    return std::pair(Boss::enBossAnim::Laser, 0.5f);
}

void BossLaserState::DrawImGui()
{
#if _DEBUG
    ImGui::Begin(IMGUI_JP("BossLaser State"));
    CImGuiManager::Slider<float>(IMGUI_JP("チャージ時間"), m_ChargeDuration, 0.0f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("発射時間"), m_FireDuration, 0.0f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("ダメージ"), m_LaserDamage, 0.0f, 100.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("範囲"), m_LaserRadius, 0.0f, 200.0f, true);
    BossAttackStateBase::DrawImGui();
    if (ImGui::Button(IMGUI_JP("Load"))) { try { LoadSettings(); } catch (...) {} }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("Save"))) { try { SaveSettings(); } catch (...) {} }
    ImGui::End();
#endif
}

void BossLaserState::LoadSettings()
{
    BossAttackStateBase::LoadSettings();
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("ChargeDuration")) m_ChargeDuration = j["ChargeDuration"].get<float>();
    if (j.contains("FireDuration")) m_FireDuration = j["FireDuration"].get<float>();
    if (j.contains("LaserDamage")) m_LaserDamage = j["LaserDamage"].get<float>();
    if (j.contains("LaserRadius")) m_LaserRadius = j["LaserRadius"].get<float>();
    if (j.contains("LaserRange")) m_LaserRange = j["LaserRange"].get<float>();
}

void BossLaserState::SaveSettings() const
{
    json j = SerializeSettings();
    j["ChargeDuration"] = m_ChargeDuration;
    j["FireDuration"] = m_FireDuration;
    j["LaserDamage"] = m_LaserDamage;
    j["LaserRadius"] = m_LaserRadius;
    j["LaserRange"] = m_LaserRange;
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    FileManager::JsonSave(filePath, j);
}
