#include "SpecialAttack.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/04_Time/Time.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/CombatCoordinator/CombatCoordinator.h"

namespace PlayerState {
SpecialAttack::SpecialAttack(Player* owner)
	: System(owner)
    , m_AttackDuration(250.0f * m_pOwner->GetDelta())
    , m_EffectDuration(110.0f * m_pOwner->GetDelta())
    , m_HoldTime      (200.0f * m_pOwner->GetDelta())
    , m_LastAttackMove(1.1f)
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
    m_pOwner->SetAnimSpeed(15.0f);
    m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_0);

    // 時間スケールを遅くして演出
    Time::GetInstance().SetWorldTimeScale(0.3f, 0.5f);

    // カメラ演出（シェイク）
    CameraManager::GetInstance().ShakeCamera(0.2f, 0.5f);
    m_pOwner->PlayEffectAtWorldPos("Special", m_pOwner->GetPosition(), 8.f);
    m_pOwner->SetSpecial(true);
}

void SpecialAttack::Update()
{
    float deltaTime = m_pOwner->GetDelta();
    m_CurrentTime += deltaTime;
    m_DurationTimer += deltaTime;

    if (!m_Anim1Changed && m_CurrentTime <= m_EffectDuration) {
        m_pOwner->SetAnimSpeed(5.0f);
        m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_1);
        m_Anim1Changed = true;
    }

    // 攻撃判定発生（演出中盤）
    if(m_DurationTime < m_DurationTimer && m_CurrentTime <= m_EffectDuration)
    {
        CombatCoordinator::GetInstance().DamageToBoss(m_OraOraDamage);
        CombatCoordinator::GetInstance().HitSpecialAttackToBoss();
        m_pOwner->m_Combo += 3;
        m_DurationTimer = 0.0f;
        SoundManager::GetInstance().Play("Damage");
        SoundManager::GetInstance().SetVolume("Damage", 8500);
    }
    else if (!m_HasMoved && m_CurrentTime >= m_EffectDuration)
    {
        if (!m_Anim2Changed) {
            m_pOwner->SetAnimSpeed(3.f);
            m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_2);
            m_Anim2Changed = true;
        }
        m_HasMoved = true;

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

        // 向きベクトル（正規化済み）
        float rotY = std::atan2(diff_vec.x, diff_vec.z);
        m_pOwner->GetTransform()->SetRotationY(rotY);

        m_pOwner->m_MoveVec = diff_vec;
        DirectX::XMFLOAT3 moveDirection = { m_pOwner->m_MoveVec.x, 0.0f, m_pOwner->m_MoveVec.z };
        DirectX::XMFLOAT3 movement = {
            moveDirection.x * m_Distance * m_LastAttackMove,
            0.f,
            moveDirection.z * m_Distance * m_LastAttackMove };
        m_pOwner->AddPosition(movement);
    }
    else if (!m_HasActivated && m_CurrentTime >= m_HoldTime)
    {
        m_HasActivated = true;
        m_pOwner->PlayEffectAtWorldPos("Special2", m_pOwner->GetPosition(), 8.f);
        CombatCoordinator::GetInstance().DamageToBoss(m_AttackDamage);
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
    m_pOwner->SetSpecial(false);
    // 攻撃判定を無効化
    m_pOwner->SetAttackColliderActive(false);
    m_pOwner->m_pAttackCollider->SetAttackAmount(10.0f);  // 通常ダメージに戻す

    m_CurrentTime = 0.f;
    m_DurationTimer = 0.f;
    m_Anim1Changed = false;
    m_Anim2Changed = false;
    m_HasMoved     = false;
    m_HasActivated = false;
} 
}// PlayerState.
