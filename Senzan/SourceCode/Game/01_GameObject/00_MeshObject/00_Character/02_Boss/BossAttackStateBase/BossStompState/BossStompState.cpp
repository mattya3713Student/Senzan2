#include "BossStompState.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"

#include "Game/04_Time/Time.h"

BossStompState::BossStompState(Boss* owner)
	: BossAttackStateBase(owner)

	, m_pIdol()

	, m_List(enAttack::None)

	, m_Velocity(0.0f, 0.0f, 0.0f)

	, m_JumpPower(0.5f)
	, m_Gravity(0.098f)
	, m_JumpFrag(false)
	, m_GroundedFrag(true)

	, m_Timer(0.0f)
	, TransitionTimer(120.0f)

	, m_UpSpeed(0.15f)
{
}

BossStompState::~BossStompState()
{
}

void BossStompState::Enter()
{
	m_Velocity = {};
	m_Velocity.y = m_JumpPower;

}

void BossStompState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	switch (m_List)
	{
	case BossStompState::enAttack::None:
		//次の遷移への移動.
		m_List = enAttack::Stomp;
		break;
	case BossStompState::enAttack::Stomp:
		//ここで踏みつけ攻撃の動作を実装.
		BossAttack();

		if (m_pOwner->GetPositionY() < 0)
		{
			m_Velocity.y = 0.0f;
			m_pOwner->SetPositionY(0.f);
			m_List = enAttack::CoolTime;
		}
		break;
	case BossStompState::enAttack::CoolTime:
		//クールタイム.
		//この時にdeltaTimeの設定をする.
		//deltaTimeの加算.
		m_Timer += deltaTime;
		if (m_Timer > TransitionTimer)
		{
			m_List = enAttack::Trans;
		}
		break;
	case BossStompState::enAttack::Trans:
		//Idolへの遷移.
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_List = enAttack::None;
		}
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;
	default:
		break;
	}


}

void BossStompState::LateUpdate()
{
}

void BossStompState::Draw()
{
	//Sleep(20);
}

void BossStompState::Exit()
{
	m_GroundedFrag = true;
	m_JumpFrag = false;
}

void BossStompState::BossAttack()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	//Y軸速度から重力を引く.
	m_Velocity.y -= m_Gravity * deltaTime;

#ifdef _DEBUG
	Log::GetInstance().Info("", m_Velocity);
#endif
	//ボスのY軸の位置を取得する.
	//この計算式にもdeltaTimeを入れている意味は、上に進速度の調整.
	//deltaTimeを消すと動きが速くなる.
	m_pOwner->SetPositionY(m_pOwner->GetPositionY() + m_Velocity.y * m_UpSpeed);


}
