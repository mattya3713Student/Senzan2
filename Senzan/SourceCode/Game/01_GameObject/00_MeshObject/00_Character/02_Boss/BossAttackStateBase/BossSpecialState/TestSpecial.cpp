#include "TestSpecial.h"

#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdleState/BossIdleState.h"

TestSpecial::TestSpecial(Boss* owner)
	: BossAttackStateBase	( owner )

	, m_List				( enSpecial::None )

	, m_Velocity			( 0.0f, 0.0f, 0.0f )
	, m_SpecialPower		( 10.0f )
	, m_Gravity				( 0.098 )
	, m_SpecialFrag			( false )
	, m_GroundedFrag		( true )

	, m_Timer				( 0.0f )
	, m_TransitionTimer		( 10.0f )
	, m_AttackTimer			( 10.0f )

	, m_UpSpeed				( 0.15f )
{
}

TestSpecial::~TestSpecial()
{
}

void TestSpecial::Enter()
{
	m_Timer = 0.0f;
	m_Velocity = {};
	m_Velocity.y = m_SpecialPower;
}

void TestSpecial::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	switch (m_List)
	{
	case TestSpecial::enSpecial::None:
		m_List = enSpecial::Charge;
		break;
	case TestSpecial::enSpecial::Charge:
		//ため時間.
		m_Timer += deltaTime;
		if (m_Timer > m_AttackTimer)
		{
			//遷移の時にタイマーを使用するので初期化する.
			m_Timer = 0.0f;
			m_List = enSpecial::Attack;
		}
		break;
	case TestSpecial::enSpecial::Attack:
		BossAttack();

		if (m_pOwner->GetPositionY() < 0)
		{
			m_Velocity.y = 0.0f;
			m_pOwner->SetPositionY(0.f);
			m_List = enSpecial::CoolTime;
		}
		break;
	case TestSpecial::enSpecial::CoolTime:
		//クールタイム.
		m_Timer += deltaTime;
		if (m_Timer > m_TransitionTimer)
		{
			m_List = enSpecial::Trans;
		}
		break;
	case TestSpecial::enSpecial::Trans:
		//Idolへの遷移.
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_List = enSpecial::None;
		}
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdleState>(m_pOwner));
		break;
	default:
		break;
	}
}

void TestSpecial::LateUpdate()
{
}

void TestSpecial::Draw()
{
	//Sleep(20);
}

void TestSpecial::Exit()
{
	m_GroundedFrag = true;
	m_SpecialFrag = false;
}

void TestSpecial::BossAttack()
{
	//ボスのポジションを取得する.
	DirectX::XMFLOAT3 BossPos = m_pOwner->GetPosition();
	//プレイヤーのポジションを取得する.
	DirectX::XMFLOAT3 PlayerPos = m_pOwner->GetTargetPos();

	if (!m_SpecialFrag)
	{
		m_SpecialFrag = true;
		//ボスがプレイヤーの方向に向いて突進してくる処理.
		//Playerの位置 - Bossの位置.
		DirectX::XMFLOAT3 Dir
		{
			PlayerPos.x - BossPos.x,
			PlayerPos.y - BossPos.y,
			PlayerPos.z - BossPos.z,
		};

		//DirをXMFLOATからVECTORに変更.
		DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&Dir);
		//XMVECTORのDirを正規化する.
		Direction = DirectX::XMVector3Normalize(Direction);
		//XMFLOAT3に変更する.
		DirectX::XMStoreFloat3(&Dir, Direction);

		m_Velocity.y = 0.0f;


		m_Velocity.x = Dir.x * m_SpecialPower;
		m_Velocity.z = Dir.z * m_SpecialPower;
		m_Velocity.y = m_UpSpeed;

		//重力.
		m_Velocity.y += -m_Gravity;

		//位置更新.
		BossPos.x += m_Velocity.x;
		BossPos.y += m_Velocity.y;
		BossPos.z += m_Velocity.z;

		m_pOwner->SetPosition(BossPos);
	}
}
