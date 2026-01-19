#include "BossAttackStateBase.h"

#include "Game/04_Time/Time.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "System/Utility/FileManager/FileManager.h"

#include "System/Singleton/ImGui/CImGuiManager.h"
#include <cstring>
#include <cmath>

BossAttackStateBase::BossAttackStateBase(Boss* owner)
    : StateBase<Boss>(owner)
    , m_pTransform(std::make_shared<Transform>())
{
}

DirectX::XMFLOAT3 BossAttackStateBase::ComputeMovementEndPos(const MovementWindow& mv, const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos) const
{
    // デフォルト実装: ターゲット方向に対して Speed * Duration * Distance だけ移動する
    DirectX::XMVECTOR start_v = DirectX::XMLoadFloat3(&startPos);
    DirectX::XMVECTOR target_v = DirectX::XMLoadFloat3(&targetPos);
    DirectX::XMVECTOR dir_v = DirectX::XMVectorSubtract(target_v, start_v);
    float len = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir_v));
    DirectX::XMFLOAT3 endPos = startPos;
    if (len > 1e-6f)
    {
        DirectX::XMVECTOR dir_n = DirectX::XMVector3Normalize(dir_v);
        DirectX::XMVECTOR move_amount_v = DirectX::XMVectorScale(dir_n, mv.Speed * mv.Duration * mv.Distance);
        DirectX::XMVECTOR end_v = DirectX::XMVectorAdd(start_v, move_amount_v);
        DirectX::XMStoreFloat3(&endPos, end_v);
    }
    return endPos;
}

void BossAttackStateBase::Enter()
{
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
    // 秒数更新.
    m_CurrentTime += dt;

    // アニメーション速度制御.
    float currentAnimSpeed = 0.f;
    if (m_CurrentTime > m_EndTime)
    {
        currentAnimSpeed = m_AnimSpeedExit;
    }
    else if (m_CurrentTime > m_ChargeTime)
    {
        currentAnimSpeed = m_AnimSpeedAttack;
    }
    else
    {
        currentAnimSpeed = m_AnimSpeedCharge;
    }

    m_AnimSpeed = currentAnimSpeed;
    m_pOwner->SetAnimSpeed(currentAnimSpeed);

    // 当たり判定更新.
	for (auto& window : m_ColliderWindows)
	{
        if (window.IsEnd) { continue; }

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

    // 動き更新 (イージング適用).
    for (auto& mv : m_MovementWindows)
    {
        if (!mv.IsAct && m_CurrentTime >= mv.Start)
        {
            mv.IsAct = true;
        }

        if (mv.IsAct && m_CurrentTime < (mv.Start + mv.Duration))
        {
            // 現在位置とターゲット位置を取得.
            DirectX::XMFLOAT3 pos = m_pOwner->GetPosition();
            DirectX::XMFLOAT3 pos_target = m_pOwner->m_PlayerPos;

            // 初回アクティブ化時の初期化.
            if (!mv.Initialized)
            {
                mv.StartPos = pos;
                // 方向はターゲット位置の方を向く(XZ 平面).
                DirectX::XMVECTOR pos_v = DirectX::XMLoadFloat3(&pos);
                DirectX::XMVECTOR target_v = DirectX::XMLoadFloat3(&pos_target);
                DirectX::XMVECTOR dir_v = DirectX::XMVectorSubtract(target_v, pos_v);
                // 正規化ができない場合は移動しない.
                float len = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir_v));
                DirectX::XMFLOAT3 endPos = mv.StartPos;
                if (len > EPSILON_E6)
                {
                    DirectX::XMVECTOR dir_n = DirectX::XMVector3Normalize(dir_v);
                    DirectX::XMVECTOR move_amount_v = DirectX::XMVectorScale(dir_n, mv.Speed * mv.Duration * mv.Distance);
                    DirectX::XMVECTOR end_v = DirectX::XMVectorAdd(pos_v, move_amount_v);
                    DirectX::XMStoreFloat3(&endPos, end_v);
                    endPos = ComputeMovementEndPos(mv, mv.StartPos, pos_target);
                }
                mv.EndPos = endPos;
                // 初期化フラグと LastEasedPos を設定.
                mv.Initialized = true;
                mv.LastEasedPos = mv.StartPos;

                // 追加: 反転フラグが立っている場合、Start->End のベクトルを反転して EndPos を再設定
                if (mv.Reverse) {
                    DirectX::XMVECTOR v_start = DirectX::XMLoadFloat3(&mv.StartPos);
                    DirectX::XMVECTOR v_end = DirectX::XMLoadFloat3(&mv.EndPos);
                    DirectX::XMVECTOR v_delta = DirectX::XMVectorSubtract(v_end, v_start);
                    DirectX::XMVECTOR v_inv_end = DirectX::XMVectorSubtract(v_start, v_delta);
                    DirectX::XMStoreFloat3(&mv.EndPos, v_inv_end);
                }

                // 回転.
                float dx = mv.EndPos.x - mv.StartPos.x;
                float dz = mv.EndPos.z - mv.StartPos.z;
                float target_rad = std::atan2f(dx, dz);
                const float RAD_TO_DEG = 180.0f / 3.14159f;
                float target_deg = target_rad * RAD_TO_DEG;
                m_pOwner->RotetToTarget(target_deg, 180.0f);
            }

            // 経過時間 (ウィンドウ内).
            float t = m_CurrentTime - mv.Start;

            // イージング.
            DirectX::XMFLOAT3 easedPos;
            MyEasing::UpdateEasing(mv.EasingType, t, mv.Duration, mv.StartPos, mv.EndPos, easedPos);

            // 前フレームからの差分を加算して移動を行う.
            DirectX::XMFLOAT3 delta = {};
            delta.x = easedPos.x - mv.LastEasedPos.x;
            delta.y = easedPos.y - mv.LastEasedPos.y;
            delta.z = easedPos.z - mv.LastEasedPos.z;
            m_pOwner->AddPosition(delta);

            mv.LastEasedPos = easedPos;
        }
        else if (mv.IsAct && m_CurrentTime >= (mv.Start + mv.Duration))
        {
            // ウィンドウ終了時に最終位置へ補正して終了フラグをクリア.
            if (mv.Initialized)
            {
                DirectX::XMFLOAT3 finalDelta;
                finalDelta.x = mv.EndPos.x - mv.LastEasedPos.x;
                finalDelta.y = mv.EndPos.y - mv.LastEasedPos.y;
                finalDelta.z = mv.EndPos.z - mv.LastEasedPos.z;
                m_pOwner->AddPosition(finalDelta);
            }
            mv.IsAct = false;
            mv.Initialized = false;
        }
    }
}

void BossAttackStateBase::Exit()
{
    for (auto& w : m_ColliderWindows) { w.Reset(); }
    for (auto& w : m_MovementWindows) { w.Reset(); }

    m_MovementWindows.clear();
    m_ColliderWindows.clear();
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
    ImGui::SliderFloat(IMGUI_JP("判定半径"), &m_ColliderWidth, 0.0f, 100.0f);
    ImGui::SliderFloat(IMGUI_JP("判定高さ"), &m_ColliderHeight, 0.0f, 100.0f);

    // ボーン名表示
    ImGui::Text(IMGUI_JP("追従ボーン: %s"), m_AttackBoneName.empty() ? "None" : m_AttackBoneName.c_str());

    // ステートの時間設定
    ImGui::SliderFloat(IMGUI_JP("ステート全体の長さ"), &m_EndTime, 0.0f, 10.0f);
    ImGui::SliderFloat(IMGUI_JP("チャージ時間"), &m_ChargeTime, 0.0f, 10.0f);
    ImGui::SliderFloat(IMGUI_JP("攻撃時間"), &m_AttackTime, 0.0f, 10.0f);

	ImGui::Separator();
	ImGui::Text(IMGUI_JP("アニメーション速度"));
    ImGui::SliderFloat("Anim Speed - Charge", &m_AnimSpeedCharge, 0.0f, 3.0f);
    ImGui::SliderFloat("Anim Speed - Attack", &m_AnimSpeedAttack, 0.1f, 10.0f);
    ImGui::SliderFloat("Anim Speed - Exit", &m_AnimSpeedExit, 0.1f, 3.0f);

    const float attackStart = m_ChargeTime;
    const float attackEnd = (m_AttackTime > 0.0f) ? m_AttackTime : m_EndTime;
    const char* phase = "Unknown";
    if (m_CurrentTime < attackStart && m_ChargeTime > 0.0f) phase = IMGUI_JP("溜め");
    else if (m_CurrentTime >= attackStart && m_CurrentTime < attackEnd) phase = IMGUI_JP("攻撃");
    else phase = IMGUI_JP("余波");

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("現在の経過時間: %.3f 秒"), m_CurrentTime);
    ImGui::Text(IMGUI_JP("現在フェーズ: %s"), phase);

    // 遷移制御: 各フェーズでアニメ終了か時間かで遷移するか選択
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("遷移条件"));
    ImGui::Checkbox(IMGUI_JP("溜め(Charge)フェーズ: アニメ終了で遷移"), &m_TransitionOnAnimEnd_Charge);
    ImGui::Checkbox(IMGUI_JP("攻撃(Attack)フェーズ: アニメ終了で遷移"), &m_TransitionOnAnimEnd_Attack);
    ImGui::Checkbox(IMGUI_JP("余韻(Exit)フェーズ: アニメ終了で遷移"), &m_TransitionOnAnimEnd_Exit);

    // コライダーウィンドウ編集
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("当たり判定ウィンドウ"));
    for (size_t i = 0; i < m_ColliderWindows.size(); ++i)
    {
        auto &w = m_ColliderWindows[i];
        ImGui::PushID((int)i);
        // 開始時刻／継続時間
        ImGui::DragFloat(IMGUI_JP("開始時刻 (秒)"), &w.Start, 0.01f, 0.0f, m_EndTime);
       // ImGui::SameLine();
        ImGui::DragFloat(IMGUI_JP("継続時間 (秒)"), &w.Duration, 0.01f, 0.0f, m_EndTime);
       // ImGui::SameLine();
        // ボーン名編集
        char buf[128];
#ifdef _MSC_VER
        strncpy_s(buf, sizeof(buf), w.BoneName.c_str(), _TRUNCATE);
#else
        std::strncpy(buf, w.BoneName.c_str(), sizeof(buf)); buf[sizeof(buf)-1] = '\0';
#endif
        if (ImGui::InputText(IMGUI_JP("ボーン名"), buf, sizeof(buf))) {
            w.BoneName = std::string(buf);
        }
        ImGui::SameLine();
        if (ImGui::Button(IMGUI_JP("削除"))) { m_ColliderWindows.erase(m_ColliderWindows.begin() + i); ImGui::PopID(); break; }
        ImGui::PopID();
        ImGui::Separator();
    }
    if (ImGui::Button(IMGUI_JP("ウィンドウを追加"))) { ColliderWindow nw; nw.Start = 0.0f; nw.Duration = 0.1f; nw.BoneName = ""; m_ColliderWindows.push_back(nw); }

    // 移動ウィンドウ編集
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("移動ウィンドウ"));
    for (size_t i = 0; i < m_MovementWindows.size(); ++i)
    {
        auto &mv = m_MovementWindows[i];
        ImGui::PushID((int)i + 1000);
        ImGui::DragFloat(IMGUI_JP("開始時刻 (秒)"), &mv.Start, 0.01f, 0.0f, m_EndTime);
        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_JP("継続時間 (秒)"), &mv.Duration, 0.01f, 0.0f, m_EndTime);
        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_JP("速度"), &mv.Speed, 0.1f, 0.0f, 1000.0f);
        //ImGui::SameLine(); 
        // Easing 選択
        int easing_idx = static_cast<int>(mv.EasingType);
        const char* easing_names[] = {
            "Liner","InSine","OutSine","InOutSine","InQuad","OutQuad","InOutQuad",
            "InCubic","OutCubic","InOutCubic","InQuart","OutQuart","InOutQuart",
            "InQuint","OutQuint","InOutQuint","InExpo","OutExpo","InOutExpo",
            "InCirc","OutCirc","InOutCirc","InBack","OutBack","InOutBack",
            "InElastic","OutElastic","InOutElastic","InBounce","OutBounce","InOutBounce"
        };
        if (ImGui::Combo(IMGUI_JP("イージング"), &easing_idx, easing_names, IM_ARRAYSIZE(easing_names))) {
            mv.EasingType = static_cast<MyEasing::Type>(easing_idx);
        }
        ImGui::SameLine();
        // 移動量係数の編集
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(IMGUI_JP("距離"), &mv.Distance, 0.01f, 0.0f, 1000.0f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Checkbox(IMGUI_JP("逆向きに移動"), &mv.Reverse);
        ImGui::SameLine();
        if (ImGui::Button(IMGUI_JP("削除"))) { m_MovementWindows.erase(m_MovementWindows.begin() + i); ImGui::PopID(); break; }
        ImGui::PopID();
    }
    if (ImGui::Button(IMGUI_JP("移動ウィンドウを追加"))) { MovementWindow mv; mv.Start = 0.0f; mv.Duration = 0.2f; mv.Speed = 10.0f; m_MovementWindows.push_back(mv); }

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
    // 読み込み前に既存ウィンドウ設定をクリアして重複を防ぐ
    m_ColliderWindows.clear();
    m_MovementWindows.clear();
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("m_AttackAmount")) m_AttackAmount = j["m_AttackAmount"].get<float>();
    if (j.contains("m_ColliderWidth")) m_ColliderWidth = j["m_ColliderWidth"].get<float>();
    if (j.contains("m_ColliderHeight")) m_ColliderHeight = j["m_ColliderHeight"].get<float>();
    if (j.contains("m_EndTime")) m_EndTime = j["m_EndTime"].get<float>();
    if (j.contains("m_AttackTime")) m_AttackTime = j["m_AttackTime"].get<float>();
    if (j.contains("m_ChargeTime")) m_ChargeTime = j["m_ChargeTime"].get<float>();
    if (j.contains("m_TransitionOnAnimEnd_Charge")) m_TransitionOnAnimEnd_Charge = j["m_TransitionOnAnimEnd_Charge"].get<bool>();
    if (j.contains("m_TransitionOnAnimEnd_Attack")) m_TransitionOnAnimEnd_Attack = j["m_TransitionOnAnimEnd_Attack"].get<bool>();
    if (j.contains("m_TransitionOnAnimEnd_Exit")) m_TransitionOnAnimEnd_Exit = j["m_TransitionOnAnimEnd_Exit"].get<bool>();
    if (j.contains("m_AttackBoneName")) m_AttackBoneName = j["m_AttackBoneName"].get<std::string>();
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
    if (j.contains("MovementWindows") && j["MovementWindows"].is_array()) {
        m_MovementWindows.clear();
        for (const auto& entry : j["MovementWindows"]) {
            if (entry.contains("start") && entry.contains("duration") && entry.contains("speed")) {
                MovementWindow mv;
                mv.Start = entry["start"].get<float>();
                mv.Duration = entry["duration"].get<float>();
                mv.Speed = entry["speed"].get<float>();
                if (entry.contains("easing")) {
                    int et = entry["easing"].get<int>();
                    mv.EasingType = static_cast<MyEasing::Type>(et);
                }
                if (entry.contains("distance")) {
                    mv.Distance = entry["distance"].get<float>();
                }
                if (entry.contains("reverse")) {
                    mv.Reverse = entry["reverse"].get<bool>();
                }
                m_MovementWindows.push_back(mv);
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
    j["m_AttackTime"] = m_AttackTime;
    j["m_ChargeTime"] = m_ChargeTime;
    j["m_TransitionOnAnimEnd_Charge"] = m_TransitionOnAnimEnd_Charge;
    j["m_TransitionOnAnimEnd_Attack"] = m_TransitionOnAnimEnd_Attack;
    j["m_TransitionOnAnimEnd_Exit"] = m_TransitionOnAnimEnd_Exit;
    j["m_AttackBoneName"] = m_AttackBoneName;
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

    j["MovementWindows"] = json::array();
    for (const auto& mv : m_MovementWindows) {
        json e;
        e["start"] = mv.Start;
        e["duration"] = mv.Duration;
        e["speed"] = mv.Speed;
        e["reverse"] = mv.Reverse;
        e["easing"] = static_cast<int>(mv.EasingType);
        e["distance"] = mv.Distance;
        j["MovementWindows"].push_back(e);
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
    j["m_AttackTime"] = m_AttackTime;
    j["m_ChargeTime"] = m_ChargeTime;
    j["m_TransitionOnAnimEnd_Charge"] = m_TransitionOnAnimEnd_Charge;
    j["m_TransitionOnAnimEnd_Attack"] = m_TransitionOnAnimEnd_Attack;
    j["m_TransitionOnAnimEnd_Exit"] = m_TransitionOnAnimEnd_Exit;
    j["m_AttackBoneName"] = m_AttackBoneName;
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

    j["MovementWindows"] = nlohmann::json::array();
    for (const auto& mv : m_MovementWindows) {
        nlohmann::json e;
        e["start"] = mv.Start;
        e["duration"] = mv.Duration;
        e["speed"] = mv.Speed;
        e["easing"] = static_cast<int>(mv.EasingType);
        j["MovementWindows"].push_back(e);
    }

    return j;
}

