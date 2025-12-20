#include "SnowBall.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "..//04_Time/Time.h"

#include <algorithm>

SnowBall::SnowBall()
	: MeshObject()

	, Totle_ThrowingTime(3.0f)
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("snowball_nomal"));
	//雪玉の配置とサイズの変更.
	//最初の生成位置.
	DirectX::XMFLOAT3 pos = { 1000.0f, -1000.0f, 1000.0f };
	DirectX::XMFLOAT3 scale = { 0.05f, 0.05f, 0.05f };
	m_spTransform->SetPosition(pos);
	m_spTransform->SetScale(scale);

	IsVisible = false;
}

SnowBall::~SnowBall()
{
}

void SnowBall::Update()
{
	//ここに二次ベジェ曲線のコードを実装する.
	using namespace DirectX;

	// 雪玉が移動中でなければ、計算をスキップ
	if (!IsAction)
	{
		return;
	}

	// ----------------------------------------------------
	// 1. 時間 t の計算

	float deltaTime = Time::GetInstance().GetDeltaTime() * 0.1f;
	ThrowingTime += deltaTime;

	// t を計算し、1.0 に制限
	float t = ThrowingTime / Totle_ThrowingTime;
	t = std::min(t, 1.0f);

	// ----------------------------------------------------
	// 2. 二次ベジェ曲線による位置の計算 (3点の XMVECTOR を使用)

	// P0, P1, P2 を XMVECTOR にロード
	XMVECTOR P0 = XMLoadFloat3(&Boss_Pos);
	XMVECTOR P2 = XMLoadFloat3(&Player_Pos);
	XMVECTOR P1 = XMLoadFloat3(&Current_Pos);

	// Step A: Lerp Level 1
	// A = Lerp(P0, P1, t)
	XMVECTOR A = XMVectorLerp(P0, P1, t);
	// B = Lerp(P1, P2, t)
	XMVECTOR B = XMVectorLerp(P1, P2, t);

	// Step B: Lerp Level 2 (最終位置)
	// B(t) = Lerp(A, B, t)
// Step B: Lerp Level 2 (最終位置 B(t) を求める)
	XMVECTOR NewPosition_Vec = XMVectorLerp(A, B, t);

	// ----------------------------------------------------
	// 修正: XMVECTOR -> XMFLOAT3 への変換

	DirectX::XMFLOAT3 NewPosition_F;
	// XMVECTORの値をNewPosition_Fに格納する
	XMStoreFloat3(&NewPosition_F, NewPosition_Vec);

	// 計算結果をTransformに反映（XMFLOAT3を渡す）
	m_spTransform->SetPosition(NewPosition_F);


	if (t >= 1.0f)
	{
		// 移動完了 = 着弾処理
		IsAction = false;
	}

}

void SnowBall::LateUpdate()
{
}

void SnowBall::Draw()
{
	if (IsVisible)
	{
		MeshObject::Draw();
	}
}

void SnowBall::Fire(
	const DirectX::XMFLOAT3 PlayerPos,
	const DirectX::XMFLOAT3 BossPos)
{
	//外部から(GameMain)座標の取得.

	Player_Pos = PlayerPos;
	Boss_Pos = BossPos;

	Launch();
}

void SnowBall::ResetPosition()
{
	//雪玉を元の位置に戻す
	m_spTransform->SetPosition(Init_Pos);

	//非表示にする
	IsVisible = false;

	//次の発射のために移動フラグをリセット
	IsAction = false;
}


void SnowBall::Launch()
{
	const float Height = 8.0f;

	//=====================================================================
	// XMFLOAT3をVECTORに変換する.
	//=====================================================================
	//XMFLOAT3からVECTORに変換する.
	//BossのFLOAT3をVECTORに変換している.
	DirectX::XMVECTOR P0 = DirectX::XMLoadFloat3(&Boss_Pos);
	//PlayerのFLOAT3をVECTORに変換している.
	DirectX::XMVECTOR P2 = DirectX::XMLoadFloat3(&Player_Pos);

	//P0とP2の水平中間点Mを求める(二次ベジェ曲線).
	DirectX::XMVECTOR MidPoint = DirectX::XMVectorLerp(P0, P2, 0.5f);

	//曲線のY軸を8.0fに変更している.
	DirectX::XMVECTOR Height_Offset = DirectX::XMVectorSet(0.0f, Height, 0.0f, 0.0f);

	DirectX::XMVECTOR P1_Vec = DirectX::XMVectorAdd(MidPoint, Height_Offset);
	DirectX::XMStoreFloat3(&Current_Pos, P1_Vec);

	// 4. 初期位置を P0 に設定
	m_spTransform->SetPosition(Boss_Pos);

	// 5. 時間をリセットし、移動を有効化
	ThrowingTime = 0.0f;
	IsAction = true;
	IsVisible = true;
}
