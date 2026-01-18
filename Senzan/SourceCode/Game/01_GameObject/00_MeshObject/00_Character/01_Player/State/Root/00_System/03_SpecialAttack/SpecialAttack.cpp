#include "SpecialAttack.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/04_Time/Time.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/05_InputDevice/VirtualPad.h"

namespace PlayerState {

// Constants for special attack
static constexpr float SPECIAL_ATTACK_DURATION = 3.0f;      // Total duration
static constexpr float DARKEN_DURATION = 0.5f;               // Screen darkening duration
static constexpr float CAMERA_PULLBACK_DISTANCE = 20.0f;     // Camera pull back distance
static constexpr float HITBOX_START_TIME = 0.8f;            // When hitbox becomes active
static constexpr float HITBOX_END_TIME = 2.5f;              // When hitbox deactivates
static constexpr float SPECIAL_ATTACK_DAMAGE = 50.0f;       // Special attack damage

SpecialAttack::SpecialAttack(Player* owner)
	: System(owner)
{
}
SpecialAttack::~SpecialAttack()
{
}
// ID�̎擾.
constexpr PlayerState::eID SpecialAttack::GetStateID() const
{
	return PlayerState::eID::SpecialAttack;
}
void SpecialAttack::Enter()
{
	System::Enter();

	m_CurrentTime = 0.0f;
	m_IsHitboxActive = false;

	// プレイヤーの方向を敵に向ける
	DirectX::XMFLOAT3 target_pos = m_pOwner->m_TargetPos;
	DirectX::XMVECTOR v_target_pos = DirectX::XMLoadFloat3(&target_pos);
	v_target_pos = DirectX::XMVectorSetY(v_target_pos, 0.f);
	DirectX::XMFLOAT3 player_pos = m_pOwner->GetPosition();
	DirectX::XMVECTOR v_player_pos = DirectX::XMLoadFloat3(&player_pos);
	v_player_pos = DirectX::XMVectorSetY(v_player_pos, 0.f);

	DirectX::XMVECTOR v_diff_vec = DirectX::XMVectorSubtract(v_target_pos, v_player_pos);
	v_diff_vec = DirectX::XMVector3Normalize(v_diff_vec);
	DirectX::XMFLOAT3 diff_vec; 
	DirectX::XMStoreFloat3(&diff_vec, v_diff_vec);
	m_pOwner->GetTransform()->RotateToDirection(diff_vec);

	// ゲージを消費
	m_pOwner->m_CurrentUltValue = 0.0f;

	// アニメーション設定
	m_pOwner->SetIsLoop(false);
	m_pOwner->SetAnimTime(0.0f);
	m_pOwner->SetAnimSpeed(1.0f);
	m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_0);

	// 画面暗転（タイムスケール変更）
	Time::GetInstance().SetWorldTimeScale(0.3f, DARKEN_DURATION);

	// カメラを引く
	auto& camera = CameraManager::GetInstance();
	// Note: Actual camera pull back would require camera system extension
	// For now, we'll just shake it for effect
	camera.ShakeCamera(0.3f, 3.0f);
}

void SpecialAttack::Update()
{
	System::Update();

	float deltaTime = m_pOwner->GetDelta();
	m_CurrentTime += deltaTime;

	// Hitbox activation/deactivation based on timing
	if (m_CurrentTime >= HITBOX_START_TIME && m_CurrentTime <= HITBOX_END_TIME)
	{
		if (!m_IsHitboxActive)
		{
			m_pOwner->SetAttackColliderActive(true);
			// Set special attack damage
			if (m_pOwner->m_pAttackCollider)
			{
				m_pOwner->m_pAttackCollider->SetAttackAmount(SPECIAL_ATTACK_DAMAGE);
				m_pOwner->m_pAttackCollider->SetRadius(5.0f);
				m_pOwner->m_pAttackCollider->SetHeight(10.0f);
			}
			m_IsHitboxActive = true;
		}
	}
	else if (m_IsHitboxActive)
	{
		m_pOwner->SetAttackColliderActive(false);
		m_IsHitboxActive = false;
	}

	// アニメーションの遷移
	if (m_pOwner->IsAnimEnd(Player::eAnim::SpecialAttack_0))
	{
		m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_1);
	}
	else if (m_pOwner->IsAnimEnd(Player::eAnim::SpecialAttack_1))
	{
		m_pOwner->ChangeAnim(Player::eAnim::SpecialAttack_2);
	}
	else if (m_pOwner->IsAnimEnd(Player::eAnim::SpecialAttack_2) || m_CurrentTime >= SPECIAL_ATTACK_DURATION)
	{
		// 必殺技終了、Idleに遷移
		m_pOwner->ChangeState(PlayerState::eID::Idle);
	}
}

void SpecialAttack::LateUpdate()
{
	System::LateUpdate();
}

void SpecialAttack::Draw()
{
	System::Draw();
}

void SpecialAttack::Exit()
{
	System::Exit();

	// Hitboxを確実に無効化
	m_pOwner->SetAttackColliderActive(false);
	
	// ダメージを通常に戻す
	if (m_pOwner->m_pAttackCollider)
	{
		m_pOwner->m_pAttackCollider->SetAttackAmount(10.0f);
		m_pOwner->m_pAttackCollider->SetRadius(1.0f);
		m_pOwner->m_pAttackCollider->SetHeight(3.0f);
	}
}
} // PlayerState.
