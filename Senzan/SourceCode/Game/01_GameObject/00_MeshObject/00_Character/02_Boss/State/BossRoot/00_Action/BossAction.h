#pragma once
#include "..//..//BossStateBase.h"

#include "..//..//..//Boss.h"
#include "System/Utility/SingleTrigger/SingleTrigger.h"

/******************************************************
*	ボスの操作によるステート.
**/

namespace BossState
{
	class BossAction
		: public BossStateBase
	{
	public:
		BossAction(Boss* pOwner);
		~BossAction() {}

		virtual void Enter() override {}
		virtual void Update() override {}
		virtual void LateUpdate() override {}
		virtual void Draw() override {}
		virtual void Exit() override {}

	private:
		std::unique_ptr<SingleTrigger> m_AnimSpeedChangedTrigger;
	};
}