#include "BossMove.h"

#include "..//..//..//..//..//Boss.h"

namespace BossState
{
	BossMove::BossMove(Boss* pOwner)
		: BossMovement(pOwner)
	{
	}

	constexpr BossState::enID BossMove::GetStateID() const
	{
		return BossState::enID();
	}

}