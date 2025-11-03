#include "BossSpecialState.h"
// 必須ヘッダー
#include <DirectXMath.h> 
#include <cmath> 
#include <memory> // std::make_shared に必要

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

using namespace DirectX;

//---------------------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------------------
BossSpecialState::BossSpecialState(Boss* owner)
	: BossAttackStateBase(owner)
    , m_CurrentPhase(Phase::Charge)
    , m_PhaseTime(0.0f)
    , m_AttackTime(0.0f)
    , m_HasHit(false)
    // 時間設定
    , m_ChargeDuration(0.5f)      // 溜め時間
    , m_JumpDuration(0.8f)      // ジャンプ移動時間
    , m_AttackDuration(0.5f)      // 突進斬り時間
    , m_CoolDownDuration(1.0f)      // 硬直時間
    // Jump パラメータ
    , m_JumpHeght(5.0f)      // 最大到達高度 (地表からの高さ)
    // Attack パラメータ
    , m_AttackSpeed(20.0f)     // 突進の速度 (未使用、突進ロジックで使用予定)
    , m_SlashRange(1.0f)      // 突進斬りの判定幅 (未使用、突進ロジックで使用予定)
    // 位置と方向
    , m_StartPos()
    , m_JumpTargetPos()
    , m_AttackDir()
{
}

BossSpecialState::~BossSpecialState()
{
}

//---------------------------------------------------------------------
// Enter
//---------------------------------------------------------------------
void BossSpecialState::Enter()
{
    m_Attacktime = 0.0f;
    m_PhaseTime = 0.0f;
    m_CurrentPhase = Phase::Charge;
    m_HasHit = false;

    // 1. ボスの向きを設定（チャージ開始時にプレイヤーの方向を向く）
    const XMFLOAT3 BossPosF = m_pOwner->GetPosition();
    XMVECTOR BossPosXM = XMLoadFloat3(&BossPosF);

    const XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
    XMVECTOR PlayerPosXM = XMLoadFloat3(&PlayerPosF);

    XMVECTOR Direction = XMVectorSubtract(PlayerPosXM, BossPosXM);
    Direction = XMVectorSetY(Direction, 0.0f); // XZ平面の方向ベクトル

    // Y軸回転角度を計算し、ボスをプレイヤーに向かせる
    float dx = XMVectorGetX(Direction);
    float dz = XMVectorGetZ(Direction);
    float angle_radian = std::atan2f(dx, dz);
    m_pOwner->SetRotationY(angle_radian);

    // 2. 攻撃開始位置を保存 (m_StartPos)
    XMStoreFloat3(&m_StartPos, BossPosXM);

    // 3. Jumpフェーズの目標地点 (突進の始点) を計算
    // ターゲット地点: プレイヤーから 1.0m 後ろ (手前) の上空 4.0m
    XMVECTOR dirToPlayerXZ = XMVectorSubtract(PlayerPosXM, BossPosXM);
    dirToPlayerXZ = XMVectorSetY(dirToPlayerXZ, 0.0f);
    dirToPlayerXZ = XMVector3Normalize(dirToPlayerXZ);

    XMVECTOR TargetOffset = XMVectorScale(dirToPlayerXZ, -1.0f); // プレイヤー方向と逆 (手前)
    XMVECTOR TargetPosXM = XMVectorAdd(PlayerPosXM, TargetOffset);
    TargetPosXM = XMVectorSetY(TargetPosXM, 4.0f); // 高度 4.0m

    // m_JumpTargetPos に保存
    XMStoreFloat3(&m_JumpTargetPos, TargetPosXM);

    // 位置を m_StartPos に固定 (Chargeフェーズ開始)
    m_pOwner->SetPosition(m_StartPos);
}

//---------------------------------------------------------------------
// Update (フェーズ遷移とジャンプ軌道ロジック)
//---------------------------------------------------------------------
void BossSpecialState::Update()
{
    const float deltaTime = Time::GetInstance().GetDeltaTime();
    const float Time_Rate = 1.0f;

    // 時間の更新
    m_Attacktime += Time_Rate * deltaTime;
    m_PhaseTime += deltaTime;

    switch (m_CurrentPhase)
    {
    case Phase::Charge:
        if (m_PhaseTime >= m_ChargeDuration)
        {
            m_PhaseTime -= m_ChargeDuration;
            m_CurrentPhase = Phase::Jump;
        }
        break;

    case Phase::Jump:
    {
        float t = m_PhaseTime / m_JumpDuration; // t は 0.0f -> 1.0f
        DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&m_StartPos);
        DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&m_JumpTargetPos);

        // 1. 水平移動: start から target へ線形に移動
        DirectX::XMVECTOR xzPos = DirectX::XMVectorLerp(start, target, t);

        // 2. 垂直移動: 放物線を描く
        float currentY = (DirectX::XMVectorGetY(start) * (1.0f - t)) + (DirectX::XMVectorGetY(target) * t);
        // m_JumpHeght は m_JumpHeight の誤字と仮定して修正
        float jumpCurve = 4.0f * m_JumpHeght * t * (1.0f - t);

        DirectX::XMVECTOR newPos = DirectX::XMVectorSetY(xzPos, currentY + jumpCurve);

        // ボスの位置を更新
        DirectX::XMFLOAT3 newPosF;
        DirectX::XMStoreFloat3(&newPosF, newPos);
        m_pOwner->SetPosition(newPosF);

        if (m_PhaseTime >= m_JumpDuration)
        {
            // ジャンプ完了 -> 突進斬りフェーズへ
            m_PhaseTime -= m_JumpDuration;
            m_CurrentPhase = Phase::Attack;

            // Attackフェーズの開始時に突進方向を決定（現在のボスの位置からプレイヤーへ）
            DirectX::XMFLOAT3 playerPosXM = m_pOwner->m_PlayerPos; // ターゲットのポジションを使用
            DirectX::XMVECTOR v_playerPosXM = DirectX::XMLoadFloat3(&playerPosXM); // ターゲットのポジションを使用
            DirectX::XMFLOAT3 currentBossPos = m_pOwner->GetPosition();
            DirectX::XMVECTOR v_currentBossPos = DirectX::XMLoadFloat3(&currentBossPos);

            // プレイヤーへ向かう方向ベクトル
            DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(v_playerPosXM, v_currentBossPos);
            dir = DirectX::XMVector3Normalize(dir);
            DirectX::XMStoreFloat3(&m_AttackDir, dir); // 方向を保存
        }
    }
    break;

    case Phase::Attack:
        // 突進移動ロジック
        BossAttack();
    {
        DirectX::XMFLOAT3  currentBossPos = m_pOwner->GetPosition();
        DirectX::XMVECTOR v_currentBossPos = DirectX::XMLoadFloat3(&currentBossPos);
        DirectX::XMVECTOR attackDir = DirectX::XMLoadFloat3(&m_AttackDir);

        // 速度 * 時間 = 移動量 (V * dt)
        DirectX::XMVECTOR moveDelta = DirectX::XMVectorScale(attackDir, m_AttackSpeed * deltaTime);
        DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(v_currentBossPos, moveDelta);

        // ボスの位置を更新
        DirectX::XMFLOAT3 newPosF;
        DirectX::XMStoreFloat3(&newPosF, newPos);
        m_pOwner->SetPosition(newPosF);

        // 攻撃判定の実行
        BossAttack();

        // 突進時間が終了したら Cooldown へ
        if (m_PhaseTime >= m_AttackDuration)
        {
            m_PhaseTime -= m_AttackDuration;
            m_CurrentPhase = Phase::Cooldown;
            m_HasHit = false; // 次の攻撃に備えてリセット
        }
    }
    break;

    case Phase::Cooldown:
        // 硬直時間終了
        if (m_PhaseTime >= m_CoolDownDuration)
        {
            // 地面に確実に着地させる処理 (Exit でも良い)
            DirectX::XMFLOAT3 posF = m_pOwner->GetPosition();
            m_pOwner->SetPosition({ posF.x, 0.0f, posF.z });

            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_pOwner));
            return;
        }
        break;
    }
}
//---------------------------------------------------------------------
// BossAttack (未実装)
//---------------------------------------------------------------------
void BossSpecialState::BossAttack()
{
    // ★ 突進中の移動と当たり判定ロジックを実装予定
}

void BossSpecialState::LateUpdate() {}
void BossSpecialState::Draw() {}
void BossSpecialState::Exit() {}