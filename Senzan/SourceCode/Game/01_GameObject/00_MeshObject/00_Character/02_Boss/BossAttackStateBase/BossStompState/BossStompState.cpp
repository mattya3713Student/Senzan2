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
	m_pOwner->ChangeAnim(Boss::enBossAnim::Special_0);
}

void BossStompState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	switch (m_List)
	{
	case BossStompState::enAttack::None:

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_0))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::Special_1);
			//次の遷移への移動.
			m_List = enAttack::Stomp;
		}
		break;
	case BossStompState::enAttack::Stomp:
		//ここで踏みつけ攻撃の動作を実装.
		BossAttack();

		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Special_1))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::SpecialToIdol);
			m_List = enAttack::CoolTime;
		}

		if (m_pOwner->GetPositionY() < 0)
		{
			m_Velocity.y = 0.0f;
			m_pOwner->SetPositionY(0.f);
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
