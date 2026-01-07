#include "AttackCombo_0.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/05_InputDevice/VirtualPad.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Singleton/Debug/AnimationTuningManager/AnimationTuningManager.h"


// 攻撃開始までの速度.
static constexpr double AttackCombo_0_ANIM_SPEED_0 = 0.04; 

static constexpr float CLOSE_RANGE_THRESHOLD = 10.0f;    // Bossまでの距離に置いて近いと判断する.


static float g_DebugAnimSpeed0 = 10.f;
static float g_DebugAnimSpeed1 = 2.8f;
static float g_DebugMaxTime = 3.067f;
static float g_DebugComboStartTime = 0.5f; // 受付開始（例：踏み込み終わりのタイミング）
static float g_DebugComboEndTime = 2.5f; // 受付終了（例：アニメーション終了の少し前）

namespace PlayerState {
AttackCombo_0::AttackCombo_0(Player* owner)
    : Combat    (owner)
    , m_MoveVec  ()
    , m_isComboAccepted  ( false )
{
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

    const std::string stateKey = "AttackCombo_0";
    if (auto def = m_pOwner->GetActionDefinition(stateKey))
    {
        m_MaxTime = (def->totalDuration > 0.0f) ? def->totalDuration : g_DebugMaxTime;
        m_pOwner->ApplyActionDefinition(*def);
    }
    else
    {

        m_isComboAccepted = false; // フラグをリセット.
        m_currentTime = 0.0f;      // 時間をリセット.
        m_MaxTime = 3.067f;

        // アニメーション設定.
        m_MaxTime = g_DebugMaxTime;

        // アニメーション設定.
        m_pOwner->SetIsLoop(false);
        m_pOwner->SetAnimTime(0.0);
        m_pOwner->SetAnimSpeed(g_DebugAnimSpeed0); // デバッグ値を使用
        m_pOwner->ChangeAnim(Player::eAnim::Attack_0);

        // 当たり判定を有効化.
        // CImGuiManager に保存されたスケジュールがあれば使用し、なければデフォルト値を使用.
        CImGuiManager::ColliderSchedule sched;
        if (CImGuiManager::GetColliderSchedule(stateKey, 0, sched))
        {
            m_pOwner->ScheduleAttackCollider(static_cast<Character::AttackTypeId>(Player::AttackType::Normal), 0, sched.Delay, sched.Duration);
        }
        else
        {
            // デフォルト: 前と同じデフォルト値
            m_pOwner->ScheduleAttackCollider(static_cast<Character::AttackTypeId>(Player::AttackType::Normal), 0, 0.0f, 0.12f);
        }
    }

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
    m_pOwner->m_MoveVec = diff_vec;
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
    Combat::Update();

    if (m_pOwner->IsAnimEnd(Player::eAnim::Attack_0))
    {
        m_pOwner->ChangeState(PlayerState::eID::Idle);
    }
    // --- 先行入力の判定 ---
    // 指定された時間内に攻撃ボタンが押されたか判定
    if (m_currentTime >= g_DebugComboStartTime && m_currentTime <= g_DebugComboEndTime)
    {
        if (VirtualPad::GetInstance().IsActionDown(VirtualPad::eGameAction::Attack))
        {
            if (!m_isComboAccepted) {
                m_isComboAccepted = true;
                m_pOwner->ChangeState(PlayerState::eID::AttackCombo_1);
            }
        }
    }

    // 何も入力がなくアニメーションが終わった時,Idleステートに変える.
    if (m_currentTime >= m_pOwner->GetAnimPeriod(static_cast<int>(Player::eAnim::Attack_0)))
    {
        m_pOwner->ChangeState(PlayerState::eID::Idle);
    }

    // --- ImGui デバッグメニュー ---
    if (AnimationTuningManager::GetInstance().BeginStateDebugWindow("AttackCombo_0 Debug"))
    {
        // 時間と受付状態の可視化
        ImGui::Text("Time: %.3f / %.3f", m_currentTime, m_MaxTime);
        bool isInsideWindow = (m_currentTime >= g_DebugComboStartTime && m_currentTime <= g_DebugComboEndTime);

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
        ImGui::SliderFloat("Combo Start (sec)", &g_DebugComboStartTime, 0.0f, g_DebugMaxTime);
        ImGui::SliderFloat("Combo End (sec)", &g_DebugComboEndTime, 0.0f, g_DebugMaxTime);

        ImGui::Separator();
        ImGui::Text("Collider Schedule (Delay / Duration)");

        // ここで CImGuiManager のスケジュールを読み出して表示・編集する.
        CImGuiManager::ColliderSchedule sched;
        const std::string stateKey = "AttackCombo_0";
        if (CImGuiManager::GetColliderSchedule(stateKey, 0, sched))
        {
            float delay = sched.Delay;
            float duration = sched.Duration;
            if (ImGui::SliderFloat("Collider Delay (sec)", &delay, 0.0f, g_DebugMaxTime))
            {
                sched.Delay = delay;
                CImGuiManager::SetColliderSchedule(stateKey, 0, sched);
            }
            if (ImGui::SliderFloat("Collider Duration (sec)", &duration, 0.0f, g_DebugMaxTime))
            {
                sched.Duration = duration;
                CImGuiManager::SetColliderSchedule(stateKey, 0, sched);
            }
        }
        else
        {
            // no schedule stored yet: show and allow creation
            float delay = 0.0f;
            float duration = 0.12f;
            if (ImGui::SliderFloat("Collider Delay (sec)", &delay, 0.0f, g_DebugMaxTime))
            {
                CImGuiManager::ColliderSchedule newSched{ delay, duration };
                CImGuiManager::SetColliderSchedule(stateKey, 0, newSched);
            }
            if (ImGui::SliderFloat("Collider Duration (sec)", &duration, 0.0f, g_DebugMaxTime))
            {
                CImGuiManager::ColliderSchedule newSched{ delay, duration };
                CImGuiManager::SetColliderSchedule(stateKey, 0, newSched);
            }
        }

        if (ImGui::Button("Reschedule Collider Now"))
        {
            CImGuiManager::ColliderSchedule s2;
            if (CImGuiManager::GetColliderSchedule(stateKey, 0, s2))
            {
                m_pOwner->ScheduleAttackCollider(static_cast<Character::AttackTypeId>(Player::AttackType::Normal), 0, s2.Delay, s2.Duration);
            }
            else
            {
                m_pOwner->ScheduleAttackCollider(static_cast<Character::AttackTypeId>(Player::AttackType::Normal), 0, 0.0f, 0.12f);
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Reset & Execute Again")) { this->Enter(); }

        AnimationTuningManager::GetInstance().EndStateDebugWindow();
    }
}

void AttackCombo_0::LateUpdate()
{
    Combat::LateUpdate();

    // 経過時間を加算.
    float actual_anim_speed = MyMath::IsNearlyEqual(m_pOwner->m_AnimSpeed, 0.0) ? g_DebugAnimSpeed1 : static_cast<float>(m_pOwner->m_AnimSpeed);
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

void AttackCombo_0::Draw()
{
    Combat::Draw();
}

void AttackCombo_0::Exit()
{
    Combat::Exit();
    m_MoveVec = {};
}

} // PlayerState.

