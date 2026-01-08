#include "AttackCombo_0.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/05_InputDevice/VirtualPad.h"
#include "Resource/Mesh/02_Skin/SkinMesh.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"
#include "System/Singleton/ImGui/CImGuiManager.h"


// 攻撃開始までの速度.
static constexpr double AttackCombo_0_ANIM_SPEED_0 = 0.04; 

static constexpr float CLOSE_RANGE_THRESHOLD = 30.0f;    // Bossまでの距離に置いて近いと判断する.


static float g_DebugAnimSpeed0 = 2.f; // Enter時に設定するアニメーション速度（デバッグ）
static float g_DebugAnimSpeed1 = 2.8f; // LateUpdateでデフォルトとして使うアニメーション速度（デバッグ）
static float g_DebugMaxTime = 3.f;
static float g_DebugComboStartTime = 1.4f; // 受付開始（例：踏み込み終わりのタイミング）
static float g_DebugComboEndTime = 2.7f; // 受付終了（例：アニメーション終了の少し前）

namespace PlayerState {
AttackCombo_0::AttackCombo_0(Player* owner)
    : Combat    (owner)
    , m_MoveVec  ()
    , m_isComboAccepted    ( false )
    , m_isAttackColliderEnabled(false)
{

    m_EnableByAnimTime = 1.5f;
        m_EnableDurationAnim = 0.5f;
}

AttackCombo_0::~AttackCombo_0()
{
}

// IDの取得.
constexpr PlayerState::eID AttackCombo_0::GetStateID() const
{
    return PlayerState::eID::DodgeExecute;
}

void AttackCombo_0::Enter()
{
    Combat::Enter();

    m_isComboAccepted = false; // フラグをリセット.
    m_currentTime = 0.0f;      // 時間をリセット.
    m_MaxTime = 3.f;

    // アニメーション設定.
    m_MaxTime = g_DebugMaxTime;

    // アニメーション設定（ここでデバッグ用の速度を適用）.
    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimTime(0.0);
    m_pOwner->SetAnimSpeed(g_DebugAnimSpeed0); // デバッグ値を使用
    m_pOwner->ChangeAnim(Player::eAnim::Attack_0);

    // (既定) 当たり判定を無効化にしておき、自動でアニメーションに合わせて切り替える
    m_pOwner->SetAttackColliderActive(false);

    // リセット: 一度だけ切り替えフラグを初期化
    m_HasActivatedCollider = false;
    m_HasDeactivatedCollider = false;

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
 
    // 距離によって近づく.
    if (m_Distance < CLOSE_RANGE_THRESHOLD)
    {
        // 近いなら少し.
        DirectX::XMVECTOR v_small_move = DirectX::XMVectorScale(v_diff_vec, 0.1f);
        DirectX::XMStoreFloat3(&m_MoveVec, v_small_move);
        Log::GetInstance().Info("", "近い");
    }
    else
    {
        // 遠い且つ入力があれば敵へダッシュ!.
        if (!MyMath::IsVector2NearlyZero(input_vec, 0.f)) {
            m_MoveVec = diff_vec;
            Log::GetInstance().Info("", "遠い");
        }
    }
}

void AttackCombo_0::Update()
{
    // --- ImGui デバッグメニュー ---
    ImGui::Begin(IMGUI_JP("AttackCombo_0 デバッグ"));

    // 停止フラグ（trueでロジックを一時停止）
    static bool isStop = false;
    ImGui::Checkbox(IMGUI_JP("ストップ"), &isStop);

    // ロジックを実行するか制御
    if (!isStop)
    {
        Combat::Update();
        // --- 先行入力の判定 ---
        // 指定された時間内に攻撃ボタンが押されたか判定
        if (m_currentTime >= g_DebugComboStartTime && m_currentTime <= g_DebugComboEndTime)
        {
            if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Attack))
            {
                if (!m_isComboAccepted) {
                    m_isComboAccepted = true;
                }
            }
        }

        if (m_currentTime > 2.3f && m_isComboAccepted)
        {
            m_pOwner->ChangeState(PlayerState::eID::AttackCombo_1);
        }

        // 何も入力がなくアニメーションが終わった時,Idleステートに変える.
        if (m_currentTime >= m_MaxTime)
        {
            m_pOwner->ChangeState(PlayerState::eID::Idle);
        }
    }
    else
    {
        ImGui::Text(IMGUI_JP("停止中"));
    }

#if 1

    // 時間と受付状態の可視化
    ImGui::Text(IMGUI_JP("時間: %.3f / %.3f"), m_currentTime, m_MaxTime);
    bool isInsideWindow = (m_currentTime >= g_DebugComboStartTime && m_currentTime <= g_DebugComboEndTime);

    if (isInsideWindow) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), IMGUI_JP("受付ウィンドウ: 開 (攻撃を押してください)"));
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), IMGUI_JP("受付ウィンドウ: 閉"));
    }

    // 先行入力フラグの状態を表示
    ImGui::Checkbox(IMGUI_JP("コンボ受付済み"), &m_isComboAccepted);

    ImGui::Separator();
    ImGui::Text(IMGUI_JP("入力ウィンドウ設定"));
    ImGui::SliderFloat(IMGUI_JP("開始時間 (秒)"), &g_DebugComboStartTime, 0.0f, g_DebugMaxTime);
    ImGui::SliderFloat(IMGUI_JP("終了時間 (秒)"), &g_DebugComboEndTime, 0.0f, g_DebugMaxTime);

    // アニメーション速度の設定
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("アニメーション設定 (デバッグ)"));
    ImGui::DragFloat(IMGUI_JP("開始時のアニメ速度"), &g_DebugAnimSpeed0, 0.1f, 0.1f, 60.0f);
    ImGui::DragFloat(IMGUI_JP("通常時のアニメ速度"), &g_DebugAnimSpeed1, 0.1f, 0.1f, 60.0f);
    ImGui::Text(IMGUI_JP("現在のアニメ速度: %.3f"), static_cast<float>(m_pOwner->m_AnimSpeed));
    if (ImGui::Button(IMGUI_JP("現在のアニメ速度を適用"))) {
        m_pOwner->SetAnimSpeed(g_DebugAnimSpeed0);
    }
    // コライダー設定.
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("コライダー設定 (ステート経過時間制御)"));
    ImGui::DragFloat(IMGUI_JP("開始時刻 (ステート秒)"), &m_EnableByAnimTime, 0.01f, 0.0f, m_MaxTime);
    ImGui::DragFloat(IMGUI_JP("継続時間 (ステート秒)"), &m_EnableDurationAnim, 0.01f, 0.0f, m_MaxTime);
    // 現在のステート経過時間を表示
    ImGui::Text(IMGUI_JP("現在のステート経過時間: %.3f"), m_currentTime);

    ImGui::Separator();
    if (ImGui::Button(IMGUI_JP("リセットして再実行"))) { this->Enter(); }

    // デバッグ: 強制的にIdle状態へ遷移
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("アイドルへ移行")))
    {
        m_pOwner->ChangeState(PlayerState::eID::Idle);
    }

    ImGui::End();
#endif
}

void AttackCombo_0::LateUpdate()
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

    // -- 当たり判定の遅延有効化/無効化処理（常にステート経過時間 m_currentTime で制御）
    {
        bool shouldEnable = (m_currentTime >= m_EnableByAnimTime && m_currentTime <= (m_EnableByAnimTime + m_EnableDurationAnim));

        // m_currentTime が開始時間を越えたとき一度だけ有効化
        if (shouldEnable && !m_HasActivatedCollider)
        {
            m_HasActivatedCollider = true;
            m_HasDeactivatedCollider = false;
            m_isAttackColliderEnabled = true;
            m_pOwner->SetAttackColliderActive(true);
        }

        // m_currentTime が 継続終了後 を越えたとき一度だけ無効化
        if (!shouldEnable && !m_HasDeactivatedCollider && m_HasActivatedCollider)
        {
            m_HasDeactivatedCollider = true;
            m_HasActivatedCollider = false;
            m_isAttackColliderEnabled = false;
            m_pOwner->SetAttackColliderActive(false);
        }
    }

    // もし手動オーバーライド中でない場合、自動での当たり判定切り替えは上記で行われる。
}

void AttackCombo_0::Draw()
{
    Combat::Draw();
}

void AttackCombo_0::Exit()
{
    Combat::Exit();
    m_MoveVec = {};
    // 当たり判定を無効化.
    m_pOwner->SetAttackColliderActive(false);
}

} // PlayerState.

