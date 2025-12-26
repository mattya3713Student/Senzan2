#include "BossIdol.h"

namespace BossState
{
	BossIdol::BossIdol(Boss* pOwner)
		: BossMovement (pOwner)
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
		
		//判定.
		//ToDo : ここに距離でMoveに入るようにする.
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