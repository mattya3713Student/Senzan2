#include "AttackCombo_1.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/05_InputDevice/VirtualPad.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"
#include "System/Singleton/ImGui/CImGuiManager.h"

// 攻撃開始までの速度.
static constexpr double AttackCombo_0_ANIM_SPEED_0 = 0.04;

static constexpr float CLOSE_RANGE_THRESHOLD = 20.0f;	// Bossまでの距離に置いて近いと判断する.

// デバッグ用に値を弄れるように static 変数などで管理（またはクラスメンバに追加）
static float g_1DebugAnimSpeed0 = 3.4f;
static float g_1DebugMaxTime = 2.3f;
static float g_1DebugComboStartTime = 1.8f; // 受付開始（例：踏み込み終わりのタイミング）
static float g_1DebugComboEndTime = 3.8f; // 受付終了（例：アニメーション終了の少し前）

namespace PlayerState {
AttackCombo_1::AttackCombo_1(Player* owner)
	: Combat(owner)
	, m_MoveVec()
	, m_isComboAccepted(false)
	{
	}
	AttackCombo_1::~AttackCombo_1()
	{
	}
	
	// IDの取得.
	constexpr PlayerState::eID AttackCombo_1::GetStateID() const
	{
		return PlayerState::eID::AttackCombo_1;
	}

	void AttackCombo_1::Enter()
	{
		Combat::Enter();
		m_isComboAccepted = false; // フラグをリセット.
		m_currentTime = 0.0f;      // 時間をリセット.


		// アニメーション設定.
		m_MaxTime = g_1DebugMaxTime;

		// アニメーション設定.
		m_pOwner->SetIsLoop(false);
		m_pOwner->SetAnimTime(0.0);
		m_pOwner->SetAnimSpeed(g_1DebugAnimSpeed0); // デバッグ値を使用
		m_pOwner->ChangeAnim(Player::eAnim::Attack_1);

		// 距離算出用座標.
		DirectX::XMFLOAT3 target_pos = m_pOwner->m_TargetPos;
		DirectX::XMVECTOR v_target_pos = DirectX::XMLoadFloat3(&target_pos);
		v_target_pos = DirectX::XMVectorSetY(v_target_pos, 0.f);
		DirectX::XMFLOAT3 player_pos = m_pOwner->GetPosition();
		DirectX::XMVECTOR v_player_pos = DirectX::XMLoadFloat3(&player_pos);
		v_player_pos = DirectX::XMVectorSetY(v_player_pos, 0.f);

		// 距離算出.
		DirectX::XMVECTOR v_Lenght = {};
		DirectX::XMFLOAT3 diff_vec = {};
		DirectX::XMVECTOR v_diff_vec = DirectX::XMVectorSubtract(v_target_pos, v_player_pos);
		v_Lenght = DirectX::XMVector3Length(v_diff_vec);
		DirectX::XMStoreFloat(&m_Distance, v_Lenght);
		v_diff_vec = DirectX::XMVector3Normalize(v_diff_vec);
		DirectX::XMStoreFloat3(&diff_vec, v_diff_vec);

		// 敵の方向を向く.
		m_pOwner->GetTransform()->RotateToDirection(diff_vec);

		// 入力を取得.
		DirectX::XMFLOAT2 input_vec = VirtualPad::GetInstance().GetAxisInput(VirtualPad::eGameAxisAction::Move);

		// 少し近づく.
		DirectX::XMVECTOR v_small_move = DirectX::XMVectorScale(v_diff_vec, 0.1f);
		DirectX::XMStoreFloat3(&m_MoveVec, v_small_move);
		Log::GetInstance().Info("", "近い");
	}
	void AttackCombo_1::Update()
	{
		Combat::Update();

	
		if (m_currentTime >= g_1DebugComboStartTime && m_currentTime <= g_1DebugComboEndTime)
		{
			if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Attack))
			{
				if (!m_isComboAccepted) {
					m_isComboAccepted = true;
					Log::GetInstance().Info("", "コンボ1：入力受付成功！");
				}
			}
		}

		// --- ステート遷移判定 ---
		// Attack_1 の終了を判定する
		if (m_currentTime >= m_MaxTime)
		{
			if (m_isComboAccepted) {
				// 次のコンボ(AttackCombo_2)があるならここを有効化
				m_pOwner->ChangeState(PlayerState::eID::AttackCombo_2);
				Log::GetInstance().Info("", "次のコンボ(2)へ遷移予定");
			}
			else {
				m_pOwner->ChangeState(PlayerState::eID::Idle);
			}
		}

#if 1
		// --- ImGui デバッグメニュー (AttackCombo_1専用) ---
		ImGui::Begin("AttackCombo_1 Debug"); // ← 1に変更

		ImGui::Text("Current Time: %.3f / %.3f", m_currentTime, m_MaxTime);
		ImGui::ProgressBar(m_currentTime / m_MaxTime);

		// 受付ウィンドウの視覚化
		bool isInsideWindow = (m_currentTime >= g_1DebugComboStartTime && m_currentTime <= g_1DebugComboEndTime);
		if (isInsideWindow) {
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "WINDOW: OPEN");
		}
		else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "WINDOW: CLOSED");
		}
		ImGui::Checkbox("Combo Accepted", &m_isComboAccepted);
		if(ImGui::Button("Start"))
		{
			m_pOwner->SetAnimTime(0.0);
		}

		ImGui::Separator();
		ImGui::Text("Parameters");
		ImGui::SliderFloat("Anim Speed (Start)", &g_1DebugAnimSpeed0, 0.0f, 20.0f);
		ImGui::SliderFloat("Action Duration (MaxTime)", &g_1DebugMaxTime, 0.1f, 10.0f);

		if (ImGui::Button("Reset & Execute Again"))
		{
			this->Enter();
		}

		ImGui::End();
#endif

#if 0

		// --- ImGui デバッグメニュー ---
		ImGui::Begin("AttackCombo_0 Debug");

		// 時間と受付状態の可視化
		ImGui::Text("Time: %.3f / %.3f", m_currentTime, m_MaxTime);
		bool isInsideWindow = (m_currentTime >= g_1DebugComboStartTime && m_currentTime <= g_1DebugComboEndTime);

		if (isInsideWindow) {
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "WINDOW: OPEN (Push Attack!)");
		}
		else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "WINDOW: CLOSED");
		}

		// 先行入力フラグの状態を表示
		ImGui::Checkbox("Combo Accepted", &m_isComboAccepted);

		ImGui::Separator();
		ImGui::Text("Input Window Settings");
		ImGui::SliderFloat("Combo Start (sec)", &g_1DebugComboStartTime, 0.0f, g_1DebugMaxTime);
		ImGui::SliderFloat("Combo End (sec)", &g_1DebugComboEndTime, 0.0f, g_1DebugMaxTime);

		// ... 既存のデバッグ項目 ...
		if (ImGui::Button("Reset & Execute Again")) { this->Enter(); }

		ImGui::End();
#endif
	}

	void AttackCombo_1::LateUpdate()
	{
		Combat::LateUpdate();

		// 経過時間を加算.
		float actual_anim_speed = m_pOwner->m_AnimSpeed;
		float delta_time = actual_anim_speed * m_pOwner->GetDelta();
		m_currentTime += delta_time;

		// 移動量の算出.
		float movement_speed = m_Distance / m_MaxTime;
		float move_amount = movement_speed * delta_time;

		// 移動方向.
		DirectX::XMFLOAT3 moveDirection = { m_MoveVec.x, 0.0f, m_MoveVec.z };

		// 移動量加算.
		DirectX::XMFLOAT3 movement = {};
		movement.x = moveDirection.x * move_amount;
		movement.y = 0.f;
		movement.z = moveDirection.z * move_amount;

		m_pOwner->AddPosition(movement);
	}

	void AttackCombo_1::Draw()
	{
		Combat::Draw();
	}

	void AttackCombo_1::Exit()
	{
		Combat::Exit();
		m_MoveVec = {};
	}
} // PlayerState.
