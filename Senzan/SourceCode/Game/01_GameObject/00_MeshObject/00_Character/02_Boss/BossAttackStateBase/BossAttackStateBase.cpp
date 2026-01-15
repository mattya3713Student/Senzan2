#include "BossAttackStateBase.h"

#include "Game/04_Time/Time.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "System/Utility/FileManager/FileManager.h"

#include "System/Singleton/ImGui/CImGuiManager.h"

BossAttackStateBase::BossAttackStateBase(Boss* owner)
    : StateBase<Boss>(owner)
    , m_pTransform(std::make_shared<Transform>())
{
}

void BossAttackStateBase::Enter()
{
    // 設定を読み込み、タイマーをリセットする
    LoadSettings();
    m_CurrentTime = 0.0f;
}

void BossAttackStateBase::Update()
{
    // デバッグ用の ImGui を表示
    DrawImGui();

    // デバッグ停止フラグが立っている場合は Update の残り処理をスキップして
    // ステート遷移を発生させないようにする。
    if (m_IsDebugStop)
    {
        return;
    }

    // Update では時間更新は行わない。時間は UpdateBaseLogic の呼び出し側が dt を渡して更新する。
}

void BossAttackStateBase::UpdateBaseLogic(float dt)
{
    // 基本攻撃ロジックの更新
    // - ステートタイマーを進める
    // - アニメーション速度を適用する
    // - 当たり判定ウィンドウを開始/終了する
    // - 移動ウィンドウ中は前方へ移動する
    // 経過時間を進める
    m_CurrentTime += dt;

    // フェーズ（溜め -> 攻撃 -> 余波）に応じてアニメーション速度を切り替える
    float currentAnimSpeed = m_AnimSpeed;
    const float attackStart = m_ChargeTime;
    // 攻撃終了時間: m_AttackTime が指定されていればそれを使用、未指定なら m_EndTime を利用
    const float attackEnd = (m_AttackTime > 0.0f) ? m_AttackTime : m_EndTime;
    if (m_CurrentTime < attackStart && m_ChargeTime > 0.0f)
    {
        currentAnimSpeed = m_AnimSpeedCharge;
    }
    else if (m_CurrentTime >= attackStart && m_CurrentTime < attackEnd)
    {
        currentAnimSpeed = m_AnimSpeedAttack;
    }
    else
    {
        currentAnimSpeed = m_AnimSpeedExit;
    }

    m_AnimSpeed = currentAnimSpeed;
    m_pOwner->SetAnimSpeed(currentAnimSpeed);

	for (auto& window : m_ColliderWindows)
	{
        if (!window.IsAct && m_CurrentTime >= window.Start)
		{
			m_pOwner->SetColliderActiveByName(window.BoneName, true);
			window.IsAct = true;
		}
        if (window.IsAct && !window.IsEnd && m_CurrentTime >= (window.Start + window.Duration))
		{
			m_pOwner->SetColliderActiveByName(window.BoneName, false);
			window.IsEnd = true;
		}
	}

	for (auto& mv : m_MovementWindows)
	{
        if (!mv.IsAct && m_CurrentTime >= mv.Start)
		{
			mv.IsAct = true;
		}
        if (mv.IsAct && m_CurrentTime < (mv.Start + mv.Duration))
		{
			// 注意: 前方への移動。専用の移動 API があればそちらを使用してください。
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

void BossAttackStateBase::Exit()
{
    for (auto& w : m_ColliderWindows) { w.Reset(); }
    for (auto& w : m_MovementWindows) { w.Reset(); }
};

void BossAttackStateBase::DrawImGui()
{
    // ImGui を用いた攻撃パラメータ編集パネル
    // このパネルからダメージ、当たり判定サイズ、フェーズ毎のアニメ速度
    // などのパラメータを調整できます。実行時のみの編集用です。
	if (!ImGui::Begin(IMGUI_JP("Boss Attack State"))) {
		ImGui::End();
		return;
	}

    ImGui::Checkbox(IMGUI_JP("ストップ"), &m_IsDebugStop);

    // 攻撃力
    ImGui::SliderFloat(IMGUI_JP("攻撃力"), &m_AttackAmount, 0.0f, 9999.0f);

    // 当たり判定サイズ
    ImGui::SliderFloat(IMGUI_JP("判定幅"), &m_ColliderWidth, 0.0f, 20.0f);
    ImGui::SliderFloat(IMGUI_JP("判定高さ"), &m_ColliderHeight, 0.0f, 10.0f);

    // ステートの時間設定
    ImGui::SliderFloat(IMGUI_JP("ステート全体の長さ"), &m_EndTime, 0.0f, 10.0f);
    ImGui::SliderFloat(IMGUI_JP("チャージ時間"), &m_ChargeTime, 0.0f, 5.0f);

	ImGui::Separator();
	ImGui::Text(IMGUI_JP("アニメーション速度"));
	 ImGui::SliderFloat("Anim Speed - Charge", &m_AnimSpeedCharge, 0.1f, 3.0f);
	 ImGui::SliderFloat("Anim Speed - Attack", &m_AnimSpeedAttack, 0.1f, 3.0f);
	 ImGui::SliderFloat("Anim Speed - Exit", &m_AnimSpeedExit, 0.1f, 3.0f);

    // 現在フェーズ表示：溜め / 攻撃 / 終わり
    const float attackStart = m_ChargeTime;
    const float attackEnd = (m_AttackTime > 0.0f) ? m_AttackTime : m_EndTime;
    const char* phase = "Unknown";
    if (m_CurrentTime < attackStart && m_ChargeTime > 0.0f) phase = IMGUI_JP("溜め");
    else if (m_CurrentTime >= attackStart && m_CurrentTime < attackEnd) phase = IMGUI_JP("攻撃");
    else phase = IMGUI_JP("余波");

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("現在の経過時間: %.3f 秒"), m_CurrentTime);
    ImGui::Text(IMGUI_JP("現在フェーズ: %s"), phase);

    // アニメ速度は UpdateBaseLogic で毎フレーム決定してオーナーへ適用します。

    // Load / Save ボタン
    if (ImGui::Button(IMGUI_JP("Load"))) {
        try {
            LoadSettings();
        }
        catch (...) {}
    }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("Save"))) {
        try {
            SaveSettings();
        }
        catch (...) {}
    }
    if (ImGui::Button(IMGUI_JP("ReEtert"))) {
        try {
            this->Enter();
        }
        catch (...) {}
    }

    ImGui::End();
}

void BossAttackStateBase::LoadSettings()
{
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        filePath = std::filesystem::current_path() / "Data" / "Json" / "Boss" / filePath;
    }
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("m_AttackAmount")) m_AttackAmount = j["m_AttackAmount"].get<float>();
    if (j.contains("m_ColliderWidth")) m_ColliderWidth = j["m_ColliderWidth"].get<float>();
    if (j.contains("m_ColliderHeight")) m_ColliderHeight = j["m_ColliderHeight"].get<float>();
    if (j.contains("m_EndTime")) m_EndTime = j["m_EndTime"].get<float>();
    if (j.contains("m_ChargeTime")) m_ChargeTime = j["m_ChargeTime"].get<float>();
    if (j.contains("AnimSpeeds") && j["AnimSpeeds"].is_object()) {
        auto& a = j["AnimSpeeds"];
        if (a.contains("Charge")) m_AnimSpeedCharge = a["Charge"].get<float>();
        if (a.contains("Attack")) m_AnimSpeedAttack = a["Attack"].get<float>();
        if (a.contains("Exit")) m_AnimSpeedExit = a["Exit"].get<float>();
    }
    if (j.contains("ColliderWindows") && j["ColliderWindows"].is_array()) {
        m_ColliderWindows.clear();
        for (const auto& entry : j["ColliderWindows"]) {
            if (entry.contains("start") && entry.contains("duration")) {
                ColliderWindow w;
                w.Start = entry["start"].get<float>();
                w.Duration = entry["duration"].get<float>();
                if (entry.contains("BoneName")) w.BoneName = entry["BoneName"].get<std::string>();
                m_ColliderWindows.push_back(w);
            }
        }
    }
}

void BossAttackStateBase::SaveSettings() const
{
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        auto dir = std::filesystem::current_path() / "Data" / "Json" / "Boss";
        std::filesystem::create_directories(dir);
        filePath = dir / filePath;
    }

    json j;
    j["m_AttackAmount"] = m_AttackAmount;
    j["m_ColliderWidth"] = m_ColliderWidth;
    j["m_ColliderHeight"] = m_ColliderHeight;
    j["m_EndTime"] = m_EndTime;
    j["m_ChargeTime"] = m_ChargeTime;
    j["AnimSpeeds"] = json::object();
    j["AnimSpeeds"]["Charge"] = m_AnimSpeedCharge;
    j["AnimSpeeds"]["Attack"] = m_AnimSpeedAttack;
    j["AnimSpeeds"]["Exit"] = m_AnimSpeedExit;

    j["ColliderWindows"] = json::array();
    for (const auto& w : m_ColliderWindows) {
        json e;
        e["start"] = w.Start;
        e["duration"] = w.Duration;
        e["BoneName"] = w.BoneName;
        j["ColliderWindows"].push_back(e);
    }

    FileManager::JsonSave(filePath, j);
}

nlohmann::json BossAttackStateBase::SerializeSettings() const
{
    nlohmann::json j;
    j["m_AttackAmount"] = m_AttackAmount;
    j["m_ColliderWidth"] = m_ColliderWidth;
    j["m_ColliderHeight"] = m_ColliderHeight;
    j["m_EndTime"] = m_EndTime;
    j["m_ChargeTime"] = m_ChargeTime;
    j["AnimSpeeds"] = nlohmann::json::object();
    j["AnimSpeeds"]["Charge"] = m_AnimSpeedCharge;
    j["AnimSpeeds"]["Attack"] = m_AnimSpeedAttack;
    j["AnimSpeeds"]["Exit"] = m_AnimSpeedExit;

    j["ColliderWindows"] = nlohmann::json::array();
    for (const auto& w : m_ColliderWindows) {
        nlohmann::json e;
        e["start"] = w.Start;
        e["duration"] = w.Duration;
        e["BoneName"] = w.BoneName;
        j["ColliderWindows"].push_back(e);
    }

    return j;
}

