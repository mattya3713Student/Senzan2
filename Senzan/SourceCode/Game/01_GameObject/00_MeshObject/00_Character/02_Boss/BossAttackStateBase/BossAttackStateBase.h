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
	void Enter() override = 0;
	//動作.
	void Update() override = 0;
	//かかなくていい.
	void LateUpdate() override = 0;
	//描画.
	void Draw() override = 0;
	//終わるときに一回だけ入る.
	void Exit() override = 0;

	//攻撃を実行させる関数.
	virtual void BossAttack() = 0;

	// パリィー用関数.
	//virtual bool CanBeParried() const { return m_canBeParried; }

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
