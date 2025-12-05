#include "JustDodge.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"
#include "../Dodge.h"


static constexpr double JUSTDODGE_ANIM_SPEED = 0.03;
namespace PlayerState {
JustDodge::JustDodge(Player* owner)
	: Dodge(owner)
{
}
JustDodge::~JustDodge()
{
}

// ID‚ÌŽæ“¾.
constexpr PlayerState::eID JustDodge::GetStateID() const
{
	return PlayerState::eID::JustDodge;
}

void JustDodge::Enter()
{
    Dodge::Enter();

	m_Distance = 250.f;
	m_MaxTime = 10.8f;

    Time::GetInstance().SetWorldTimeScale(0.01f, 2.5f);

    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimSpeed(JUSTDODGE_ANIM_SPEED);
    m_pOwner->ChangeAnim(Player::eAnim::Dodge);
}

void JustDodge::Update()
{
    Dodge::Update();

	float deltaTime = m_pOwner->GetDelta();
	float speed = m_Distance / m_MaxTime;
	float moveAmount = speed * deltaTime;

	Log::GetInstance().Info("a", deltaTime);

	// Œo‰ßŽžŠÔ‚ð‰ÁŽZ.
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
		m_pOwner->ChangeState(PlayerState::eID::Idle);
	}
}

void JustDodge::LateUpdate()
{
    Dodge::LateUpdate();
}

void JustDodge::Draw()
{
    Dodge::Draw();
}

void JustDodge::Exit()
{
	Dodge::Exit();
}

} // PlayerState.