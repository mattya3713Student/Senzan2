#pragma once

#include "..//BossAction.h"

class Boss;
class SingleTrigger;

/*********************************************************
*	ボスの攻撃のステートクラス.
**/

namespace BossState
{
	class BossAttack
		: public BossAction
	{
	public:
		BossAttack(Boss* pOwner);
		~BossAttack();

		virtual void Enter() override;
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void Draw() override;
		virtual void Exit() override;

	protected:

	};
}