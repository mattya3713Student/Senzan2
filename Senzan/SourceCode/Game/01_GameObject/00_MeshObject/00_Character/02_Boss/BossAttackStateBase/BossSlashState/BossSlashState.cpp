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

	// 初期位置を保存.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMStoreFloat3(&m_StartPos, DirectX::XMLoadFloat3(&BossPosF));

	m_List = enList::ChargeSlash;
}

void BossSlashState::Update()
{
    BossAttackStateBase::Update();

    const float dt = Time::GetInstance().GetDeltaTime();
    
    UpdateBaseLogic(dt);

    switch (m_List)
    {
    case BossSlashState::enList::ChargeSlash:
       
        if (m_CurrentTime >= m_ChargeTime)
        {
            m_List = enList::SlashAttack;
        }
        break;

    case BossSlashState::enList::SlashAttack:
        if (m_TransitionOnAnimEnd_Attack)
        {
            if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Slash))
            {
                m_List = enList::SlashIdol;
                m_pOwner->ChangeAnim(Boss::enBossAnim::SlashToIdol);
            }
        }
        else
        {
            if (m_CurrentTime >= m_ChargeTime + m_AttackTime)
            {
                m_List = enList::SlashIdol;
                m_pOwner->ChangeAnim(Boss::enBossAnim::SlashToIdol);
            }
        }
        break;

    case BossSlashState::enList::SlashIdol:
        // Idol遷移も時間で統一（余韻用に固定）
        if (m_TransitionOnAnimEnd_Exit)
        {
            if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SlashToIdol))
            {
                if (!m_IsDebugStop)
                {
                    m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
                }
                break;
            }
        }
        else
        {
            if (m_CurrentTime >= m_ChargeTime + m_AttackTime + m_EndTime)
            {
                if (!m_IsDebugStop)
                {
                    m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
                }
                break;
            }
        }

    default:
        break;
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
    ImGui::Separator();
    // 回転オフセット調整 (度数法)
    if (m_pOwner) {
        CImGuiManager::Slider<float>(IMGUI_JP("斬撃オフセット X(deg)"), m_pOwner->m_SlashRotOffsetDeg.x, -180.0f, 180.0f, true);
        CImGuiManager::Slider<float>(IMGUI_JP("斬撃オフセット Y(deg)"), m_pOwner->m_SlashRotOffsetDeg.y, -180.0f, 180.0f, true);
        CImGuiManager::Slider<float>(IMGUI_JP("斬撃オフセット Z(deg)"), m_pOwner->m_SlashRotOffsetDeg.z, -180.0f, 180.0f, true);
    }

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
}

void BossSlashState::SaveSettings() const
{
    // ベースの設定を取得して派生固有情報を追加して一度だけ保存する
    json j = SerializeSettings();
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = std::filesystem::current_path() / "Data" / "Json" / "Boss" / GetSettingsFileName();
    std::filesystem::create_directories(filePath.parent_path());
    FileManager::JsonSave(filePath, j);
}
