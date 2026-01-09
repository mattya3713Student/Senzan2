#include "BossThrowingState.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "..//04_Time/Time.h"
#include "00_MeshObject/00_Character/03_SnowBall/SnowBall.h"

BossThrowingState::BossThrowingState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_Timer(0.0f)
	, m_TransitionTimer(60.0f)
	, m_List(enThrowing::None)
	, m_Parry(enParry::none)
	, m_pBall(std::make_unique<SnowBall>())
{
}

BossThrowingState::~BossThrowingState()
{
}

void BossThrowingState::Enter()
{
	// プレイヤーの方を向く
	DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;

	float dx = PlayerPosF.x - BossPosF.x;
	float dz = PlayerPosF.z - BossPosF.z;
	float angle_radian = std::atan2f(dx, dz) + DirectX::XM_PI;
	m_pOwner->SetRotationY(angle_radian);

	// アニメーション開始
	m_pOwner->SetAnimSpeed(2.0);
	m_pOwner->ChangeAnim(Boss::enBossAnim::LaserCharge); // 投擲用の溜め

	m_IsLaunched = false;
	m_List = enThrowing::None;
}

void BossThrowingState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	// パリィ（怯み）状態の場合はUpdateをスキップしてParryTimeへ
	if (m_Parry != enParry::none)
	{
		ParryTime();
		return;
	}

	switch (m_List)
	{
	case enThrowing::None:
		// 当たり判定を無効化.
		m_pOwner->SetAttackColliderActive(true);
		m_List = enThrowing::Anim;
		break;

	case enThrowing::Anim:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserCharge))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::Laser); // 投げる瞬間のアニメ
			m_List = enThrowing::Attack;
		}
		break;

	case enThrowing::Attack:
		if (!m_IsLaunched)
		{
			// 発射位置を調整（ボスの手元の高さ：約5.0f）
			DirectX::XMFLOAT3 startPos = m_pOwner->GetPosition();
			startPos.y += 5.0f;

			// 雪玉発射
			m_pBall->Fire(m_pOwner->GetTargetPos(), startPos);
			m_IsLaunched = true;
		}

		// 雪玉の移動更新
		m_pBall->Update();

		// 雪玉が着弾、かつ投げアニメーションが終了したら次へ
		if (!m_pBall->IsAction)
		{
			if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Laser))
			{
				m_pOwner->ChangeAnim(Boss::enBossAnim::LaserEnd);
				m_List = enThrowing::CoolDown;
			}
		}
		break;

	case enThrowing::CoolDown:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserEnd))
		{
			m_pBall->ResetPosition();
			m_List = enThrowing::Trans;
		}
		break;

	case enThrowing::Trans:
		// 次の行動（アイドル）へ
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;
	}
}

void BossThrowingState::LateUpdate()
{
}

void BossThrowingState::Draw()
{
	m_pBall->Draw();
}

void BossThrowingState::Exit()
{
	// 当たり判定を無効化.
	m_pOwner->SetAttackColliderActive(false);
}

void BossThrowingState::ParryTime()
{
	// 省略なしで記述（パリィロジック）
	switch (m_Parry)
	{
	case enParry::Flinch:
		m_pOwner->SetAnimSpeed(5.0);
		m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
		{
			m_pOwner->SetAnimSpeed(5.0);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
			m_Parry = enParry::FlinchTimer;
		}
		break;
	case enParry::FlinchTimer:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
		{
			m_pOwner->SetAnimSpeed(3.0);
			m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
			m_Parry = enParry::FlinchToIdol;
		}
		break;
	case enParry::FlinchToIdol:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchToIdol))
		{
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		}
		break;
	}
}

void BossThrowingState::BossAttack()
{
	m_pBall->Update();
}
