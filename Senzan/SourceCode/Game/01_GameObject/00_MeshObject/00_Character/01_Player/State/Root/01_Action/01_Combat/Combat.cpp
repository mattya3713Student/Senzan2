#include "Combat.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
Combat::Combat(Player* owner)
	: Action		(owner)
	, m_MaxTime		()
	, m_currentTime	()
{
}

Combat::~Combat()
{
}

void Combat::Enter()
{
	Action::Enter();
	m_currentTime = 0.f;

}

void Combat::Update()
{
	Action::Update();
}

void Combat::LateUpdate()
{
	Action::LateUpdate();
}

void Combat::Draw()
{
	Action::Draw();
}

void Combat::Exit()
{
	Action::Exit();
}

} // PlayerState.

