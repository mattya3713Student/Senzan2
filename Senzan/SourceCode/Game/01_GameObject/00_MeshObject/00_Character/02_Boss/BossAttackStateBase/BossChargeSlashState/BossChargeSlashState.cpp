#include "BossChargeSlashState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先

//ボーンの位置を取得するために使用するImGuiです.
#include "System/Singleton/ImGui/CImGuiManager.h"

// Transform.h のインクルードが不足している場合は追加してください
#include <DirectXMath.h>
#include <memory> 
#include <cstdio> 

BossChargeSlashState::BossChargeSlashState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_List(enChargeSlashAnim::none)
{
	Enter();
}

BossChargeSlashState::~BossChargeSlashState()
{
}

void BossChargeSlashState::Enter()
{
	m_currentTimer = 0.0f;

	// 当たり判定を有効化.
	m_pOwner->SetAttackColliderActive(true);

	//ボスの向きを設定.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	//XMVectorSubtract: 引数の左から右を引く関数.
	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	//X,Z方向の平面の方向ベクトル.
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	//Y軸回転角度を計算し、ボスをプレイヤーに向かせる.
	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);
	float angle_radian = std::atan2f(-dx, -dz);
	m_pOwner->SetRotationY(angle_radian);

	//初期位置を保存.
	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);

	//アニメーション速度.
	m_pOwner->SetAnimSpeed(m_pOwner->m_AnimSpeed);

	//ため斬りアニメーションの再生.
	m_pOwner->ChangeAnim(Boss::enBossAnim::ChargeAttack);
}

void BossChargeSlashState::Update()
{
	switch (m_List)
	{
	case BossChargeSlashState::enChargeSlashAnim::none:
		m_List = enChargeSlashAnim::Charge;
		break;
	case BossChargeSlashState::enChargeSlashAnim::Charge:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::ChargeAttack))
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::ChargeToIdol);
			m_List = enChargeSlashAnim::ChargeSlash;
		}
		break;
	case BossChargeSlashState::enChargeSlashAnim::ChargeSlash:
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::ChargeToIdol))
		{
			m_List = enChargeSlashAnim::ChargeSlashToIdol;
		}
		break;
	case BossChargeSlashState::enChargeSlashAnim::ChargeSlashToIdol:
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;
	default:
		break;
	}
}

void BossChargeSlashState::LateUpdate()
{
}

void BossChargeSlashState::Draw()
{
}

void BossChargeSlashState::Exit()
{
	// 当たり判定を無効化.
	m_pOwner->SetAttackColliderActive(false);
}

void BossChargeSlashState::BoneDraw()
{
}

void BossChargeSlashState::BossAttack()
{
}

