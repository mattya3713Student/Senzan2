#include "AttackCombo_0.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/05_InputDevice/VirtualPad.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"


// 攻撃開始までの速度.
static constexpr double AttackCombo_0_ANIM_SPEED_0 = 0.04;

// 攻撃中の速度.
static constexpr double AttackCombo_0_ANIM_SPEED_1 = 0.1;

namespace PlayerState {
AttackCombo_0::AttackCombo_0(Player* owner)
	: Combat	(owner)
	, m_MoveVec	()
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

	m_MaxTime = 3.067f;

	// アニメーション設定.
	m_pOwner->SetIsLoop(false);
	m_pOwner->SetAnimSpeed(AttackCombo_0_ANIM_SPEED_0);
	m_pOwner->ChangeAnim(Player::eAnim::Attack_0);

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

	// 入力を取得.
	DirectX::XMFLOAT2 input_vec = VirtualPad::GetInstance().GetAxisInput(VirtualPad::eGameAxisAction::Move);


	m_pOwner->GetTransform()->RotateToDirection(diff_vec);
	// 入力があれば敵へダッシュ！.
	if (MyMath::IsVector2NearlyZero(input_vec, 0.f)) {

	}
}

void AttackCombo_0::Update()
{
	Combat::Update();
}

void AttackCombo_0::LateUpdate()
{
	Combat::LateUpdate();

	// 経過時間を加算.
	float actual_anim_speed = MyMath::IsNearlyEqual(m_pOwner->m_AnimSpeed, 0.0) ? AttackCombo_0_ANIM_SPEED_1 : m_pOwner->m_AnimSpeed;
	float delta_time = actual_anim_speed * m_pOwner->GetDelta();
	m_currentTime += delta_time;

	// アニメーション速度を切り替え.
	if(m_currentTime > 1.0f)
		m_AnimSpeedChangedTrigger->CheckAndTrigger(
			[this]() { return  m_pOwner->SetAnimSpeed(AttackCombo_0_ANIM_SPEED_1); },
			[&]() {
				return false;
			});

	float movement_speed = m_Distance / m_MaxTime;
	float move_amount = movement_speed * delta_time;

	// 移動方向.
	DirectX::XMFLOAT3 moveDirection = { m_pOwner->m_TargetPos.x, 0.0f, m_pOwner->m_TargetPos.y };

	// 移動量加算.
	DirectX::XMFLOAT3 movement = {};
	movement.x = moveDirection.x * movement_speed;
	movement.y = 0.f;
	movement.z = moveDirection.z * movement_speed;

	//m_pOwner->AddPosition(movement);

	Log::GetInstance().Info("", m_pOwner->GetPosition());

	// 攻撃_1完了.
	if (m_pOwner->IsAnimEnd(Player::eAnim::Attack_0))
	{
		m_pOwner->ChangeState(PlayerState::eID::Idle);
	}
}

void AttackCombo_0::Draw()
{
	Combat::Draw();
}

void AttackCombo_0::Exit()
{
	Combat::Exit();
}

} // PlayerState.