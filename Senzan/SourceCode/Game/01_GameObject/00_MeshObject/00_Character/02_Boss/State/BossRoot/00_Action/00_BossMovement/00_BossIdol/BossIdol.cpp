#include "BossIdol.h"

namespace BossState
{
	BossIdol::BossIdol(Boss* pOwner)
		: BossMovement (pOwner)
		, m_pMove(nullptr)
	{
	}

	BossIdol::~BossIdol()
	{
	}

	constexpr BossState::enID BossIdol::GetStateID() const
	{
		return BossState::enID::Idol;
	}

	void BossIdol::Enter()
	{
		m_pOwner->SetIsLoop(true);
		m_pOwner->SetAnimSpeed(50.0);
		//待機アニメションを再生.
		m_pOwner->ChangeAnim(Boss::enBossAnim::Idol);

	}

	void BossIdol::Update()
	{
		RotateToPlayer();

		const DirectX::XMFLOAT3& BossPos_XF = m_pOwner->GetPosition();
		const DirectX::XMFLOAT3& PlayerPos_XF = m_pOwner->GetTargetPos();

		DirectX::XMVECTOR BossPos_Vec = DirectX::XMLoadFloat3(&BossPos_XF);
		DirectX::XMVECTOR PlayerPos_Vec = DirectX::XMLoadFloat3(&PlayerPos_XF);

		DirectX::XMVECTOR Diff_Vec = DirectX::XMVectorSubtract(PlayerPos_Vec, BossPos_Vec);

		//距離の計算(2乗).
		DirectX::XMVECTOR DistSq_Vec = DirectX::XMVector3Length(Diff_Vec);
		float currentDistanceSq = DirectX::XMVectorGetX(DistSq_Vec);

		//判定.
		//ToDo : ここに距離でMoveに入るようにする.
		constexpr float MOVE_THRESHOLD = 30.0f;
		const float MOVE_THRESHOLD_SQ = MOVE_THRESHOLD * MOVE_THRESHOLD;

		// プレイヤーが一定距離より遠い場合に移動ステートへ
		constexpr float MOVE_START_RANGE = 25.0f;
		if (currentDistanceSq > MOVE_START_RANGE)
		{
			// BossRootを取得してステートを切り替える
			m_pOwner->ChangeState(BossState::enID::Move);
		}
	}

	void BossIdol::LateUpdate()
	{
	}

	void BossIdol::Draw()
	{
	}

	void BossIdol::Exit()
	{
	}
} //BossState.