#pragma once
#include "..//BossAction.h"

class Boss;
class CapsuleCollider;

/***********************************************
*	ボスの移動の共通ステート.
**/

namespace BossState
{
	class BossMovement
		: public BossAction
	{
	public:
		BossMovement(Boss* pOwner);
		~BossMovement();

		void Enter() override;
		void Update() override;
		void LateUpdate() override;
		void Draw() override;
		void Exit() override;

	private:
		std::shared_ptr<CapsuleCollider> m_Collision_Capsul;
	};
}