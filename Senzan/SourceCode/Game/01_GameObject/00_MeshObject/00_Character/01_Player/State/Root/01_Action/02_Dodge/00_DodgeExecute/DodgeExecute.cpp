#include "DodgeExecute.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"
#include "../Dodge.h"

#include "Game/05_InputDevice/VirtualPad.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/ImGui/CImGuiManager.h"

#include "System/Utility/SingleTrigger/SingleTrigger.h"

// ‰ñ”ð’†‚Ì‘¬“x.
static constexpr double DODGE_ANIM_SPEED_1 = 0.00001;

namespace PlayerState {
DodgeExecute::DodgeExecute(Player* owner)
	: Dodge(owner)
	, m_IsAnimEnd(false)
{
}

DodgeExecute::~DodgeExecute()
{
}

// ID‚ÌŽæ“¾.
constexpr PlayerState::eID DodgeExecute::GetStateID() const
{
	return PlayerState::eID::DodgeExecute;
}

void DodgeExecute::Enter()
{
	Dodge::Enter();

	m_Distance = 250.f;
	m_MaxTime = 3.8f;

	m_pOwner->SetIsLoop(false);
	m_pOwner->SetAnimSpeed(DODGE_ANIM_SPEED_1);
	m_pOwner->SetAnimTime(1.5);
	m_pOwner->ChangeAnim(Player::eAnim::Attack_0);
}

void DodgeExecute::Update()
{
	Dodge::Update();
}

void DodgeExecute::LateUpdate()
{
	Dodge::LateUpdate();

	// Œo‰ßŽžŠÔ‚ð‰ÁŽZ.
	double value = MyMath::IsNearlyEqual(m_pOwner->m_AnimSpeed, 0.0) ? 1.0 : m_pOwner->m_AnimSpeed;
	float deltaTime = value * m_pOwner->GetDelta();
	float speed = m_Distance / m_MaxTime;
	float moveAmount = speed * deltaTime;

	m_currentTime += deltaTime;

	// ˆÚ“®•ûŒü.
	DirectX::XMFLOAT3 moveDirection = { m_InputVec.x, 0.0f, m_InputVec.y };

	// ˆÚ“®—Ê‰ÁŽZ.
	DirectX::XMFLOAT3 movement = {};
	movement.x = moveDirection.x * moveAmount;
	movement.y = 0.f;
	movement.z = moveDirection.z * moveAmount;

	m_pOwner->AddPosition(movement);

	// ‰ñ”ðŠ®—¹.
	if (m_currentTime >= m_MaxTime)
	{
		m_IsAnimEnd = true;
		m_pOwner->ChangeState(PlayerState::eID::Idle);
	}
}

void DodgeExecute::Draw()
{
	Dodge::Draw();
}

void DodgeExecute::Exit()
{
	Dodge::Exit();
}

} // PlayerState.