#include "Character.h"
#include "Game/03_Collision/Box/BoxCollider.h"
#include "Game/03_Collision/Capsule/CapsuleCollider.h"
#include "Game/03_Collision/Sphere/SphereCollider.h"

Character::Character()
	: MeshObject()
	, m_pPressCollider(std::make_shared<CapsuleCollider>(m_Transform))
{
	m_pPressCollider->SetColor(Color::eColor::Cyan);
	m_pPressCollider->SetPositionOffset(0.f, 50.f, 0.f);
	m_pPressCollider->SetRadius(50.f);
	m_pPressCollider->SetHeight(50.f);
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
	MeshObject::Draw();
	m_pPressCollider->SetDebugInfo();
}

//------------------------------------------------------------------------------------.

void Character::UpdateAfterCollision()
{
	// “–‚½‚è”»’è‚ªÚ‘±‚³‚ê‚Ä‚¢‚È‚©‚Á‚½‚çˆ—‚µ‚È‚¢.
	if (m_pPressCollider == nullptr)
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
	m_pPressCollider.reset();
}

const std::shared_ptr<ColliderBase>& Character::GetCollider() const
{
	return m_pPressCollider;
}
