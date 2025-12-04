#include "PlayerStateBase.h"
#include "../Player.h"    
#include "Root/Root.h"    

#include "Game/04_Time/Time.h"    

static constexpr float DEFAULT_ROTATION_SPEED = 720.0f;

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

    // 移動していない場合は、向きを変えない.
    if (MyMath::IsVector3NearlyZero(move_vec, 0.0f)) {
        return;
    }

    float target_angle_rad = std::atan2f(move_vec.x, move_vec.z);
    float target_angle_deg = target_angle_rad * (180.0f / DirectX::XM_PI);

    RotetToTarget(target_angle_deg, DEFAULT_ROTATION_SPEED);
}


// 目標方向へラープ回転.
void PlayerStateBase::RotetToTarget(float TargetRote, float RotetionSpeed)
{
    DirectX::XMFLOAT3 current_rotation = m_pOwner->GetTransform()->GetRotationDegrees();
    float CurrentRote = current_rotation.y;
    float deltaTime = Time::GetInstance().GetDeltaTime();

    // 角度を正規化.
    TargetRote = MyMath::NormalizeAngleDegrees(TargetRote);
    CurrentRote = MyMath::NormalizeAngleDegrees(CurrentRote);

    // 回転角度の差を計算し、最短回転量に正規化.
    float AngleDiff = TargetRote - CurrentRote;
    AngleDiff = MyMath::NormalizeAngleDegrees(AngleDiff);
    float max_rotate_amount = RotetionSpeed * deltaTime;

    // 適用.
    if (std::fabsf(AngleDiff) <= max_rotate_amount)
    {
        // 差が1フレームの移動量以下なら、直接目標角度を設定
        current_rotation.y = TargetRote;
    }
    else
    {
        // 目標の方向に向かって max_rotate_amount 分だけ回転
        current_rotation.y += (AngleDiff > 0)
            ? max_rotate_amount : -max_rotate_amount;

        // 再度正規化する.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
        current_rotation.y = MyMath::NormalizeAngleDegrees(current_rotation.y);
    }

    m_pOwner->GetTransform()->SetRotationDegrees(current_rotation);
}

