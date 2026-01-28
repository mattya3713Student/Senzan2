#include "SpecialAttack.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/04_Time/Time.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/ParryManager/ParryManager.h"

namespace PlayerState {
SpecialAttack::SpecialAttack(Player* owner)
	: System(owner)
    , m_AttackDuration(90.0f * m_pOwner->GetDelta())
    , m_LastAttackMove(2.0f)
{
}
SpecialAttack::~SpecialAttack()
{
}
// IDの取得.
constexpr PlayerState::eID SpecialAttack::GetStateID() const
{
	return PlayerState::eID::SpecialAttack;
}
void SpecialAttack::Enter()
{
    m_CurrentTime = 0.0f;
    m_HasActivated = false;

    // ゲージを消費
    m_pOwner->m_CurrentUltValue = 0.0f;

    // 必殺技アニメーション開始
    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimSpeed(5.0f);
    m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_0);

    // 時間スケールを遅くして演出
    Time::GetInstance().SetWorldTimeScale(0.3f, 0.5f);

    // カメラ演出（シェイク）
    CameraManager::GetInstance().ShakeCamera(0.2f, 0.5f);
    m_pOwner->PlayEffectAtWorldPos("Special", m_pOwner->GetPosition(), 8.f);

    // 移動量処理.
    DirectX::XMFLOAT3 target_pos = m_pOwner->m_TargetPos;
    DirectX::XMVECTOR v_target_pos = DirectX::XMLoadFloat3(&target_pos);
    v_target_pos = DirectX::XMVectorSetY(v_target_pos, 0.f);
    DirectX::XMFLOAT3 player_pos = m_pOwner->GetPosition();
    DirectX::XMVECTOR v_player_pos = DirectX::XMLoadFloat3(&player_pos);
    v_player_pos = DirectX::XMVectorSetY(v_player_pos, 0.f);

    DirectX::XMVECTOR v_diff_vec = DirectX::XMVectorSubtract(v_target_pos, v_player_pos);
    DirectX::XMVECTOR v_Lenght = DirectX::XMVector3Length(v_diff_vec);
    DirectX::XMStoreFloat(&m_Distance, v_Lenght);
    v_diff_vec = DirectX::XMVector3Normalize(v_diff_vec);
    DirectX::XMFLOAT3 diff_vec; DirectX::XMStoreFloat3(&diff_vec, v_diff_vec);
    m_pOwner->GetTransform()->RotateToDirection(diff_vec);

    m_pOwner->m_MoveVec = diff_vec;
}
void SpecialAttack::Update()
{
    float deltaTime = m_pOwner->GetDelta();
    m_CurrentTime += deltaTime;
    m_DurationTimer += deltaTime;
    if (m_pOwner->IsAnimEnd(Player::eAnim::SpecialAttack_0)) {
        m_pOwner->SetAnimSpeed(5.0f);
        m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_1);
    }

    // 攻撃判定発生（演出中盤）
    if(m_DurationTime < m_DurationTimer && m_CurrentTime <= m_AttackDuration)
    {
        ParryManager::GetInstance().DamageToBoss(m_OraOraDamage);
        m_pOwner->m_Combo++;
        m_DurationTimer = 0.0f;
        SoundManager::GetInstance().Play("Damage");
        SoundManager::GetInstance().SetVolume("Damage", 8500);
        if (m_pOwner->IsAnimEnd(Player::eAnim::SpecialAttack_1) && m_pOwner->IsAnimEnd(Player::eAnim::SpecialAttack_2)) {
            m_pOwner->SetAnimSpeed(1.0f);
            m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_2);
        }
    }
    else if (!m_HasActivated && m_CurrentTime >= m_AttackDuration)
    {
        // 最後の一撃の移動処理.
        DirectX::XMFLOAT3 moveDirection = { m_pOwner->m_MoveVec.x, 0.0f, m_pOwner->m_MoveVec.z };
        DirectX::XMFLOAT3 movement = {};
        movement.x = moveDirection.x * m_Distance * m_LastAttackMove;
        movement.y = 0.f;
        movement.z = moveDirection.z * m_Distance * m_LastAttackMove;
        m_pOwner->AddPosition(movement);



        m_HasActivated = true;
        m_pOwner->PlayEffectAtWorldPos("Special2", m_pOwner->GetPosition(), 8.f);
        ParryManager::GetInstance().DamageToBoss(m_AttackDamage);
        SoundManager::GetInstance().Play("Throw");
        SoundManager::GetInstance().SetVolume("Throw", 8500);
        SoundManager::GetInstance().Play("BreakSnow");
        SoundManager::GetInstance().SetVolume("BreakSnow", 8500);
        SoundManager::GetInstance().Play("Hit2");
        SoundManager::GetInstance().SetVolume("Hit2", 8500);
    }

    // 演出終了
    if (m_CurrentTime >= m_AttackDuration)
    {
        m_pOwner->ChangeState(PlayerState::eID::Idle);
    }
}
void SpecialAttack::LateUpdate()
{
}
void SpecialAttack::Draw()
{
}
void SpecialAttack::Exit()
{
    // 攻撃判定を無効化
    m_pOwner->SetAttackColliderActive(false);
    m_pOwner->m_pAttackCollider->SetAttackAmount(10.0f);  // 通常ダメージに戻す
}
} // PlayerState.
