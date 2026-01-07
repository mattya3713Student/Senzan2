#include "PlayerStateBase.h"
#include "../Player.h"    
#include "Root/Root.h"    

#include "Game/04_Time/Time.h"    

#include "System/Singleton/Debug/Log/DebugLog.h"    
#include "Game/01_GameObject/00_MeshObject/00_Character/ActionData.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

static constexpr float DEFAULT_ROTATION_SPEED = 720.0f;

PlayerStateBase::PlayerStateBase(Player* owner)
	: StateBase <Player>(owner)
{
}

Player* PlayerStateBase::GetPlayer() const
{
    return m_pOwner;
}

void PlayerStateBase::SetActionDefinition(const ActionStateDefinition* def)
{
    m_ActionDefinition = def;
    m_ActionElapsed = 0.0f;
}

void PlayerStateBase::ResetActionTimeline()
{
    m_ActionElapsed = 0.0f;
}

void PlayerStateBase::UpdateActionTimeline(float dt)
{
    if (!m_ActionDefinition) return;

    m_ActionElapsed += dt;

    for (const auto& evt : m_ActionDefinition->colliderEvents)
    {
        const float endTime = evt.startTime + evt.duration;
        const bool active = (m_ActionElapsed >= evt.startTime) && (m_ActionElapsed < endTime);

        // 仕様反映.
        if (ColliderBase* collider = m_pOwner->GetColliderByIndex(evt.type, evt.index))
        {
            if (auto capsule = dynamic_cast<CapsuleCollider*>(collider))
            {
                capsule->SetRadius(evt.spec.Radius);
                capsule->SetHeight(evt.spec.Height);
            }
            collider->SetPositionOffset(evt.spec.Offset);
            collider->SetAttackAmount(evt.spec.AttackAmount);
            collider->SetMyMask(static_cast<eCollisionGroup>(evt.spec.MyMask));
            collider->SetTarGetTargetMask(static_cast<eCollisionGroup>(evt.spec.TargetMask));
            collider->SetColor(evt.spec.DebugColor);
        }

        m_pOwner->SetColliderActive(evt.type, evt.index, active);

        if (active)
        {
            DirectX::XMFLOAT3 bonePos{};
            DirectX::XMFLOAT3 boneForward{};
            if (!evt.boneName.empty() && m_pOwner->GetBoneWorldPose(evt.boneName, bonePos, boneForward))
            {
                m_pOwner->SetColliderTransform(evt.type, evt.index, bonePos, boneForward);
            }
            else
            {
                // ボーン指定なし: オフセットのみ反映済みなので何もしない.
            }
        }
        else
        {
            // 非アクティブ時は明示的な処理なし.
        }
    }

    if (m_ActionDefinition->totalDuration > 0.0f && m_ActionElapsed >= m_ActionDefinition->totalDuration)
    {
        // 派生ステート側で遷移を行うためのフラグとして利用できるよう、ここではリセットのみ。
        m_ActionElapsed = m_ActionDefinition->totalDuration;
    }
}

// 正面へラープ回転.
void PlayerStateBase::RotetToFront()
{
    DirectX::XMFLOAT3 move_vec = m_pOwner->m_MoveVec;

    // 移動していない場合は、向きを変えない.
    if (MyMath::IsVector3NearlyZero(move_vec, 0.0f, 0.1f)) {
        return;
    }

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
        // 差が1フレームの移動量以下なら、直接目標角度を設定.
        current_rotation.y = TargetRote;
    }
    else
    {
        // 目標の方向に向かって max_rotate_amount 分だけ回転.
        current_rotation.y += (angle_Diff > 0)
            ? max_rotate_amount : -max_rotate_amount;
    }

    // 正規化.
    current_rotation.y = MyMath::NormalizeAngleDegrees(current_rotation.y);

    m_pOwner->GetTransform()->SetRotationDegrees(current_rotation);
}

