#include "BossAttack.h"

#include "..//..//..//..//Boss.h"
#include "System/Utility/SingleTrigger/SingleTrigger.h"


namespace BossState
{
	BossAttack::BossAttack(Boss* pOwner)
		: BossAction	(pOwner)
	{
	}

	BossAttack::~BossAttack()
	{
	}

	void BossAttack::Enter()
	{
		BossAction::Enter();
	}

	void BossAttack::Update()
	{
		BossAction::Update();
	}

	void BossAttack::LateUpdate()
	{
		BossAction::LateUpdate();
	}

	void BossAttack::Draw()
	{
		BossAction::Draw();
	}

	void BossAttack::Exit()
	{
		BossAction::Exit();
	}
} //BoaaState.