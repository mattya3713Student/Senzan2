#include "BossMoveState.h"
#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossJumpOnlState/BossJumpOnlState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossShoutState/BossShoutState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossStompState/BossStompState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include <algorithm>
#include <random>

static constexpr double Move_Run_AnimSpeed = 5.0;

BossMoveState::BossMoveState(Boss* owner)
	: StateBase<Boss>(owner)
	, m_RotationAngle(0.0f)
	, m_RotationSpeed(0.1f) 
	, m_rotationDirection(1.0f)
	, m_AnimNo()
	, m_AnimTimer()
	, m_BonePos()
	, m_InitBossPos()
	, m_Phase(MovePhase::Start)
{
}

BossMoveState::~BossMoveState()
{
}

void BossMoveState::Enter()
{
	m_Timer = 0.0f;
	m_RotationAngle = 0.0f;
	m_rotationDirection = 1.0f;
	m_Phase = MovePhase::Start;

	m_pOwner->SetAnimSpeed(Move_Run_AnimSpeed);
	m_pOwner->ChangeAnim(Boss::enBossAnim::IdolToRun);

    // Load persisted settings
    LoadSettings();
}

void BossMoveState::Update()
{
	using namespace DirectX;

	// === ImGui Debug Window ===
#if _DEBUG
    ImGui::Begin(IMGUI_JP("Boss Attack Debug"));

    ImGui::Text(IMGUI_JP("=== Distance Settings ==="));
    ImGui::SliderFloat(IMGUI_JP("Near Range"), &s_NearRange, 5.0f, 50.0f);
    ImGui::SliderFloat(IMGUI_JP("Mid Range"), &s_MidRange, 20.0f, 80.0f);
    ImGui::SliderFloat(IMGUI_JP("Attack Delay(sec)"), &s_AttackDelay, 0.1f, 5.0f);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("=== Attack Settings ==="));
    for (int i = 0; i < Count; ++i)
    {
        ImGui::PushID(i);
        // 攻撃名 (日本語) と有効フラグ
        ImGui::Checkbox(s_AttackNames[i], &s_Enable[i]);
        ImGui::SameLine();
        // 重みは0..100(%表示)
        ImGui::SliderFloat(IMGUI_JP("重み (%)"), &s_Weight[i], 0.0f, 100.0f);
        // 右側に現在値をパーセント表示
        ImGui::SameLine();
        ImGui::Text("%0.0f%%", s_Weight[i]);
        ImGui::SameLine();
        ImGui::SliderFloat(IMGUI_JP("クールダウン (秒)"), &s_CooldownDefault[i], 0.0f, 10.0f);
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("=== 強制攻撃 ==="));
    const char* attackNames[] = { "ランダム", "斬り", "飛びかかり", "溜め", "叫び", "投擲" };
    ImGui::Combo(IMGUI_JP("強制攻撃"), &s_ForceAttackIndex, attackNames, IM_ARRAYSIZE(attackNames));
    s_ForceAttackIndex -= 1; // -1 = Random, 0-4 = Each attack

    ImGui::Separator();
    XMFLOAT3 debugTargetPos = m_pOwner->GetTargetPos();
    XMFLOAT3 debugBossPos = m_pOwner->GetPosition();
    float debugDist = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&debugTargetPos), XMLoadFloat3(&debugBossPos))));
    ImGui::Text(IMGUI_JP("Current Distance: %.2f"), debugDist);
    if (debugDist < s_NearRange) ImGui::TextColored(ImVec4(1,0,0,1), "-> Near");
    else if (debugDist < s_MidRange) ImGui::TextColored(ImVec4(1,1,0,1), "-> Mid");
    else ImGui::TextColored(ImVec4(0,1,0,1), "-> Far");
    
    // 読み込み / 保存 ボタン
    if (ImGui::Button(IMGUI_JP("BossMoveState 読み込み")))
    {
        LoadSettings();
    }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("BossMoveState 保存")))
    {
        SaveSettings();
    }

    ImGui::End();
#endif

	float delta = Time::GetInstance().GetDeltaTime();

	// Attack timer
	m_Timer += delta;

    // Decrease cooldowns
    for (size_t i = 0; i < m_CooldownRemaining.size(); ++i)
    {
        m_CooldownRemaining[i] -= delta;
        if (m_CooldownRemaining[i] < 0.0f) m_CooldownRemaining[i] = 0.0f;
    }

	// 1. Get position info
	XMVECTOR vBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMFLOAT3 playerPosF = m_pOwner->GetTargetPos();
	XMVECTOR vTarget = XMLoadFloat3(&playerPosF);

	// Direction vector and distance to player
	XMVECTOR vToPlayer = XMVectorSubtract(vTarget, vBossPos);
	vToPlayer = XMVectorSetY(vToPlayer, 0.0f);
	float distanceToPlayer = XMVectorGetX(XMVector3Length(vToPlayer));

	constexpr float STRAFE_RANGE = 20.0f;

	// --------------------------------------------------------
	// 2. Phase-based movement and animation
	// --------------------------------------------------------
	switch (m_Phase)
	{
	case MovePhase::Start:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::IdolToRun))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::Run);
			m_Phase = MovePhase::Run;
		}
		break;

	case MovePhase::Run:
	{
		constexpr float APPROACH_SPEED = 10.0f;
		XMVECTOR vMoveDir = XMVector3Normalize(vToPlayer);
		XMVECTOR vNewPos = XMVectorAdd(vBossPos, XMVectorScale(vMoveDir, APPROACH_SPEED * delta));

		XMFLOAT3 newPosF;
		XMStoreFloat3(&newPosF, vNewPos);
		m_pOwner->SetPosition(newPosF);
		vBossPos = vNewPos;

		if (distanceToPlayer <= STRAFE_RANGE)
		{
			m_Phase = MovePhase::Stop;
			m_pOwner->ChangeAnim(Boss::enBossAnim::RunToIdol);
		}
	}
	break;

	case MovePhase::Stop:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::RunToIdol))
		{
			m_Phase = MovePhase::Strafe;
			XMVECTOR vDirFromPlayer = XMVectorSubtract(vBossPos, vTarget);
			m_BaseAngle = atan2f(XMVectorGetX(vDirFromPlayer), XMVectorGetZ(vDirFromPlayer));
			m_RotationAngle = 0.0f;
		}
		break;

	case MovePhase::Strafe:
	{
		m_RotationAngle += static_cast<float>(m_RotationSpeed) * delta * m_rotationDirection;
		const float MAX_SWAY = XM_PIDIV4;
		if (fabsf(m_RotationAngle) > MAX_SWAY)
		{
			m_rotationDirection *= -1.0f;
			m_RotationAngle = std::clamp(m_RotationAngle, -MAX_SWAY, MAX_SWAY);
		}

		float finalAngle = m_BaseAngle + m_RotationAngle;
		XMVECTOR vOffset = XMVectorSet(
			sinf(finalAngle) * STRAFE_RANGE,
			0.0f,
			cosf(finalAngle) * STRAFE_RANGE,
			0.0f
		);
		XMVECTOR vIdealPos = XMVectorAdd(vTarget, vOffset);

		constexpr float TRACKING_DELAY = 0.7f;

		XMVECTOR vCurrentPos = XMLoadFloat3(&m_pOwner->GetPosition());
		float lerpFactor = TRACKING_DELAY * delta;
		if (lerpFactor > 1.0f) lerpFactor = 1.0f;

		XMVECTOR vNextPos = XMVectorLerp(vCurrentPos, vIdealPos, lerpFactor);

		XMFLOAT3 finalPosF;
		XMStoreFloat3(&finalPosF, vNextPos);
		finalPosF.y = m_pOwner->GetPosition().y;
		m_pOwner->SetPosition(finalPosF);

		m_pOwner->SetAnimSpeed(3.0);
		if (m_rotationDirection > 0)
			m_pOwner->ChangeAnim(Boss::enBossAnim::LeftMove);
		else
			m_pOwner->ChangeAnim(Boss::enBossAnim::RightMove);
	}
	break;
	}

	// --------------------------------------------------------
	// 3. Always face player
	// --------------------------------------------------------
	XMVECTOR vFinalBossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMVECTOR vLookAt = XMVectorSubtract(vTarget, vFinalBossPos);
	float dx = XMVectorGetX(vLookAt);
	float dz = XMVectorGetZ(vLookAt);
	float angle = atan2f(dx, dz) + DirectX::XM_PI;
	m_pOwner->SetRotationY(angle);

	// --------------------------------------------------------
	// 4. Attack selection (distance-based)
	// --------------------------------------------------------
    if (m_Timer >= s_AttackDelay)
	{
		float dist = XMVectorGetX(XMVector3Length(vLookAt));
        // Build weighted candidate list: pair(factory, weight, id)
        struct Candidate { std::function<std::unique_ptr<StateBase<Boss>>() > factory; float weight; int id; };
        std::vector<Candidate> weighted;

        auto pushCandidate = [&](int id, std::function<std::unique_ptr<StateBase<Boss>>() > factory){
            if (!s_Enable[id]) return;
            // If on cooldown, skip
            if (m_CooldownRemaining[id] > 0.0f) return;
            float w = s_Weight[id];
            // apply repeat penalty
            if (m_LastAttackId == id) w *= s_RepeatPenalty;
            if (w <= 0.0f) return;
            weighted.push_back({ factory, w, id });
        };

        // Force attack mode (override weights but still respect cooldown)
        if (s_ForceAttackIndex >= 0 && s_ForceAttackIndex <= 4)
        {
            switch (s_ForceAttackIndex)
            {
            case 0: pushCandidate(AttackId::Slash, [this]() { return std::make_unique<BossSlashState>(m_pOwner); }); break;
            case 1: pushCandidate(AttackId::Stomp, [this]() { return std::make_unique<BossStompState>(m_pOwner); }); break;
            case 3: pushCandidate(AttackId::Shout, [this]() { return std::make_unique<BossShoutState>(m_pOwner); }); break;
            case 4: pushCandidate(AttackId::Throwing, [this]() { return std::make_unique<BossThrowingState>(m_pOwner); }); break;
            }
        }
        else
        {
            // Near: Slash or Stomp
            if (dist < s_NearRange) {
                pushCandidate(AttackId::Slash, [this]() { return std::make_unique<BossSlashState>(m_pOwner); });
                pushCandidate(AttackId::Stomp, [this]() { return std::make_unique<BossStompState>(m_pOwner); });
            }
            // Mid: Shout (and possibly Charge in future)
            else if (dist < s_MidRange) {
                pushCandidate(AttackId::Shout, [this]() { return std::make_unique<BossShoutState>(m_pOwner); });
            }
            // Far: Throwing or Stomp
            else {
                pushCandidate(AttackId::Throwing, [this]() { return std::make_unique<BossThrowingState>(m_pOwner); });
                pushCandidate(AttackId::Stomp, [this]() { return std::make_unique<BossStompState>(m_pOwner); });
            }
        }

        if (!weighted.empty())
        {
            // Sum weights
            // Weights are expressed as 0..100 percentages; sum them for sampling
            float total = 0.0f;
            for (auto &c : weighted) total += c.weight;

            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(0.0f, total);
            float r = dis(gen);
            float acc = 0.0f;
            int chosenId = -1;
            std::function<std::unique_ptr<StateBase<Boss>>() > chosenFactory = nullptr;
            for (auto &c : weighted)
            {
                acc += c.weight;
                if (r <= acc)
                {
                    chosenId = c.id;
                    chosenFactory = c.factory;
                    break;
                }
            }

            if (chosenFactory)
            {
                // Set cooldown for chosen attack
                // set cooldown from defaults
                if (chosenId >= 0 && chosenId < Count)
                {
                    m_CooldownRemaining[chosenId] = s_CooldownDefault[chosenId];
                }

                m_LastAttackId = chosenId;
                m_Timer = 0.0f; // reset attack timer
                m_pOwner->GetStateMachine()->ChangeState(std::move(chosenFactory()));
                return;
            }
        }
	}
}

void BossMoveState::LateUpdate()
{
}

void BossMoveState::Draw() 
{
}

void BossMoveState::Exit() 
{
}

void BossMoveState::SetInitialAngle(float angle)
{
	m_RotationAngle = angle;
}

void BossMoveState::LoadSettings()
{
    auto dataDir = std::filesystem::path("Data") / "Json" / "Boss";
    auto filePath = dataDir / std::filesystem::path("BossMoveState.json");
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("RepeatPenalty")) s_RepeatPenalty = j["RepeatPenalty"].get<float>();
    for (int i = 0; i < Count; ++i)
    {
        // JSON では英語 ID をキーに使う
        std::string keyEnable = std::string(s_AttackIds[i]) + "_Enable";
        std::string keyWeight = std::string(s_AttackIds[i]) + "_Weight";
        std::string keyCD = std::string(s_AttackIds[i]) + "_Cooldown";
        if (j.contains(keyEnable)) s_Enable[i] = j[keyEnable].get<bool>();
        if (j.contains(keyWeight)) s_Weight[i] = j[keyWeight].get<float>();
        if (j.contains(keyCD)) s_CooldownDefault[i] = j[keyCD].get<float>();
    }
}

void BossMoveState::SaveSettings() const
{
    json j;
    j["RepeatPenalty"] = s_RepeatPenalty;
    for (int i = 0; i < Count; ++i)
    {
        // JSON では英語 ID をキーに使う
        std::string keyEnable = std::string(s_AttackIds[i]) + "_Enable";
        std::string keyWeight = std::string(s_AttackIds[i]) + "_Weight";
        std::string keyCD = std::string(s_AttackIds[i]) + "_Cooldown";
        j[keyEnable] = s_Enable[i];
        j[keyWeight] = s_Weight[i];
        j[keyCD] = s_CooldownDefault[i];
    }
    auto dataDir = std::filesystem::path("Data") / "Json" / "Boss";
    std::error_code ec;
    std::filesystem::create_directories(dataDir, ec);
    auto filePath = dataDir / std::filesystem::path("BossMoveState.json");
    FileManager::JsonSave(filePath, j);
}
