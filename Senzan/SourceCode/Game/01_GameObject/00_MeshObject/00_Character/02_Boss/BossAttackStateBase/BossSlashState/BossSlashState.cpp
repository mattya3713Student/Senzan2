#include "BossSlashState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先
#include "Resource\\Mesh\\02_Skin\\SkinMesh.h"

#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

constexpr float MY_PI = 3.1415926535f;

BossSlashState::BossSlashState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_List(enList::none)
{
}

BossSlashState::~BossSlashState()
{
}

void BossSlashState::Enter()
{
    BossAttackStateBase::Enter();

    m_CurrentTime = 0.0f;
    m_AnimSpeed = 0.0f;

    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimTime(0.0);
    m_pOwner->SetAnimSpeed(m_AnimSpeed);

    // 斬るアニメーションの再生.
    m_pOwner->ChangeAnim(Boss::enBossAnim::Slash);

    m_pOwner->SetAttackColliderActive(false);

    // 斬撃当たり判定: 右手ボーン
    ColliderWindow slashWindow;
    slashWindow.BoneName = "boss_Hand_R";
    slashWindow.Start = m_SlashStart;
    slashWindow.Duration = m_SlashDuration;
    m_ColliderWindows.push_back(slashWindow);

	// 踏み込み
    MovementWindow stepIn;
    stepIn.Start = m_StepStart;
    stepIn.Duration = m_StepDuration;
    stepIn.Speed = m_StepSpeed;
    m_MovementWindows.push_back(stepIn);

	// 当たり判定は window 側で ON/OFF するので通常攻撃判定はOFF（念のため）
	m_pOwner->SetAttackColliderActive(false);

	// 初期位置を保存.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMStoreFloat3(&m_StartPos, DirectX::XMLoadFloat3(&BossPosF));

	m_List = enList::SlashAttack;
}

void BossSlashState::Update()
{
    BossAttackStateBase::Update();


    const float dt = Time::GetInstance().GetDeltaTime();

    //// ホーミング（一定時間まで）
    //if (m_StateTimer <= m_HomingEndTime)
    //{
    //    const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
    //    DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

    //    const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
    //    DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

    //    DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
    //    Direction = DirectX::XMVectorSetY(Direction, 0.0f);

    //    float dx = DirectX::XMVectorGetX(Direction);
    //    float dz = DirectX::XMVectorGetZ(Direction);
    //    float angle_radian = std::atan2f(-dx, -dz);
    //    m_pOwner->SetRotationY(angle_radian);
    //}

    UpdateBaseLogic(dt);

    switch (m_List)
    {
    case BossSlashState::enList::ChargeSlash:
        if (m_CurrentTime >= m_ChargeTime)
        {
            m_pOwner->ChangeAnim(Boss::enBossAnim::Charge);
            m_List = enList::SlashAttack;
        }
        break;

    case BossSlashState::enList::SlashAttack:
        if (m_CurrentTime >= m_ChargeTime + m_AttackTime)
        {
            m_List = enList::SlashIdol;
        }
        break;

    case BossSlashState::enList::SlashIdol:
        // Idol遷移も時間で統一（余韻用に固定）
        if (m_CurrentTime >= m_ChargeTime + m_AttackTime + m_EndTime)
        {
            if (!m_IsDebugStop)
            {
                m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
            }
            break;

    default:
        break;
        }
    }

}

void BossSlashState::LateUpdate()
{
    BossAttackStateBase::LateUpdate();
}

void BossSlashState::Draw()
{
    BossAttackStateBase::Draw();
}

void BossSlashState::Exit()
{
    BossAttackStateBase::Exit();
	// window 制御のコライダーを確実にOFF
	m_pOwner->SetColliderActiveByName("boss_Hand_R", false);
	m_pOwner->SetAttackColliderActive(false);
}

void BossSlashState::DrawImGui()
{
    ImGui::Begin(IMGUI_JP("ボス斬撃設定"));
    ImGui::Text(IMGUI_JP("斬撃設定"));
    CImGuiManager::Slider<float>(IMGUI_JP("斬撃開始"), m_SlashStart, 0.0f, 1.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("斬撃持続"), m_SlashDuration, 0.0f, 1.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("ホーミング終了時間"), m_HomingEndTime, 0.0f, 2.0f, true);
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("踏み込み設定"));
    CImGuiManager::Slider<float>(IMGUI_JP("踏み込み開始"), m_StepStart, 0.0f, 1.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("踏み込み持続"), m_StepDuration, 0.0f, 1.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("踏み込み速度"), m_StepSpeed, 0.0f, 200.0f, true);

    ImGui::Separator();
    BossAttackStateBase::DrawImGui();
    ImGui::End();
}

void BossSlashState::LoadSettings()
{
    // 基底の読み込みを行った後、Data/Json/Boss/<file> から派生項目を読み込む
    BossAttackStateBase::LoadSettings();
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        filePath = std::filesystem::current_path() / "Data" / "Json" / "Boss" / filePath;
    }
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("SlashStart")) m_SlashStart = j["SlashStart"].get<float>();
    if (j.contains("SlashDuration")) m_SlashDuration = j["SlashDuration"].get<float>();
    if (j.contains("HomingEndTime")) m_HomingEndTime = j["HomingEndTime"].get<float>();
    if (j.contains("StepStart")) m_StepStart = j["StepStart"].get<float>();
    if (j.contains("StepDuration")) m_StepDuration = j["StepDuration"].get<float>();
    if (j.contains("StepSpeed")) m_StepSpeed = j["StepSpeed"].get<float>();
}

void BossSlashState::SaveSettings() const
{
    // ベースの設定を取得して派生固有情報を追加して一度だけ保存する
    json j = SerializeSettings();
    j["SlashStart"] = m_SlashStart;
    j["SlashDuration"] = m_SlashDuration;
    j["HomingEndTime"] = m_HomingEndTime;
    j["StepStart"] = m_StepStart;
    j["StepDuration"] = m_StepDuration;
    j["StepSpeed"] = m_StepSpeed;

    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = std::filesystem::current_path() / "Data" / "Json" / "Boss" / GetSettingsFileName();
    std::filesystem::create_directories(filePath.parent_path());
    FileManager::JsonSave(filePath, j);
}
