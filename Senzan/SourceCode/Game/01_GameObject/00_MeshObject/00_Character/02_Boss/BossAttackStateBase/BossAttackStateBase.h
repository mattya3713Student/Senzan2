#pragma once

#include "System//Utility//StateMachine//StateBase.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"

/*****************************************************************
*	ボスの攻撃ベースクラス(基底クラス).
**/

//前方宣言.
class Boss;
class Time;

class BossAttackStateBase
	: public StateBase<Boss>

{
public:
	BossAttackStateBase(Boss* owner);
	virtual ~BossAttackStateBase() = default;

	//最初に入る.
	virtual void Enter() override {};
	//動作.
	virtual void Update() override {};
	//かかなくていい.
	virtual void LateUpdate() override {};
	//描画.
	virtual void Draw() override {};
	//終わるときに一回だけ入る.
	virtual void Exit() override {};

	//攻撃を実行させる関数.
	virtual void BossAttack() {};

	// パリィー用関数.
	//virtual bool CanBeParried() const { return m_canBeParried; }

	virtual void ParryTime() {};

protected:
	//メンバ変数を作成.
	//攻撃開始からの経過時間取得
	float m_Attacktime;
	//ダメージを一度与えたかどうかのフラグ.
	//bool m_isDamageDealt; 
	//パリィー可能かどうか.
	//bool m_canBeParried;       
	// 

	//アニメーションに必要なメンバ変数を設定している.
	DirectX::XMFLOAT3			m_BonePos;	// ボーン座標.

	LPD3DXANIMATIONCONTROLLER   AnimCtrl;

	std::shared_ptr<Transform> m_pTransform;

	float m_currentTimer;
	static constexpr float m_currentAnimSpeed = 0.001;



};
