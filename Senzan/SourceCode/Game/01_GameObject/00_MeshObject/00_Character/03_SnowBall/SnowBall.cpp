#include "SnowBall.h"
#include "Game/04_Time/Time.h"
#include <algorithm>

SnowBall::SnowBall()
	: MeshObject()
	, Totle_ThrowingTime(1.5f)
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("snowball_nomal"));

	// 初期位置を画面外の遠くへ
	Init_Pos = { 1000.0f, -1000.0f, 1000.0f };
	DirectX::XMFLOAT3 scale = { 0.05f, 0.05f, 0.05f }; // サイズを少し大きく修正

	m_spTransform->SetPosition(Init_Pos);
	m_spTransform->SetScale(scale);

	IsVisible = false;
	IsAction = false;
}

SnowBall::~SnowBall()
{
}

void SnowBall::Update()
{
	using namespace DirectX;

	if (!IsAction) return;

	// 1. 時間の更新（deltaTimeに余計な倍率をかけない）
	float deltaTime = Time::GetInstance().GetDeltaTime();
	ThrowingTime += deltaTime;

	// t (0.0 ～ 1.0) を計算
	float t = ThrowingTime / Totle_ThrowingTime;
	if (t >= 1.0f) t = 1.0f;

	// 2. 二次ベジェ曲線計算
	XMVECTOR P0 = XMLoadFloat3(&Boss_Pos);
	XMVECTOR P1 = XMLoadFloat3(&Current_Pos);
	XMVECTOR P2 = XMLoadFloat3(&Player_Pos);

	// 二次ベジェ曲線の公式： B(t) = (1-t)^2*P0 + 2(1-t)t*P1 + t^2*P2
	XMVECTOR A = XMVectorLerp(P0, P1, t);
	XMVECTOR B = XMVectorLerp(P1, P2, t);
	XMVECTOR NewPosVec = XMVectorLerp(A, B, t);

	// 3. 座標の適用
	DirectX::XMFLOAT3 NewPosF;
	XMStoreFloat3(&NewPosF, NewPosVec);
	m_spTransform->SetPosition(NewPosF);

	// 4. 着弾判定
	if (t >= 1.0f)
	{
		IsAction = false;
		// 着弾後、非表示にする場合は以下を有効化
		// IsVisible = false;
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

void SnowBall::Fire(const DirectX::XMFLOAT3 PlayerPos, const DirectX::XMFLOAT3 BossPos)
{
	Player_Pos = PlayerPos;
	Boss_Pos = BossPos;

	Launch();
}

void SnowBall::ResetPosition()
{
	m_spTransform->SetPosition(Init_Pos);
	IsVisible = false;
	IsAction = false;
}

void SnowBall::Launch()
{
	using namespace DirectX;
	const float ArcHeight = 10.0f; // 放物線の高さ

	XMVECTOR P0 = XMLoadFloat3(&Boss_Pos);
	XMVECTOR P2 = XMLoadFloat3(&Player_Pos);

	// 中間地点を計算して高さを出す
	XMVECTOR MidPoint = XMVectorLerp(P0, P2, 0.5f);
	XMVECTOR HeightOffset = XMVectorSet(0.0f, ArcHeight, 0.0f, 0.0f);

	XMVECTOR P1_Vec = XMVectorAdd(MidPoint, HeightOffset);
	XMStoreFloat3(&Current_Pos, P1_Vec);

	m_spTransform->SetPosition(Boss_Pos);
	ThrowingTime = 0.0f;
	IsAction = true;
	IsVisible = true;
}
