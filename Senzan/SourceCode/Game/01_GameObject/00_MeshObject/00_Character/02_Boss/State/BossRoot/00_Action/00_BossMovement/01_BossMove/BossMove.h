#pragma once

#include "..//BossMovement.h"

class Boss;

/**************************************************
*	ボスの動作関数.
*	ToDo : 動作関数では左右移動と攻撃の処理の作成をします.
*	ToDo : 攻撃の判定はランダムにして確率を操作して実装していきます.
**/

namespace BossState
{
	class BossMove final
		: public BossMovement
	{
	public:
		BossMove(Boss* pOwner);
		~BossMove();

		constexpr BossState::enID GetStateID() const override;

	private:

	};
}