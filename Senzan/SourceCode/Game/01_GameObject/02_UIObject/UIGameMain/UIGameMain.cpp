#include "UIGameMain.h"
#include "02_UIObject/UILoader/UILoader.h"
#include "Utility/Color/Color.h"

#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Math/Easing/Easing.h"
#include "ResourceManager/ResourceManager.h"

#if _DEBUG
#include "Singleton/ImGui/CImGuiManager.h"
#endif

UIGameMain::UIGameMain()
	: m_pUIs			()
	, m_GuageDelSpeed	()
	, m_ClockSecInitRot	( 6.28f )
	, m_ClockSecNow		( 0.0f )
	, m_Combo			( 0 )
	, m_ComboMax		( 99 )
	, m_PlayerHP		()
	, m_PlayerDamage	()
	, m_PlayerUlt		()
	, m_BossHP			()
	, m_BossDamage		()
{
	UILoader::LoadFromJson("Data\\Image\\Sprite\\UIData\\GameMain.json", m_pUIs);
	Create();
}

UIGameMain::~UIGameMain()
{
}

//-----------------------------------------------------------------------.

void UIGameMain::Create()
{
	for (auto& ui : m_pUIs)
	{
		if (ui->GetUIName() == "HPGauge_0") {
			m_BossHP.InitRate = ui->GetScaleX();
		}
		else if (ui->GetUIName() == "HPDamage_0") {
			m_BossDamage.InitRate = ui->GetScaleX();
		}
		else if (ui->GetUIName() == "ULTGauge_0") {
			m_PlayerUlt.InitRate = ui->GetScaleX();
		}
		else if (ui->GetUIName() == "HPGauge_1") {
			m_PlayerHP.InitRate = ui->GetScaleX();
		}
		else if (ui->GetUIName() == "HPDamage_1") {
			m_PlayerDamage.InitRate = ui->GetScaleX();
		}
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::Update()
{
	float dt = Time::GetInstance().GetDeltaTime();

	m_PlayerHP.Update(dt);
	m_PlayerDamage.Update(dt);

	m_PlayerUlt.Update(dt);
	
	m_BossHP.Update(dt);
	m_BossDamage.Update(dt);

	m_BossHP.Before		= m_BossHP.Now;
	m_PlayerHP.Before	= m_PlayerHP.Now;
	m_PlayerUlt.Before	= m_PlayerUlt.Now;

	// デバッグ用のImGui.
	//	UIのデバッグ以外で通すと表示に齟齬が出るので消す.
#if 0
	// --- ImGui Debug ---.
	ImGui::Begin("UI Gauge Debug");

	ImGui::Text("=== Player HP ===");
	ImGui::DragFloat("HP Max", &m_DebugPlayerHPMax, 1.0f, 1.0f, 9999.0f);
	ImGui::DragFloat("HP Now", &m_DebugPlayerHP, 1.0f, 0.0f, m_DebugPlayerHPMax);
		SetPlayerHP(m_DebugPlayerHPMax, m_DebugPlayerHP);

	ImGui::Separator();

	ImGui::Text("=== Player ULT ===");
	ImGui::DragFloat("ULT Max", &m_DebugPlayerUltMax, 1.0f, 1.0f, 9999.0f);
	ImGui::DragFloat("ULT Now", &m_DebugPlayerUlt, 1.0f, 0.0f, m_DebugPlayerUltMax);
		SetPlayerUlt(m_DebugPlayerUltMax, m_DebugPlayerUlt);

	ImGui::Separator();

	ImGui::Text("=== Boss HP ===");
	ImGui::DragFloat("Boss HP Max", &m_DebugBossHPMax, 5.0f, 1.0f, 99999.0f);
	ImGui::DragFloat("Boss HP Now", &m_DebugBossHP, 5.0f, 0.0f, m_DebugBossHPMax);
		SetBossHP(m_DebugBossHPMax, m_DebugBossHP);

	ImGui::Separator();

	// --- Combo Test ---.
	ImGui::Text("=== Combo ===");
	if (ImGui::Button("+ Combo")) m_Combo++;
	ImGui::SameLine();
	if (ImGui::Button("- Combo")) m_Combo--;
	ImGui::Text("Combo : %d", m_Combo);

	ImGui::End();
#endif


	m_Combo = std::clamp(m_Combo, 0, m_ComboMax);

	for (auto& ui : m_pUIs)
	{
		if (ui->GetUIName() == "HPGauge_0") {
			ui->SetScaleX(m_BossHP.InitRate * m_BossHP.Rate);
		}
		else if (ui->GetUIName() == "HPDamage_0") {
			ui->SetScaleX(m_BossDamage.InitRate * m_BossDamage.Rate);
		}
		else if (ui->GetUIName() == "ULTGauge_0") {
			ui->SetScaleX(m_PlayerUlt.InitRate * m_PlayerUlt.Rate);
		}
		else if (ui->GetUIName() == "HPGauge_1") {
			ui->SetScaleX(m_PlayerHP.InitRate * m_PlayerHP.Rate);
		}
		else if (ui->GetUIName() == "HPDamage_1") {
			ui->SetScaleX(m_PlayerDamage.InitRate * m_PlayerDamage.Rate);
		}
		else if (ui->GetUIName() == "ClockSec_0") {
			ui->SetRotationZ(m_ClockSecInitRot * m_ClockSecNow);
		}

		if (ui->GetUIName() == "Number0_0") {
			ui->AttachSprite(ResourceManager::GetSprite2D("Number" + std::to_string((m_Combo / 10) % 10)));
		}
		if (ui->GetUIName() == "Number0_1") {
			ui->AttachSprite(ResourceManager::GetSprite2D("Number" + std::to_string(m_Combo % 10)));
		}
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::LateUpdate()
{
	for (auto& ui : m_pUIs)
	{
		ui->Update();
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::Draw()
{
	for (auto& ui : m_pUIs)
	{
		DirectX11::GetInstance().SetDepth(false);
		ui->Draw();
		DirectX11::GetInstance().SetDepth(true);
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::SetCombo(int num)
{
	m_Combo = num;
}

//-----------------------------------------------------------------------.

void UIGameMain::SetTime(float max, float now)
{
	m_ClockSecNow = m_ClockSecInitRot * (now / max);
}

//-----------------------------------------------------------------------.

void UIGameMain::SetPlayerHP(float max, float now)
{
	m_PlayerHP.Set(max, now);
	// ここで赤を追従開始させる.
	if (m_PlayerHP.IsChanged)
	{
		m_PlayerDamage.StartFollow(
			(max > 0.0f) ? (now / max) : 0.0f
		);
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::SetPlayerUlt(float max, float now)
{
	m_PlayerUlt.Set(max, now);
}

//-----------------------------------------------------------------------.

void UIGameMain::SetBossHP(float max, float now)
{
	m_BossHP.Set(max, now);
	// ここで赤を追従開始させる.
	if (m_BossHP.IsChanged)
	{
		m_BossDamage.StartFollow(
			(max > 0.0f) ? (now / max) : 0.0f
		);
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::Gauge::Set(float max, float now)
{
	Max = max;
	Now = now;

	IsChanged = (Now != Before);

	if (!IsChanged)
		return;

	float newRate = (Max > 0.0f) ? (Now / Max) : 0.0f;

	// 緑ゲージ用（即イージング）.
	IsEasing	= true;
	EaseTime	= 0.0f;
	EaseStart	= Rate;
	EaseEnd		= newRate;

	DelayMax = DelayFrame * Time::GetInstance().GetDeltaTime();
	EaseMax = EaseFrame * Time::GetInstance().GetDeltaTime();
}

//-----------------------------------------------------------------------.

void UIGameMain::Gauge::Update(float dt)
{
	// 遅延フェーズ（赤だけ）.
	if (IsDelay)
	{
		DelayTime += dt;
		if (DelayTime >= DelayMax)
		{
			IsDelay = false;
			IsEasing = true;
			EaseTime = 0.0f;
		}
		return;
	}

	// イージングフェーズ（緑も赤も）.
	if (!IsEasing)
		return;

	EaseTime += dt;

	MyEasing::UpdateEasing(
		MyEasing::Type::OutExpo,
		EaseTime,
		EaseMax,
		EaseStart,
		EaseEnd,
		Rate
	);

	if (EaseTime >= EaseMax)
	{
		Rate = EaseEnd;
		IsEasing = false;
	}
}

//-----------------------------------------------------------------------.

void UIGameMain::Gauge::StartFollow(float targetRate)
{
	// 常に最新目標を上書きする.
	IsDelay = true;
	DelayTime = 0.0f;

	IsEasing = false;	// 進行中でもリセット.
	EaseTime = 0.0f;

	EaseStart = Rate;	// 今の赤位置から.
	EaseEnd = targetRate;

	DelayMax = DelayFrame * Time::GetInstance().GetDeltaTime();
	EaseMax = EaseFrame * Time::GetInstance().GetDeltaTime();
}

//-----------------------------------------------------------------------.

bool UIGameMain::Gauge::ConsumeChanged()
{
	if (!IsChanged) return false;
	IsChanged = false;
	return true;
}