#pragma once
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
#include <DirectXMath.h>

class SnowBall : public MeshObject
{
public:
	SnowBall();
	~SnowBall() override;

	void Update() override;
	void LateUpdate() override;
	void Draw() override;

	void Fire(const DirectX::XMFLOAT3 PlayerPos, const DirectX::XMFLOAT3 BossPos);
	void ResetPosition();

public:
	bool IsAction = false;
	bool IsVisible = false;

private:
	void Launch();

private:
	DirectX::XMFLOAT3 Player_Pos; // P2 (着弾点)
	DirectX::XMFLOAT3 Boss_Pos;   // P0 (開始点)
	DirectX::XMFLOAT3 Current_Pos; // P1 (制御点)
	DirectX::XMFLOAT3 Init_Pos;    // 待機位置

	float ThrowingTime = 0.0f;
	const float Totle_ThrowingTime = 1.5f; // 1.5秒で着弾（3.0から修正）
};