#include "AttackCombo_1.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/05_InputDevice/VirtualPad.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"
#include "System/Singleton/ImGui/CImGuiManager.h"

// 攻撃開始までの速度.
static constexpr double AttackCombo_0_ANIM_SPEED_0 = 0.04;

static constexpr float CLOSE_RANGE_THRESHOLD = 20.0f;    // Bossまでの距離に置いて近いと判断する.

// デバッグ用に値を弄れるように static 変数などで管理（またはクラスメンバに追加）
static float g_1DebugAnimSpeed0 = 2.0f;
static float g_1DebugAnimSpeed1 = 2.8f; // LateUpdateでデフォルトとして使うアニメーション速度（デバッグ）
static float g_1DebugMaxTime = 1.2f;
static float g_1DebugComboStartTime = 0.7f; // 受付開始（例：踏み込み終わりのタイミング）
static float g_1DebugComboEndTime = 2.4f; // 受付終了（例：アニメーション終了の少し前）

namespace PlayerState {
AttackCombo_1::AttackCombo_1(Player* owner)
    : Combat(owner)
    , m_MoveVec()
    , m_isComboAccepted(false)
{
    // default collider windows
    ClearColliderWindows();
    AddColliderWindow(0.8f, 0.1f);
    AddColliderWindow(1.5f, 0.1f);
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

    // 当たり判定を無効化（ステート側で自動切替）.
    m_pOwner->SetAttackColliderActive(false);

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
    // --- ImGui デバッグメニュー (AttackCombo_1) ---
    ImGui::Begin(IMGUI_JP("AttackCombo_1 デバッグ"));

    // 停止フラグ（trueでロジックを一時停止）
    static bool isStop1 = false;
    ImGui::Checkbox(IMGUI_JP("ストップ"), &isStop1);

    // コンボ強制フラグ（手動で入力を模擬）
    static bool forceAccept = false;
    ImGui::Checkbox(IMGUI_JP("強制でコンボ受付する"), &forceAccept);

    // Collider ウィンドウリスト表示・編集 via Combat base
    RenderColliderWindowsUI();

    // ロジックを実行するか制御
    if (!isStop1)
    {
        Combat::Update();

        // 受付ウィンドウ判定
        if (m_currentTime >= g_1DebugComboStartTime && m_currentTime <= g_1DebugComboEndTime)
        {
            if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Attack) || forceAccept)
            {
                if (!m_isComboAccepted) {
                    m_isComboAccepted = true;
                    Log::GetInstance().Info("", "コンボ1：入力受付成功！");
                }
            }
        }

        // Process collider windows using Combat helper
        ProcessColliderWindows(m_currentTime);

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
    }
    else
    {
        ImGui::Text(IMGUI_JP("停止中"));
    }

    // デバッグUI: 時間・ウィンドウ・速度など
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("時間: %.3f / %.3f"), m_currentTime, m_MaxTime);
    bool isInsideWindow = (m_currentTime >= g_1DebugComboStartTime && m_currentTime <= g_1DebugComboEndTime);
    if (isInsideWindow) ImGui::TextColored(ImVec4(0,1,0,1), IMGUI_JP("受付ウィンドウ: 開"));
    else ImGui::TextColored(ImVec4(1,0,0,1), IMGUI_JP("受付ウィンドウ: 閉"));

    ImGui::Checkbox(IMGUI_JP("コンボ受付済み"), &m_isComboAccepted);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("入力ウィンドウ設定"));
    ImGui::SliderFloat(IMGUI_JP("開始時間 (秒)"), &g_1DebugComboStartTime, 0.0f, g_1DebugMaxTime);
    ImGui::SliderFloat(IMGUI_JP("終了時間 (秒)"), &g_1DebugComboEndTime, 0.0f, g_1DebugMaxTime);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("アニメーション設定 (デバッグ)"));
    ImGui::DragFloat(IMGUI_JP("アニメ速度"), &g_1DebugAnimSpeed0, 0.1f, 0.1f, 60.0f);
    ImGui::DragFloat(IMGUI_JP("ステート持続時間 (MaxTime)"), &g_1DebugMaxTime, 0.01f, 0.1f, 10.0f);
    ImGui::Text(IMGUI_JP("現在のアニメ速度: %.3f"), static_cast<float>(m_pOwner->m_AnimSpeed));
    if (ImGui::Button(IMGUI_JP("現在のアニメ速度を適用"))) {
        m_pOwner->SetAnimSpeed(g_1DebugAnimSpeed0);
    }

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("コライダー設定 (ステート経過時間制御)"));
    ImGui::Text(IMGUI_JP("現在のステート経過時間: %.3f"), m_currentTime);

    ImGui::Separator();
    if (ImGui::Button(IMGUI_JP("リセットして再実行"))) { this->Enter(); }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("アイドルへ移行"))) { m_pOwner->ChangeState(PlayerState::eID::Idle); }

    ImGui::End();
}

void AttackCombo_1::LateUpdate()
{
    Combat::LateUpdate();

    // 経過時間を加算.
    float delta_time = m_pOwner->GetDelta();
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
    // 当たり判定を無効化.
    m_pOwner->SetAttackColliderActive(false);
}
} // PlayerState.
