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

	, m_Parry(enParry::none)

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

	// --- 追加：当たり判定のパラメータ設定 ---
	m_pOwner->SetAttackColliderActive(true);
	auto* attackColl = m_pOwner->m_pAttackCollider;
	if (attackColl) {
		// チャージ斬りなので、通常の斬撃より少し大きく設定する例
		attackColl->SetRadius(20.0f);
		attackColl->SetHeight(45.0f);
		attackColl->SetPositionOffset(0.0f, 10.0f, -35.0f);
	}
	// ---------------------------------------

	// ボスの向きを設定
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

	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);

	m_pOwner->SetAnimSpeed(25.0);
	m_pOwner->ChangeAnim(Boss::enBossAnim::ChargeAttack);
}

void BossChargeSlashState::Update()
{
	// Bossクラスから斬撃用判定を取得
	auto* pSlashCollider = m_pOwner->GetSlashCollider();

	switch (m_List)
	{
	case BossChargeSlashState::enChargeSlashAnim::none:
		// --- 追加：攻撃開始時に判定を有効化 ---
		if (pSlashCollider)
		{
			pSlashCollider->SetActive(true);
		}
		m_List = enChargeSlashAnim::Charge;
		break;

	case BossChargeSlashState::enChargeSlashAnim::Charge:
		// チャージ攻撃アニメーションが終了したら
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::ChargeAttack))
		{
			// --- 追加：攻撃が終わったので判定を無効化 ---
			if (pSlashCollider)
			{
				pSlashCollider->SetActive(false);
			}

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

void BossChargeSlashState::ParryTime()
{
	switch (m_Parry)
	{
	case BossChargeSlashState::enParry::none:
		//ひるんだ時のアニメーションの再生へ入る.
		m_Parry = enParry::Flinch;
		break;
	case BossChargeSlashState::enParry::Flinch:
		//アニメーションの再生.
		m_pOwner->SetAnimSpeed(15.0);
		m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchParis);
		//アニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::FlinchParis))
		{
			m_pOwner->SetAnimSpeed(5.0);
			m_pOwner->ChangeAnim(Boss::enBossAnim::Flinch);
			//怯み中のコードに入る.
			m_Parry = enParry::FlinchTimer;
		}
		break;
	case BossChargeSlashState::enParry::FlinchTimer:
		//怯み中のアニメーションの再生が終了したら.
		if (m_pOwner->IsAnimEnd(Boss::enBossAnim::Flinch))
		{
			m_pOwner->SetAnimSpeed(15.0);
			m_pOwner->ChangeAnim(Boss::enBossAnim::FlinchToIdol);
			//待機にもどるアニメーションの再生.
			m_Parry = enParry::FlinchToIdol;
		}
		break;
	case BossChargeSlashState::enParry::FlinchToIdol:
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


void BossChargeSlashState::BossAttack()
{
}

