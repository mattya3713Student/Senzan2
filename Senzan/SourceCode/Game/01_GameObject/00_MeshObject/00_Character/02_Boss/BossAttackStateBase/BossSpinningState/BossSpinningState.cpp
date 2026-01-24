#include "BossSpinningState.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "..//04_Time/Time.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

BossSpinningState::BossSpinningState(Boss* owner)
    : BossAttackStateBase(owner)
    , m_List(enSpinning::None)
    , m_IsSpun(false)
    , m_Radius(3.0f)
    , m_RotateSpeed(5.0f)
    , m_AnimIdxChargeToIdol(0)
    , m_AnimIdxChargeAttack(0)
    , m_AnimIdxCharge(0)
{
}

BossSpinningState::~BossSpinningState()
{
}

void BossSpinningState::Enter()
{
    BossAttackStateBase::Enter();
    m_CurrentTime = 0.0f;
    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimTime(0.0);
    m_pOwner->ChangeAnim(Boss::enBossAnim::ChargeToIdol); // reuse special anim for spinning
    m_List = enSpinning::Anim;
    m_IsSpun = false;

    // 設定: 回転攻撃用の当たり判定ウィンドウを追加
    m_ColliderWindows.clear();
    ColliderWindow cw;
    cw.BoneName = "boss_Hand_R"; // 手を基準に当たり判定
    cw.Start = m_ChargeTime;      // チャージ終了後に判定を出す
    cw.Duration = m_AttackTime > 0.0f ? m_AttackTime : 0.5f; // デフォルト継続時間
    cw.Offset = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
    cw.JustTime = 0.1f;
    m_ColliderWindows.push_back(cw);

    // 追従ボーン名を設定して基底ロジックでコライダー設定を反映させる
    m_AttackBoneName = cw.BoneName;
}

void BossSpinningState::Update()
{
    BossAttackStateBase::Update();
    const float dt = Time::GetInstance().GetDeltaTime();
    UpdateBaseLogic(dt);

    switch (m_List)
    {
    case enSpinning::None:
        m_List = enSpinning::Anim;
        break;

    case enSpinning::Anim:
        // start attack when animation ends or after charge time
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_1) || m_CurrentTime >= m_ChargeTime)
        {
            m_List = enSpinning::Attack;
        }
        break;

    case enSpinning::Attack:
        if (!m_IsSpun)
        {
            // simple radial movement around boss center
            // (for now just mark spun; movement handled elsewhere if needed)
            m_IsSpun = true;
        }

        if (m_CurrentTime >= m_ChargeTime + m_AttackTime)
        {
            m_List = enSpinning::CoolDown;
            m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
        }
        break;

    case enSpinning::CoolDown:
        if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SpecialToIdol) || m_CurrentTime >= m_ChargeTime + m_AttackTime + m_EndTime)
        {
            m_List = enSpinning::Trans;
        }
        break;

    case enSpinning::Trans:
        if (!m_IsDebugStop)
        {
            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
        }
        break;
    }

    // 基底 Update は先頭で呼んでいるためここでは不要
}

void BossSpinningState::LateUpdate()
{
    BossAttackStateBase::LateUpdate();
}

void BossSpinningState::Draw()
{
    BossAttackStateBase::Draw();
}

void BossSpinningState::Exit()
{
    BossAttackStateBase::Exit();
}

void BossSpinningState::DrawImGui()
{
#if _DEBUG
    ImGui::Begin(IMGUI_JP("BossSpinning State"));
    CImGuiManager::Slider<float>(IMGUI_JP("半径"), m_Radius, 0.0f, 20.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("回転速度"), m_RotateSpeed, 0.1f, 20.0f, true);
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("再生アニメ番号 (ChargeToIdol / ChargeAttack / Charge)"));
    ImGui::InputInt("ChargeToIdol", &m_AnimIdxChargeToIdol);
    ImGui::InputInt("ChargeAttack", &m_AnimIdxChargeAttack);
    ImGui::InputInt("Charge", &m_AnimIdxCharge);
    BossAttackStateBase::DrawImGui();
    ImGui::End();
#endif
}

void BossSpinningState::LoadSettings()
{
    BossAttackStateBase::LoadSettings();
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("Radius")) m_Radius = j["Radius"].get<float>();
    if (j.contains("RotateSpeed")) m_RotateSpeed = j["RotateSpeed"].get<float>();
    if (j.contains("AnimIdx")) {
        auto a = j["AnimIdx"];
        if (a.contains("ChargeToIdol")) m_AnimIdxChargeToIdol = a["ChargeToIdol"].get<int>();
        if (a.contains("ChargeAttack")) m_AnimIdxChargeAttack = a["ChargeAttack"].get<int>();
        if (a.contains("Charge")) m_AnimIdxCharge = a["Charge"].get<int>();
    }
}

void BossSpinningState::SaveSettings() const
{
    json j = SerializeSettings();
    j["Radius"] = m_Radius;
    j["RotateSpeed"] = m_RotateSpeed;
    j["AnimIdx"] = json::object();
    j["AnimIdx"]["ChargeToIdol"] = m_AnimIdxChargeToIdol;
    j["AnimIdx"]["ChargeAttack"] = m_AnimIdxChargeAttack;
    j["AnimIdx"]["Charge"] = m_AnimIdxCharge;
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    FileManager::JsonSave(filePath, j);
}
