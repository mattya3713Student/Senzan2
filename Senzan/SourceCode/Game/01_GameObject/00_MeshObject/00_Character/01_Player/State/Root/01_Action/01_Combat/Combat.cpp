#include "Combat.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

namespace PlayerState {
Combat::Combat(Player* owner)
	: Action	(owner)
{
}

Combat::~Combat()
{
}
void Combat::Enter()
{
}
void Combat::Update()
{
}
void Combat::LateUpdate()
{
}
void Combat::Draw()
{
}
void Combat::Exit()
{
}

} // PlayerState.