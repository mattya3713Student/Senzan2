#include "BossThrowingState.h"

#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "..//04_Time/Time.h"

#include "00_MeshObject/03_SnowBall/SnowBall.h"

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
	//ボスの向きを設定.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	//X,Z平面の方向.
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	//Y軸回転角度を計算し、ボスをプレイヤーに向かせる.
	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);
	float angle_radian = std::atan2f(-dx, -dz);
	m_pOwner->SetRotationY(angle_radian);

	//攻撃開始位置.
	DirectX::XMFLOAT3 m_StartPos;

	//初期位置を保存.
	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);


	m_pOwner->SetAnimSpeed(0.06);
	m_pOwner->ChangeAnim(Boss::enBossAnim::LaserCharge);
}

void BossThrowingState::Update()
{
	//deltaTimeの取得.
	float deltaTime = Time::GetInstance().GetDeltaTime();

	switch (m_List)
	{
	case BossThrowingState::enThrowing::None:
		//すぐにアニメションを再生する.
		m_List = enThrowing::Anim;
		break;
	case BossThrowingState::enThrowing::Anim:
		//アニメーションをここには書かない.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserCharge))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::Laser);
			m_List = enThrowing::Attack;
		}
		break;
	case BossThrowingState::enThrowing::Attack:
		//アニメーションをしている時に攻撃を発動させる.
		//ここに二次ベジェ曲線を使用して投擲を起こす.
		//falseのときに入る.
		if (!m_IsLaunched)
		{
			m_pBall->Fire(m_pOwner->GetTargetPos(), m_pOwner->GetPosition());

			// 発射フラグをONにし、二度と初期化されないようにする
			m_IsLaunched = true;
		}

		//投擲用の雪玉を表示する.
		m_pBall->Update();

		//雪玉が、前のPlayerの位置についたときに入る.
		if (!m_pBall->IsAction)
		{
			m_IsLaunched = false; // 次の攻撃のためにリセット
			if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Laser))
			{
				m_pOwner->ChangeAnim(Boss::enBossAnim::LaserEnd);
				m_List = enThrowing::CoolDown;
			}
		}
		break;
	case BossThrowingState::enThrowing::CoolDown:
		//攻撃のクールタイムが終了した際にTransに移動する.

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::LaserEnd))
		{
			m_pBall->ResetPosition();
			m_List = enThrowing::Trans;
		}
		break;
	case BossThrowingState::enThrowing::Trans:
		//Idolへの遷移用.
		//今デバッグのためにEnterで再度攻撃をするようにしている
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_List = enThrowing::None;
		}
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;
	default:
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
}

void BossThrowingState::ParryTime()
{
	switch (m_Parry)
	{
	case BossThrowingState::enParry::none:
		//ひるんだ時のアニメーションの再生へ入る.
		m_Parry = enParry::Flinch;
		break;
	case BossThrowingState::enParry::Flinch:
		//アニメーションの再生.
		m_pOwner->SetAnimSpeed(0.03);
		m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
		//アニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
		{
			m_pOwner->SetAnimSpeed(0.01);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
			//怯み中のコードに入る.
			m_Parry = enParry::FlinchTimer;
		}
		break;
	case BossThrowingState::enParry::FlinchTimer:
		//怯み中のアニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
		{
			m_pOwner->SetAnimSpeed(0.03);
			m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
			//待機にもどるアニメーションの再生.
			m_Parry = enParry::FlinchToIdol;
		}
		break;
	case BossThrowingState::enParry::FlinchToIdol:
		//待機へ戻るアニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchToIdol))
		{
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		}
		break;
	default:
		break;
	}
}

void BossThrowingState::BossAttack()
{
	m_pBall->Update();
}
