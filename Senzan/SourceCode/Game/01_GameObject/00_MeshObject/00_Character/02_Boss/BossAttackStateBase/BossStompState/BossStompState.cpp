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
	// 当たり判定を有効化.
	m_pOwner->SetAttackColliderActive(true);
	// 踏みつけ用コライダーの取得
	auto* pStompCollider = m_pOwner->GetStompCollider();
	if (pStompCollider) {
		pStompCollider->SetActive(true);             // 表示・判定をON
		pStompCollider->SetColor(Color::eColor::Magenta); // 目立つ色（マゼンタ等）に変更
		pStompCollider->SetRadius(50.0f);            // 溜め中は少し小さめ
		pStompCollider->SetHeight(15.0f);
	}


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
	auto* pStompCollider = m_pOwner->GetStompCollider();

	switch (m_List)
	{
	case BossStompState::enAttack::None:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0))
		{
			m_pOwner->SetPositionY(m_JumpPower);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
			m_List = enAttack::Stomp;

			// 落下開始時に攻撃判定を大きくする
			if (pStompCollider) {
				pStompCollider->SetRadius(30.0f);
				pStompCollider->SetAttackAmount(15.0f); // ダメージ設定
			}
		}
		break;

	case BossStompState::enAttack::Stomp:
		BossAttack();

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_1))
		{
			m_pOwner->SetPositionY(0.0f);
			m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
			m_List = enAttack::CoolTime;

			// 着地したので判定を消す、または色を変える
			if (pStompCollider) {
				pStompCollider->SetActive(false);
			}
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

		// 当たり判定を無効化.
		m_pOwner->SetAttackColliderActive(false);
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