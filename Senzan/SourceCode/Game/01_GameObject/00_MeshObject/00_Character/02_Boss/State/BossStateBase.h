#pragma once

#include "System//Utility//StateMachine//StateBase.h"
#include "BossStateID.h"

//ToDo : 前方宣言.
class Boss;

/**************************************************
*	ボスの状態の基底クラス.
*	ボスクラスで使う関数の作成.
**/

class BossStateBase
	: public StateBase<Boss>
{
public:
	BossStateBase(Boss* pOwner);

	//ToDo : ボスの状態一覧.
	//		 純粋仮想関数で作成していく、これからしようしていく子クラスに必要なもの.
	//		 継承もさせる.
	//		 constにしていることにより変更不可能にしている.
	//ボスの状態を取得する.
	//constexpr virtual BossState::enID GetStateID() const = 0;

	//ボスの情報取得.
	Boss* GetBoss() const;

private:

protected:

	//プレイヤーの方に常に向くようにする.
	void RotateToPlayer(float RotationSpeed = 200.0f);

	//目的の角度に向かって指定した速度で回転させる.
	//ToDo : 第一引数にはターゲット(プレイヤー)の方に向く.
	//		 第二引数には回転の速度の設定.
	//ToDo : 継承関係をしっかりできるようになったときに戻す.
	void RotetoToTarget(
		float TargetRote,
		float RotetionSpeed);


};