#include "BossSlashState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先
#include "Resource\\Mesh\\02_Skin\\SkinMesh.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

constexpr float MY_PI = 3.1415926535f;


constexpr static float AnimSlashTime		= 3000.0f;
constexpr static float AnimSlashToIdolTime	= 30.0f;


BossSlashState::BossSlashState(Boss* owner)
	: BossAttackStateBase(owner)

	, m_StartPos {}

	, m_pIdol()

	, m_pTransform(std::make_shared<Transform>())

	, m_List	(enList::none)

	, AnimChange(false)
{
	//m_pColl->SetHeight(50.0f);
	//m_pColl->SetRadius(5.0f);
}

BossSlashState::~BossSlashState()
{
}

void BossSlashState::Enter()
{
	// set bone to track for slash
	m_pOwner->SetSlashBoneName("boss_Hand_R");

	// 当たり判定を有効化: enable slash-related colliders
	m_pOwner->SetAttackCollidersActive(Boss::AttackType::Normal, true);

	// configure first slash collider if present
	auto* attackColl = m_pOwner->GetSlashCollider();
	if (attackColl) {
		attackColl->SetRadius(15.0f);      
		attackColl->SetHeight(40.0f);
		attackColl->SetPositionOffset(0.0f, 10.0f, -30.0f);
	}

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
	float angle_radian = std::atan2f(-dx, -dz);
	m_pOwner->SetRotationY(angle_radian);

	//初期位置を保存.
	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);

	//アニメーションの速度.
	m_pOwner->SetAnimSpeed(30.0);
	//斬るアニメーションの再生.
	m_pOwner->ChangeAnim(Boss::enBossAnim::Slash);

	AnimChange = false;
}

void BossSlashState::Update()
{
	// Bossクラスから、あらかじめ設定しておいた斬撃用判定を取得
	auto* pSlashCollider = m_pOwner->GetSlashCollider();

	switch (m_List)
	{
	case BossSlashState::enList::none:
		if (pSlashCollider) 
		{
			pSlashCollider->SetActive(true);
		}

		m_List = enList::SlashAttack;
		break;

	case BossSlashState::enList::SlashAttack:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Slash))
		{
			if (pSlashCollider)
			{
				pSlashCollider->SetActive(false);
			}

			m_pOwner->ChangeAnim(Boss::enBossAnim::SlashToIdol);
			m_List = enList::SlashIdol;
		}
		break;

	case BossSlashState::enList::SlashIdol:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::SlashToIdol))
		{
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		}
		break;

	default:
		break;
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
	// 当たり判定を無効化: disable slash-related colliders
	m_pOwner->SetAttackCollidersActive(Boss::AttackType::Normal, false);

	// clear bone tracking
	m_pOwner->SetSlashBoneName("");
}

void BossSlashState::BossAttack()
{
}

