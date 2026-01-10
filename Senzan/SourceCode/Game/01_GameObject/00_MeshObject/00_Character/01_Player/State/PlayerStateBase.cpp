#include "PlayerStateBase.h"
#include "../Player.h"      

#include "Game/04_Time/Time.h"    

#include "System/Singleton/Debug/Log/DebugLog.h"    

static constexpr float DEFAULT_ROTATION_SPEED = 360.0f * 2.f;

PlayerStateBase::PlayerStateBase(Player* owner)
	: StateBase <Player>(owner)
{
}

Player* PlayerStateBase::GetPlayer() const
{
    return m_pOwner;
}

// 正面へラープ回転.
void PlayerStateBase::RotetToFront()
{
    DirectX::XMFLOAT3 move_vec = m_pOwner->m_MoveVec;

    // 小さなノイズを吸収するためのイプシロン
    constexpr float kMoveEpsilon = 1e-4f;
    if (MyMath::IsVector3NearlyZero(move_vec, kMoveEpsilon)) {
        return;
    }

    // atan2f(x, y) の引数順: 0度 = +Y軸方向
    // もし 0度 = +X軸を期待する場合は atan2f(y, x) に変更してください
    float target_angle_rad = std::atan2f(move_vec.x, move_vec.y);
    float target_angle_deg = target_angle_rad * (180.0f / DirectX::XM_PI);

    RotetToTarget(target_angle_deg, DEFAULT_ROTATION_SPEED);
}


// 目標方向へラープ回転.
void PlayerStateBase::RotetToTarget(float TargetRote, float RotetionSpeed)
{
    DirectX::XMFLOAT3 current_rotation = m_pOwner->GetTransform()->GetRotationDegrees();
    float CurrentRote = current_rotation.y;
    float deltaTime = m_pOwner->GetDelta();

    // 正規化.
    TargetRote = MyMath::NormalizeAngleDegrees(TargetRote);
    CurrentRote = MyMath::NormalizeAngleDegrees(CurrentRote);

    // 正規化.
    float angle_Diff = TargetRote - CurrentRote;
    angle_Diff = MyMath::NormalizeAngleDegrees(angle_Diff);
    float max_rotate_amount = RotetionSpeed * deltaTime;

    current_rotation.y = CurrentRote;

    // 適用.
    if (std::fabsf(angle_Diff) <= max_rotate_amount)
    {
        // 差が1フレームの移動量以下なら、直接目標角度を設定
        current_rotation.y = TargetRote;
    }
    else
    {
        // 目標の方向に向かって max_rotate_amount 分だけ回転
        current_rotation.y += (angle_Diff > 0)
            ? max_rotate_amount : -max_rotate_amount;
    }

    // 正規化.
    current_rotation.y = MyMath::NormalizeAngleDegrees(current_rotation.y);

    m_pOwner->GetTransform()->SetRotationDegrees(current_rotation);
}

