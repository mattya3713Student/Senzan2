//#pragma once
//
//#include "System/Utility/StateMachine/StateBase.h"
//
////前方宣言.
//class Boss;
//class Time;
//class ChargeSlashState;
//
///************************************************************
//*	ボス行動用の動きステートクラス.
//**/
//
//class BossMoveState final
//	: public StateBase<Boss>
//{
//public:
//	BossMoveState();
//	~BossMoveState();
//
//	//最初に入る.
//	void Start() override;
//	//動作.
//	void Update() override;
//	//かかなくていい.
//	void LateUpdate() override;
//	//描画.
//	void Draw() override;
//	//終わるときに一回だけ入る.
//	void Exit() override;
//
//private:
//	float		m_CurrentTime;		//現在の経過時間.
//	const float	m_CooltimeDuration;	//移動のクールタイム.
//
//	float m_MoveDirection;
//
//};