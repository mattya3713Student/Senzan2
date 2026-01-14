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
	, m_List(enList::none)
{
}

BossSlashState::~BossSlashState()
{
}

void BossSlashState::Enter()
{
	m_StateTimer = 0.0f;
	m_currentTimer = 0.0f;
	m_Attacktime = 0.0f;

	for (auto& w : m_ColliderWindows) { w.Reset(); }
	for (auto& w : m_MovementWindows) { w.Reset(); }

	m_ColliderWindows.clear();
	m_MovementWindows.clear();

	// 既定値（ImGuiで調整可能）
	m_AnimSpeed = 3.5f;
	m_ExitTime = 1.0f;
	m_HomingEndTime = 0.2f;

	// 斬撃当たり判定: 右手ボーン
	ColliderWindow slashWindow;
	slashWindow.BoneName = "boss_Hand_R";
	slashWindow.Start = 0.15f;
	slashWindow.Duration = 0.25f;
	m_ColliderWindows.push_back(slashWindow);

	// 踏み込み
	MovementWindow stepIn;
	stepIn.Start = 0.05f;
	stepIn.Duration = 0.25f;
	stepIn.Speed = 60.0f;
	m_MovementWindows.push_back(stepIn);

	// 当たり判定は window 側で ON/OFF するので通常攻撃判定はOFF（念のため）
	m_pOwner->SetAttackColliderActive(false);

	// 初期位置を保存.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMStoreFloat3(&m_StartPos, DirectX::XMLoadFloat3(&BossPosF));

	// 斬るアニメーションの再生.
	m_pOwner->ChangeAnim(Boss::enBossAnim::Slash);

	m_List = enList::SlashAttack;
}

void BossSlashState::Update()
{
	const float dt = Time::GetInstance().GetDeltaTime();

	// ホーミング（一定時間まで）
	if (m_StateTimer <= m_HomingEndTime)
	{
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
	}

	UpdateBaseLogic(dt);

	switch (m_List)
	{
	case BossSlashState::enList::SlashAttack:
		if (m_StateTimer >= m_ExitTime)
		{
			m_pOwner->ChangeAnim(Boss::enBossAnim::SlashToIdol);
			m_List = enList::SlashIdol;
		}
		break;

	case BossSlashState::enList::SlashIdol:
		// Idol遷移も時間で統一（余韻用に固定）
		if (m_StateTimer >= (m_ExitTime + 0.4f))
		{
			m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		}
		break;

	default:
		break;
	}

#if _DEBUG
	DrawImGui();
#endif
}

void BossSlashState::LateUpdate()
{
}

void BossSlashState::Draw()
{
}

void BossSlashState::Exit()
{
	// window 制御のコライダーを確実にOFF
	m_pOwner->SetColliderActiveByName("boss_Hand_R", false);
	m_pOwner->SetAttackColliderActive(false);
}

void BossSlashState::BossAttack()
{
}
