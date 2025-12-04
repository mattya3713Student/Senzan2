#include "JustDodge.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"
#include "../Dodge.h"

#include "Game/03_Collision/00_Core/ColliderBase.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"
#include "System/Singleton/CollisionDetector/CollisionDetector.h"

namespace PlayerState {
JustDodge::JustDodge(Player* owner)
	: Dodge(owner)
{

    // 被ダメコライダーのポインタを保持.
    const auto& internal_colliders = m_pOwner->m_upColliders->GetInternalColliders();
    for (const std::unique_ptr<ColliderBase>& collider_ptr : internal_colliders)
    {
        if (collider_ptr && collider_ptr->GetMyMask() == eCollisionGroup::Player_Damage) // GetID()は仮の関数
        {
            m_pDamageDetectionCollider = collider_ptr.get();
            break;
        }
    }
}
JustDodge::~JustDodge()
{
}

// IDの取得.
constexpr PlayerState::eID JustDodge::GetStateID() const
{
	return PlayerState::eID::JustDodge;
}

void JustDodge::Enter()
{
    if (m_pDamageDetectionCollider != nullptr) 
	    CollisionDetector::GetInstance().UnregisterCollider(m_pDamageDetectionCollider);
}

void JustDodge::Update()
{
}

void JustDodge::LateUpdate()
{
}

void JustDodge::Draw()
{
}

void JustDodge::Exit()
{
	if (m_pDamageDetectionCollider != nullptr) 
	    CollisionDetector::GetInstance().RegisterCollider(*m_pDamageDetectionCollider);
}

} // PlayerState.