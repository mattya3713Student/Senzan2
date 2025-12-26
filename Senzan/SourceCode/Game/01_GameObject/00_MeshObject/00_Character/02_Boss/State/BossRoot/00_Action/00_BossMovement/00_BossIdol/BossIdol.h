#pragma once
#include "..//BossMovement.h"

class Boss;

/***********************************************
*	ボスの待機クラス.
**/

namespace BossState
{
	class BossIdol final
		: public BossMovement
	{
	public:
		BossIdol(Boss* pOwner);
		~BossIdol();

		constexpr BossState::enID GetStateID() const override;

		void Enter() override;
		void Update() override;
		void LateUpdate() override;
		void Draw() override;
		void Exit() override;

	private:

	};
}