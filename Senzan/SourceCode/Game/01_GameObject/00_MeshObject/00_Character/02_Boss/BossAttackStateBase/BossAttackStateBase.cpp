#include "BossAttackStateBase.h"
#include "../Boss.h"

BossAttackStateBase::BossAttackStateBase(Boss* owner)
	: StateBase<Boss>(owner)
	, m_Attacktime(0.0f)

	, m_spTransform(std::make_shared<Transform>())
{
}

void BossAttackStateBase::TrackSlashBone(const std::string& boneName)
{
	m_trackedSlashBoneName = boneName;
	if (m_pOwner) m_pOwner->SetSlashBoneName(boneName);
}

void BossAttackStateBase::UntrackSlashBone()
{
	m_trackedSlashBoneName.clear();
	if (m_pOwner) m_pOwner->SetSlashBoneName("");
}

void BossAttackStateBase::TrackStompBone(const std::string& boneName)
{
	m_trackedStompBoneName = boneName;
	if (m_pOwner) m_pOwner->SetStompBoneName(boneName);
}

void BossAttackStateBase::UntrackStompBone()
{
	m_trackedStompBoneName.clear();
	if (m_pOwner) m_pOwner->SetStompBoneName("");
}

void BossAttackStateBase::EnableAttackColliders(int attackTypeId, bool active)
{
	if (!m_pOwner) return;
	m_pOwner->SetAttackCollidersActive(static_cast<Boss::AttackType>(attackTypeId), active);
}

void BossAttackStateBase::EnableCurrentAttackColliders(bool active)
{
	if (!m_pOwner) return;
	m_pOwner->SetAttackColliderActive(active);
}

