#include "BossMovement.h"

#include "..//..//..//..//Boss.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

namespace BossState
{
	BossMovement::BossMovement(Boss* pOwner)
		: BossAction(pOwner)
		, m_Collision_Capsul(std::make_shared<CapsuleCollider>())
	{
	}

	BossMovement::~BossMovement()
	{
	}

	void BossMovement::Enter()
	{
		BossAction::Enter();
	}

	void BossMovement::Update()
	{
		BossAction::Update();
	}

	void BossMovement::LateUpdate()
	{
		BossAction::LateUpdate();
		RotateToPlayer();
	}

	void BossMovement::Draw()
	{
		BossAction::Draw();
	}

	void BossMovement::Exit()
	{
		BossAction::Exit();
	}
} //BossState
