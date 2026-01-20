#include "BossStompState.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game/04_Time/Time.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

BossStompState::BossStompState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_pIdol()
	, m_List(enAttack::None)
	, m_Velocity(0.0f, 0.0f, 0.0f)
	, m_JumpPower(5.0f)     
	, m_Gravity(1.5f)      
	, m_JumpFrag(false)
	, m_GroundedFrag(true)
	, m_Timer(0.0f)
	, TransitionTimer(120.0f)
	, m_UpSpeed(1.0f)
	, m_ForwardSpeed(15.0f)
	, m_TargetPos(0.0f, 0.0f, 0.0f)
	, m_StartPos_Stomp(0.0f, 0.0f, 0.0f)
	, m_HasLanded(false)
{
}

void BossStompState::DrawImGui()
{
    ImGui::Begin(IMGUI_JP("BossStomp State"));
    CImGuiManager::Slider<float>(IMGUI_JP("ジャンプ力"), m_JumpPower, 0.0f, 20.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("重力"), m_Gravity, 0.0f, 10.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("上昇速度倍率"), m_UpSpeed, 0.1f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("前方移動速度"), m_ForwardSpeed, 0.0f, 30.0f, true);
    BossAttackStateBase::DrawImGui();
    ImGui::End();
}

void BossStompState::LoadSettings()
{
    // 基底の既定設定を読み込む
    BossAttackStateBase::LoadSettings();

    // まずソースフォルダの設定ファイルを優先して読み込む
    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("JumpPower")) m_JumpPower = j["JumpPower"].get<float>();
    if (j.contains("Gravity")) m_Gravity = j["Gravity"].get<float>();
    if (j.contains("UpSpeed")) m_UpSpeed = j["UpSpeed"].get<float>();
    if (j.contains("ForwardSpeed")) m_ForwardSpeed = j["ForwardSpeed"].get<float>();
    // オフセットは ColliderWindow で管理
}

void BossStompState::SaveSettings() const
{
    // ベース設定を取得して派生設定を追加してソースフォルダに保存
    json j = SerializeSettings();
    j["JumpPower"] = m_JumpPower;
    j["Gravity"] = m_Gravity;
    j["UpSpeed"] = m_UpSpeed;
    j["ForwardSpeed"] = m_ForwardSpeed;

    auto srcDir = std::filesystem::path(__FILE__).parent_path();
    auto filePath = srcDir / GetSettingsFileName();
    FileManager::JsonSave(filePath, j);
}

BossStompState::~BossStompState()
{
}

void BossStompState::Enter()
{
	// 当たり判定を有効化.
	auto* pStompCollider = m_pOwner->GetStompCollider();
	if (pStompCollider) {
		pStompCollider->SetActive(true);            
		pStompCollider->SetColor(Color::eColor::Magenta); 
		pStompCollider->SetRadius(30.0f);
		pStompCollider->SetHeight(15.0f);
	}



	m_Velocity = { 0.0f, 0.0f, 0.0f };
	m_GroundedFrag = true;
	m_HasLanded = false;

	//向きだけプレイヤーに合わせる.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);
	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);
	float angle_radian = std::atan2f(-dx, -dz);
	m_pOwner->SetRotationY(angle_radian);

	// 開始位置と目標位置を保存（飛びかかり用）.
	m_StartPos_Stomp = BossPosF;
	m_TargetPos = PlayerPosF;
	m_TargetPos.y = 0.0f; // 地面高さに固定.

	//溜めアニメーション開始.
	m_pOwner->SetAnimSpeed(2.0);
	m_pOwner->ChangeAnim(Boss::enBossAnim::Special_0);

}

void BossStompState::Update()
{
	auto* pStompCollider = m_pOwner->GetStompCollider();

	switch (m_List)
	{
	case BossStompState::enAttack::None:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0))
		{
			m_pOwner->SetPositionY(m_JumpPower);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
			m_List = enAttack::Stomp;

			if (pStompCollider) {
				pStompCollider->SetRadius(30.0f);
				pStompCollider->SetAttackAmount(15.0f);
			}
		}
		break;

	case BossStompState::enAttack::Stomp:
		BossAttack();

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_1))
		{
			m_pOwner->SetPositionY(0.0f);
			m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
			m_List = enAttack::CoolTime;

			if (pStompCollider) 
			{
				pStompCollider->SetActive(false);
			}
		}
		break;
	case BossStompState::enAttack::CoolTime:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SpecialToIdol))
		{
			m_List = enAttack::Trans;
		}
		break;

	case BossStompState::enAttack::Trans:
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;
	}
}

void BossStompState::LateUpdate()
{
}

void BossStompState::Draw()
{
}

void BossStompState::Exit()
{
	m_GroundedFrag = true;
	m_pOwner->SetPositionY(0.0f);
}

void BossStompState::BossAttack()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	//重力のみを計算.
	m_Velocity.y -= m_Gravity * deltaTime;

	float nextY = m_pOwner->GetPositionY() + (m_Velocity.y * m_UpSpeed);

	// 前方移動（XZ平面）.
	DirectX::XMFLOAT3 currentPos = m_pOwner->GetPosition();
	DirectX::XMVECTOR vCurrent = DirectX::XMLoadFloat3(&currentPos);
	DirectX::XMVECTOR vTarget = DirectX::XMLoadFloat3(&m_TargetPos);
	DirectX::XMVECTOR vDir = DirectX::XMVectorSubtract(vTarget, vCurrent);
	vDir = DirectX::XMVectorSetY(vDir, 0.0f);
	
	// 目標に近い場合は移動しない.
	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDir));
	if (distSq > 1.0f && nextY > 0.0f) {
		vDir = DirectX::XMVector3Normalize(vDir);
		DirectX::XMVECTOR vMove = DirectX::XMVectorScale(vDir, m_ForwardSpeed * deltaTime);
		DirectX::XMFLOAT3 move;
		DirectX::XMStoreFloat3(&move, vMove);
		currentPos.x += move.x;
		currentPos.z += move.z;
		m_pOwner->SetPositionX(currentPos.x);
		m_pOwner->SetPositionZ(currentPos.z);
	}

	//地面判定.
	if (nextY <= 0.0f)
	{
		nextY = 0.0f;
		m_Velocity.y = 0.0f;

		// 着地時に1回だけ当たり判定を有効化.
		if (!m_HasLanded) {
			m_HasLanded = true;
			auto* pStompCollider = m_pOwner->GetStompCollider();
			if (pStompCollider) {
				pStompCollider->SetActive(true);
				pStompCollider->SetAttackAmount(20.0f);
			}
		}
	}

	m_pOwner->SetPositionY(nextY);
}
