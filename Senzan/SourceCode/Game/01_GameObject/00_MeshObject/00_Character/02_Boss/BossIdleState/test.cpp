//#include "test.h"
//
////前方宣言したクラスのインクルード.
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/Boss.h"
//#include "System/Utility/StateMachine/StateMachine.h"
//
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttackState/BossAttackState.h"
//
//#include "00_MeshObject/00_Character/EX_Boss/BossMoveState/BossMoveState.h"
//
//BossIdleState::BossIdleState()
//{
//}
//
//BossIdleState::~BossIdleState()
//{
//}
//
//void BossIdleState::Start()
//{
//}
//
//void BossIdleState::Update()
//{
//	//-------------------------------------------------------------------
//	// アイドルの動作では、プレイヤーとボスの位置にの計算をして、
//	// 常にプレイヤーに正対させておくためにここに作成します.
//	//-------------------------------------------------------------------
//	//ボスのポジションを入手している.
//	const D3DXVECTOR3& bossPos = m_pOwner->GetPosition();
//	//プレイヤーのポジションを入手している.
//	const D3DXVECTOR3& playerPos = m_pOwner->GetPlayer()->GetPosition();
//	//プレイヤーの位置 - ボスの位置の計算をする
//	D3DXVECTOR3 DistanceVec = playerPos - bossPos;
//	//ベクトルの長さを計算している.
//	float distance = D3DXVec3LengthSq(&DistanceVec);
//
//	//すぐにこの中に入ります.
//	m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>());
//}
//
//void BossIdleState::LateUpdate()
//{
//}
//
//void BossIdleState::Draw()
//{
//}
//
//void BossIdleState::Exit()
//{
//}
