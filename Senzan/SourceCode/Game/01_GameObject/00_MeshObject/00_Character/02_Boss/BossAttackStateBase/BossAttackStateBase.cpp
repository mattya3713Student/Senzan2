#include "BossAttackStateBase.h"

BossAttackStateBase::BossAttackStateBase(Boss* owner)
	: StateBase<Boss>(owner)
	, m_Attacktime(0.0f)

	, m_pTransform(std::make_shared<Transform>())
{
}

