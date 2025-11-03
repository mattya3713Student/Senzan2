//#pragma once
//
//#include "System/Utility/StateMachine/StateBase.h"
//
////前方宣言するところ.
//class Boss;
//
///***********************************************************************
//*	ボス行動用のアイドルステートクラス.
//*	アイドルクラスを作成しなおすのでここに前のコードを保存しておく.
//**/
//
//class BossIdleState final
//	: public StateBase<Boss>
//{
//public:
//	BossIdleState();
//	~BossIdleState();
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
//
//};