//#include "test1.h"
//
//#include "00_MeshObject/00_Character/EX_Boss/Boss.h"
//#include "System/Utility/StateMachine/StateMachine.h"
//#include "Game/04_Time/Time.h"
//
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttackState/BossAttackState.h"
//
//#include "Game/01_GameObject/00_MeshObject/00_Character/EX_Boss/BossAttackState/ChargeSlashState/ChargeSlashState.h"
//
//BossMoveState::BossMoveState()
//	: m_CurrentTime(0.0f)
//	, m_CooltimeDuration(3.0f)	//左右移動の時間を指定する
//	, m_MoveDirection(1.0f)
//{
//}
//
//BossMoveState::~BossMoveState()
//{
//}
//
//void BossMoveState::Start()
//{
//}
//
////----------------------------------------------------------------------------------------
//// ここのUpdate関数は左右移動のみを実装する.
//// 
//// 動作の関係のことで追加
//// デルタタイム系統を使用するときは++(インクリメント)を使用せずに
//// *(掛け算)で管理させる.
////----------------------------------------------------------------------------------------
//
////void BossMoveState::Update()
////{
////	//ボスの移動のクールタイムのために
////	//deltaTimeを実装する.
////	float deltaTime = Time::GetInstance().GetDeltaTime();
////	m_CurrentTime += deltaTime;
////
////	//プレイヤーの位置.
////	const D3DXVECTOR3& playerPos = m_pOwner->GetPlayer()->GetPosition();
////	//ボスの位置を取得.
////	const D3DXVECTOR3& bossPos = m_pOwner->GetPosition();
////
////	//プレイヤーの位置 - ボスの位置の計算をする
////	D3DXVECTOR3 DistanceVec = playerPos - bossPos;
////	//ベクトルの長さを計算している.
////	float distanceSq = D3DXVec3LengthSq(&DistanceVec);
////
////	//ボスが常に左右移動をする.
////	//プレイヤーが攻撃範囲外にいてたら左右移動をする.
////
////	//ボスからプレイヤーへの方向ベクトル.
////	D3DXVECTOR3 directionToPlayer;
////	//正規化をしている.
////	D3DXVec3Normalize(&directionToPlayer, &DistanceVec);
////
////	//左右方向のベクトルを計算(前方向ベクトルを90度回転).
////	D3DXVECTOR3 sideDirection;
////	sideDirection.x = -directionToPlayer.z;
////	sideDirection.y = 0.0f;
////	sideDirection.z = directionToPlayer.x;
////
////	//ボスの位置を左右に動かす.
////	D3DXVECTOR3 moveVelocity = sideDirection * m_MoveDirection * m_pOwner->GetMoveSpeed() * deltaTime;
////	m_pOwner->SetPosition(bossPos + moveVelocity);
////
////	// クールタイムが経過したら方向を反転させる
////	if (m_CurrentTime >= m_CooltimeDuration)
////	{
////		m_MoveDirection *= -1.0f; // 方向を反転
////		m_CurrentTime = 0.0f;      // タイマーをリセット
////	}
////}
//
//void BossMoveState::Update()
//{
//	// デルタタイムを取得し、時間を更新
//	float deltaTime = Time::GetInstance().GetDeltaTime();
//	m_CurrentTime += deltaTime;
//
//	// プレイヤーとボスの位置を取得
//	const D3DXVECTOR3& playerPos = m_pOwner->GetPlayer()->GetPosition();
//	const D3DXVECTOR3& bossPos = m_pOwner->GetPosition();
//
//	// 距離を計算（平方根の計算は重いので、距離の2乗で比較）
//	D3DXVECTOR3 distanceVec = playerPos - bossPos;
//	float distanceSq = D3DXVec3LengthSq(&distanceVec);
//
//	// --- ここからが攻撃選択ロジック ---
//	// 距離に応じて攻撃ステートに直接遷移
//	if (distanceSq <= 25.0f) // 距離が5m以下（5*5=25）なら斬り攻撃へ
//	{
//		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<ChargeSlashState>());
//		return;
//	}
//	else if (distanceSq > 25.0f && distanceSq <= 100.0f) // 距離が5mより遠く10m以内（10*10=100）ならため斬りへ
//	{
//		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<ChargeSlashState>());
//		return;
//	}
//	// --- 攻撃選択ロジックはここまで ---
//
//	// 攻撃範囲外なら左右移動を継続する
//	D3DXVECTOR3 directionToPlayer;
//	D3DXVec3Normalize(&directionToPlayer, &distanceVec);
//
//	D3DXVECTOR3 sideDirection;
//	sideDirection.x = -directionToPlayer.z;
//	sideDirection.y = 0.0f;
//	sideDirection.z = directionToPlayer.x;
//
//	D3DXVECTOR3 moveVelocity = sideDirection * m_MoveDirection * m_pOwner->GetMoveSpeed() * deltaTime;
//	m_pOwner->SetPosition(bossPos + moveVelocity);
//
//	if (m_CurrentTime >= m_CooltimeDuration)
//	{
//		m_MoveDirection *= -1.0f;
//		m_CurrentTime = 0.0f;
//	}
//}
//
//void BossMoveState::LateUpdate()
//{
//}
//
//void BossMoveState::Draw()
//{
//}
//
//void BossMoveState::Exit()
//{
//}
