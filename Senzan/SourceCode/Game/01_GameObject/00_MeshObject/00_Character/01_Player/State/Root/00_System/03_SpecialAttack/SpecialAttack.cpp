#include "SpecialAttack.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/04_Time/Time.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/ParryManager/ParryManager.h"

namespace PlayerState {
SpecialAttack::SpecialAttack(Player* owner)
	: System(owner)
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
    m_pOwner->SetAnimSpeed(1.0);
    m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_0);

    // 時間スケールを遅くして演出
    Time::GetInstance().SetWorldTimeScale(0.3f, 0.5f);

    // カメラ演出（シェイク）
    CameraManager::GetInstance().ShakeCamera(0.2f, 0.5f);
    m_pOwner->PlayEffectAtWorldPos("Special", m_pOwner->GetPosition(), 15.f);
}
void SpecialAttack::Update()
{
    float deltaTime = m_pOwner->GetDelta();
    m_CurrentTime += deltaTime;

    // 攻撃判定発生（演出中盤）
    if (!m_HasActivated && m_CurrentTime >= m_AttackDuration * 0.5f)
    {
        m_HasActivated = true;
        m_pOwner->SetAttackColliderActive(true);
        m_pOwner->m_pAttackCollider->SetAttackAmount(m_AttackDamage);
    }

    {
        ParryManager::GetInstance().DamageToBoss(10000.0f);
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
