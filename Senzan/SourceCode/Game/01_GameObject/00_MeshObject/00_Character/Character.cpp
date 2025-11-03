#include "Character.h"
#include "Game/03_Collision/Box/BoxCollider.h"
#include "Game/03_Collision/Capsule/CapsuleCollider.h"
#include "Game/03_Collision/Sphere/SphereCollider.h"

Character::Character()
	: MeshObject()
	, m_pCollider(std::make_shared<CapsuleCollider>(m_Transform))
{
}

Character::~Character()
{
	DetachCollider();
}

void Character::Update()
{
}

void Character::LateUpdate()
{
}

void Character::Draw()
{
}

//------------------------------------------------------------------------------------.

void Character::UpdateAfterCollision()
{
	// “–‚½‚è”»’è‚ªÚ‘±‚³‚ê‚Ä‚¢‚È‚©‚Á‚½‚çˆ—‚µ‚È‚¢.
	if (m_pCollider == nullptr)
	{
		return;
	}

	// Õ“Ë‚µ‚Ä‚¢‚½‚ç‰Ÿ‚µ–ß‚·.
	//if (m_pCollider->IsHit() == true)
	//{
	//	SetPosition(m_pCollider->GetPosition() - m_pCollider->GetOffsetPositoin());
	//}
}

//------------------------------------------------------------------------------------.

void Character::DetachCollider()
{
	m_pCollider.reset();
}

const std::shared_ptr<ColliderBase>& Character::GetCollider() const
{
	return m_pCollider;
}
