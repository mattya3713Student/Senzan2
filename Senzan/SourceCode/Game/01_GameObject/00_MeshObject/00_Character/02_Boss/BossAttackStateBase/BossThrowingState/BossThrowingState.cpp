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

	, m_pBall(std::make_unique<SnowBall>())
{
}

BossThrowingState::~BossThrowingState()
{
}

void BossThrowingState::Enter()
{
	//
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
		m_List = enThrowing::Attack;
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
			m_List = enThrowing::CoolDown;
		}
		break;
		BossAttack();
		//何らかの条件でCoolDownに移動.
		m_List = enThrowing::CoolDown;
		break;
	case BossThrowingState::enThrowing::CoolDown:
		//攻撃のクールタイムが終了した際にTransに移動する.

		m_Timer += deltaTime;
		if (m_Timer >= m_TransitionTimer)
		{
			m_pBall->ResetPosition();
			m_Timer = 0.0f;
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

void BossThrowingState::BossAttack()
{
	m_pBall->Update();
}
