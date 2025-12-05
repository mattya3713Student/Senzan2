#include "AttackCombo_0.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"


// 攻撃開始までのフレーム数
static constexpr int	ATTACK_START_FREAM = 80;
static constexpr float	ATTACK_START_TIME = ATTACK_START_FREAM / 0.016;



namespace PlayerState {
AttackCombo_0::AttackCombo_0(Player* owner)
	: Combat(owner)
{
}
AttackCombo_0::~AttackCombo_0()
{
}

// IDの取得.
constexpr PlayerState::eID AttackCombo_0::GetStateID() const
{
	return PlayerState::eID::AttackCombo_0;
}

void AttackCombo_0::Enter()
{
	m_pOwner->SetIsLoop(false);
	m_pOwner->SetAnimSpeed(ATTACK_ANIM_SPEED_0);
	m_pOwner->ChangeAnim(Player::eAnim::Dodge);
}

void AttackCombo_0::Update()
{

	m_pOwner->ChangeState(PlayerState::eID::Idle);
}

void AttackCombo_0::LateUpdate()
{
}

void AttackCombo_0::Draw()
{
}
void AttackCombo_0::Exit()
{
}

} // PlayerState.