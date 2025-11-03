#include "PlayerStateBase.h"
#include "../Player.h"    
#include "Root/Root.h"    

PlayerStateBase::PlayerStateBase(Player* owner)
	: StateBase <Player>(owner)
{
}

Player* PlayerStateBase::GetPlayer() const
{
    return m_pOwner;
}


//PlayerState::Root* PlayerStateBase::GetRoot() const
//{
//    return GetPlayer()->GetRootStateMachine();
//}
