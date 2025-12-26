#pragma once

#include "..//BossStateBase.h"
#include "..//BossStateID.h"

class Boss;

namespace BossState
{
	/******************************************************
	*	ボスの状態の最上位.
	*	各ステートの所有権と使用ステート切り替えをする.
	**/
	class BossRoot final
		: public BossStateBase
	{
	public:
		BossRoot(Boss* pOwner);
		~BossRoot();

		//ステートのIDの取得.
		//constexpr BossState::enID GetStateID() const override;

		void Enter() override;
		void Update() override;
		void LateUpdate() override;
		void Draw() override;
		void Exit() override;

		//ステートの変更.
		void ChangeState(BossState::enID id);

#pragma region GetStateRef

#pragma	endregion
	private:
		//メンバ変数.
		//ToDo : 継承したメンバ変数を書く.

	};
}