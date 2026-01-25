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
    // 初期設定をファイルから読み込む（存在すれば上書き）
    try { LoadSettings(); } catch (...) {}
}

void BossStompState::DrawImGui()
{
#if _DEBUG
    ImGui::Begin(IMGUI_JP("BossStomp State"));
    CImGuiManager::Slider<float>(IMGUI_JP("ジャンプ力"), m_JumpPower, 0.0f, 200.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("重力"), m_Gravity, 0.0f, 10.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("上昇速度倍率"), m_UpSpeed, 0.1f, 5.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("前方移動速度"), m_ForwardSpeed, 0.0f, 30.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("遅延秒数"), m_WaitSeconds, 0.0f, 5.0f, true);
    // legacy pre-slow removed
    CImGuiManager::Slider<float>(IMGUI_JP("遅延中アニメ速度"), m_SlowAnimSpeed, 0.0f, 2.0f, true);
    // pre-slow removed; use m_WaitSeconds / m_SlowAnimSpeed
    // 上昇/落下パラメータは一時無効化
    // CImGuiManager::Slider<float>(IMGUI_JP("上昇時間"), m_AscentTime, 0.0f, 5.0f, true);
    // CImGuiManager::Slider<float>(IMGUI_JP("落下水平加速"), m_FallAccel, 0.0f, 200.0f, true);
    // CImGuiManager::Slider<float>(IMGUI_JP("落下最大速度"), m_MaxFallHorizSpeed, 0.0f, 200.0f, true);
    // 手動の保存/読み込みボタン
    if (ImGui::Button(IMGUI_JP("Load"))) {
        try { LoadSettings(); }
        catch (...) {}
    }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("Save"))) {
        try { SaveSettings(); }
        catch (...) {}
    }

    BossAttackStateBase::DrawImGui();
    ImGui::End();
#endif
}

void BossStompState::LoadSettings()
{
    // まず基底の設定を読み込む
    BossAttackStateBase::LoadSettings();

    // 設定ファイルは Data/Json/Boss 以下に保存する
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        filePath = std::filesystem::current_path() / "Data" / "Json" / "Boss" / filePath;
    }
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("JumpPower")) m_JumpPower = j["JumpPower"].get<float>();
    if (j.contains("Gravity")) m_Gravity = j["Gravity"].get<float>();
    if (j.contains("UpSpeed")) m_UpSpeed = j["UpSpeed"].get<float>();
    if (j.contains("ForwardSpeed")) m_ForwardSpeed = j["ForwardSpeed"].get<float>();
    if (j.contains("WaitSeconds")) m_WaitSeconds = j["WaitSeconds"].get<float>();
    if (j.contains("SlowAnimSpeed")) m_SlowAnimSpeed = j["SlowAnimSpeed"].get<float>();
    // no additional parameters
    // オフセットは ColliderWindow で管理
}

void BossStompState::SaveSettings() const
{
    // ベース設定を取得して派生設定を追加して Data/Json/Boss に保存
    json j = SerializeSettings();
    j["JumpPower"] = m_JumpPower;
    j["Gravity"] = m_Gravity;
    j["UpSpeed"] = m_UpSpeed;
    j["ForwardSpeed"] = m_ForwardSpeed;
    j["WaitSeconds"] = m_WaitSeconds;
    j["SlowAnimSpeed"] = m_SlowAnimSpeed;
    // no additional parameters

    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        auto dir = std::filesystem::current_path() / "Data" / "Json" / "Boss";
        std::filesystem::create_directories(dir);
        filePath = dir / filePath;
    }
    FileManager::JsonSave(filePath, j);
}

BossStompState::~BossStompState()
{
}

void BossStompState::Enter()
{
	m_Velocity = { 0.0f, 0.0f, 0.0f };
	m_GroundedFrag = true;
	m_HasLanded = false;
    // 初期化: タイマーとフラグをリセット
    m_AnimSlowed = false;
    m_IsMoving = false;
    m_StateTimer = 0.0f;

    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimTime(0.0);

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
    // 呼び出し位置: 基底の Update を先に実行して ImGui を表示・デバッグ停止を反映する
    BossAttackStateBase::Update();

	auto* pStompCollider = m_pOwner->GetStompCollider();

	switch (m_List)
	{
	case BossStompState::enAttack::None:
		// 指定秒数経過でアニメをスローにする
		m_StateTimer += Time::GetInstance().GetDeltaTime();
		if (!m_AnimSlowed && m_StateTimer >= m_WaitSeconds) {
			m_AnimSlowed = true;
			m_pOwner->SetAnimSpeed(m_SlowAnimSpeed);
		}

		// Special_0 が終了したら座標移動を開始して Special_1 を再生
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0) && !m_IsMoving)
		{
			m_IsMoving = true;
			m_pOwner->SetAnimSpeed(1.0f);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
			m_List = enAttack::Stomp;
			// setup movement easing parameters
			m_MoveTimer = 0.0f;
			// compute horizontal distance & direction toward player
			DirectX::XMFLOAT3 bossPos = m_pOwner->GetPosition();
			DirectX::XMFLOAT3 playerPos = m_pOwner->m_PlayerPos;
			playerPos.y = 0.0f; bossPos.y = 0.0f;
			DirectX::XMVECTOR vBoss = DirectX::XMLoadFloat3(&bossPos);
			DirectX::XMVECTOR vPlayer = DirectX::XMLoadFloat3(&playerPos);
			DirectX::XMVECTOR vDiff = DirectX::XMVectorSubtract(vPlayer, vBoss);
			float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(vDiff));
			m_Distance = dist;
			DirectX::XMStoreFloat3(&m_MoveVec, DirectX::XMVector3Normalize(DirectX::XMVectorSetY(vDiff, 0.0f)));
			if (pStompCollider) {
				pStompCollider->SetRadius(30.0f);
				pStompCollider->SetAttackAmount(15.0f);
			}
		}
		break;

	case BossStompState::enAttack::Stomp:
		// 移動中のみ BossAttack を実行
		if (m_IsMoving) {
			// advance move timer
			m_MoveTimer += Time::GetInstance().GetDeltaTime();
			// run BossAttack which now performs eased movement based on m_MoveTimer/m_MoveDuration
			BossAttack();
		}

		// Special_1 を再生し終わったら移動を止めて SpecialToIdol を再生
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_1) && m_IsMoving)
		{
			m_IsMoving = false;
            m_pOwner->SetAnimSpeed(2.0f);
			m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
			m_List = enAttack::CoolTime;
			// 無効化されていた stomp コライダーをオフ
			auto* pStompCollider = m_pOwner->GetStompCollider();
			if (pStompCollider) pStompCollider->SetActive(false);
		}


		// (処理を上に統合したため削除)
		break;
	case BossStompState::enAttack::CoolTime:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SpecialToIdol))
		{
           m_List = enAttack::Trans; // Transition to the next state
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
	// 保存: 実行時に ImGui 等で変更した設定を永続化
	try { SaveSettings(); } catch (...) {}
	// m_pOwner->SetPositionY(0.0f);
}

void BossStompState::BossAttack()
{
    float dt = Time::GetInstance().GetDeltaTime();

    // progress for movement easing (0..1)
    float progress = m_MoveDuration > 0.0f ? (m_MoveTimer / m_MoveDuration) : 1.0f;
    if (progress > 1.0f) progress = 1.0f;

    // easeOut quad: 1 - (1-x)^2
    auto easeOut = [](float x) { return 1.0f - (1.0f - x) * (1.0f - x); };

    float eased = easeOut(progress);
    float prev = easeOut(std::max(0.0f, progress - dt / (m_MoveDuration > 0.0f ? m_MoveDuration : 1.0f)));

    float frameDistance = m_Distance * (eased - prev);

    DirectX::XMFLOAT3 movement = {
        m_MoveVec.x * frameDistance,
        0.0f,
        m_MoveVec.z * frameDistance
    };

    m_pOwner->AddPosition(movement);

    // If movement finished, ensure flags / list updated
    if (progress >= 1.0f)
    {
        m_IsMoving = false;
        // trigger transition to SpecialToIdol if not already
        if (m_List == enAttack::Stomp)
        {
            m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
            m_List = enAttack::CoolTime;
            auto* pStompCollider = m_pOwner->GetStompCollider();
            if (pStompCollider) pStompCollider->SetActive(false);
        }
    }
}
