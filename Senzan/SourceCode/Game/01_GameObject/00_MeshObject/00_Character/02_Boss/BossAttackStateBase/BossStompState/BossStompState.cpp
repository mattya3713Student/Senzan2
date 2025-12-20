#include "BossStompState.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game/04_Time/Time.h"

BossStompState::BossStompState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_pIdol()
	, m_List(enAttack::None)
	, m_Velocity(0.0f, 0.0f, 0.0f)
	, m_JumpPower(5.0f)     // 【落下を開始する高さ】
	, m_Gravity(1.5f)       // 落下速度（重力）
	, m_JumpFrag(false)
	, m_GroundedFrag(true)
	, m_Timer(0.0f)
	, TransitionTimer(120.0f)
	, m_UpSpeed(1.0f)
{
}

BossStompState::~BossStompState()
{
}

void BossStompState::Enter()
{
	// 最初は動かさない（Y=0のまま）
	m_Velocity = { 0.0f, 0.0f, 0.0f };
	m_GroundedFrag = true;

	// 向きだけプレイヤーに合わせる
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);
	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);
	float angle_radian = std::atan2f(-dx, -dz);
	m_pOwner->SetRotationY(angle_radian);

	// 溜めアニメーション開始
	m_pOwner->SetAnimSpeed(14.0); // 速度は元の14.0に戻しました
	m_pOwner->ChangeAnim(Boss::enBossAnim::Special_0);
}

void BossStompState::Update()
{
	switch (m_List)
	{
	case BossStompState::enAttack::None:
		// 【修正】Special_0の間は何もしない（移動計算を呼ばない）

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0))
		{
			// Special_1 に切り替わる瞬間に、ボスの位置を空中にセット
			m_pOwner->SetPositionY(m_JumpPower);
			m_Velocity.y = 0.0f; // 落下開始時の速度をリセット

			m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
			m_List = enAttack::Stomp;
		}
		break;

	case BossStompState::enAttack::Stomp:
		// 【落下中のみ】物理計算を回して下に下げる
		BossAttack();

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_1))
		{
			m_pOwner->SetPositionY(0.0f);
			m_Velocity.y = 0.0f;
			m_GroundedFrag = true;

			m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
			m_List = enAttack::CoolTime;
		}
		break;

	case BossStompState::enAttack::CoolTime:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SpecialToIdol))
		{
			m_List = enAttack::Trans;
		}
		break;

	case BossStompState::enAttack::Trans:
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;
	}
}

void BossStompState::LateUpdate()
{
}

void BossStompState::Draw()
{
}

void BossStompState::Exit()
{
	m_GroundedFrag = true;
	m_pOwner->SetPositionY(0.0f);
}

void BossStompState::BossAttack()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	// 重力のみを計算
	m_Velocity.y -= m_Gravity * deltaTime;

	float nextY = m_pOwner->GetPositionY() + (m_Velocity.y * m_UpSpeed);

	// 地面判定
	if (nextY <= 0.0f)
	{
		nextY = 0.0f;
		m_Velocity.y = 0.0f;
	}

	m_pOwner->SetPositionY(nextY);
}