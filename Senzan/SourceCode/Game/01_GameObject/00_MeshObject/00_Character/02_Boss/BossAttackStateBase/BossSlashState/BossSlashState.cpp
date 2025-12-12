#include "BossSlashState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先
#include "Resource\\Mesh\\02_Skin\\SkinMesh.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

constexpr float MY_PI = 3.1415926535f;



BossSlashState::BossSlashState(Boss* owner)
	: BossAttackStateBase(owner)

	, m_pIdol()

	, m_pTransform(std::make_shared<Transform>())
{
	//m_pColl->SetHeight(50.0f);
	//m_pColl->SetRadius(5.0f);
}

BossSlashState::~BossSlashState()
{
}

void BossSlashState::Enter()
{
	m_currentTimer = 0.0f;
	m_Attacktime = 0.0f;

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
	float angle_radian = std::atan2f(dx, dz);
	m_pOwner->SetRotationY(angle_radian);

	//初期位置を保存.
	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);


	m_pOwner->SetAnimSpeed(m_currentAnimSpeed);

	//アニメーション再生の無限ループ用.
	//m_pOwner->SetIsLoop(true);
	//m_pOwner->ChangeAnim(5);

}

void BossSlashState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();

	m_currentTimer += m_currentAnimSpeed;

	if (m_currentTimer > m_pOwner->GetAnimPeriod(m_pOwner->m_AnimNo))
	{
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
	}
}

void BossSlashState::LateUpdate()
{
}

void BossSlashState::Draw()
{
}

void BossSlashState::Exit()
{
}

void BossSlashState::BossAttack()
{
}