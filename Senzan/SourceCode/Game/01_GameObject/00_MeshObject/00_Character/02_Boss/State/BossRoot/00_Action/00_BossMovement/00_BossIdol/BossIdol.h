#pragma once
#include "..//BossMovement.h"

#include "..//01_BossMove/BossMove.h"

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
		std::unique_ptr<BossMove> m_pMove;
	};
}