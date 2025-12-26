#include "BossStateBase.h"
#include "..//Boss.h"
#include <cmath>
#include <algorithm>

BossStateBase::BossStateBase(Boss* pOwner)
    : StateBase<Boss>(pOwner)
{
}

// ボスへのアクセサ
Boss* BossStateBase::GetBoss() const
{
    return m_pOwner;
}

// プレイヤーの方に常に向く
void BossStateBase::RotateToPlayer(float RotationSpeed)
{
    if (!m_pOwner) return;

    // ボスとプレイヤーの位置取得
    DirectX::XMFLOAT3 BossPos = m_pOwner->GetPosition();
    DirectX::XMFLOAT3 PlayerPos = m_pOwner->GetTargetPos();

    float dx = PlayerPos.x - BossPos.x;
    float dz = PlayerPos.z - BossPos.z;

    // 距離が極端に近い場合は処理しない
    if (std::abs(dx) < 0.001f && std::abs(dz) < 0.001f) return;

    // 目標角度の計算（ラジアンから度数へ）
    float TargetAngleRad = std::atan2f(dx, dz);
    float TargetAngleDeg = TargetAngleRad * (180.0f / DirectX::XM_PI);

    RotateToTarget(TargetAngleDeg, RotationSpeed);
}

// ターゲットの角度へ回転
void BossStateBase::RotateToTarget(float TargetRote, float RotationSpeed)
{
    if (!m_pOwner || !m_pOwner->GetTransform()) return;

    // 現在の角度取得
    DirectX::XMFLOAT3 CurrentRotation = m_pOwner->GetTransform()->GetRotationDegrees();
    float CurrentRote = CurrentRotation.y;
    float deltaTime = m_pOwner->GetDelta();

    // 角度の正規化（0~360度）
    TargetRote = MyMath::NormalizeAngleDegrees(TargetRote);
    CurrentRote = MyMath::NormalizeAngleDegrees(CurrentRote);

    // 最短距離計算
    float AngleDiff = TargetRote - CurrentRote;
    if (AngleDiff > 180.0f)  AngleDiff -= 360.0f;
    if (AngleDiff < -180.0f) AngleDiff += 360.0f;

    float MaxRotate = RotationSpeed * deltaTime;

    if (std::abs(AngleDiff) <= MaxRotate)
    {
        CurrentRotation.y = TargetRote;
    }
    else
    {
        CurrentRotation.y += (AngleDiff > 0) ? MaxRotate : -MaxRotate;
    }

    // 更新
    CurrentRotation.y = MyMath::NormalizeAngleDegrees(CurrentRotation.y);
    m_pOwner->GetTransform()->SetRotationDegrees(CurrentRotation);
}