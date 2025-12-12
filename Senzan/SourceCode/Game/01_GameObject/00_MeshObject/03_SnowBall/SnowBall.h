#pragma once

#include "Game//01_GameObject//00_MeshObject//MeshObject.h"
#include "00_MeshObject/03_SnowBall/SnowBall.h"

/***********************************************************
*	雪玉クラス.
*	投擲の時にプレイヤーの方に飛ばす.
**/

class Player;
class Boss;
class Time;

class SnowBall
	: public MeshObject
{
public:
	SnowBall();
	~SnowBall() override;

	void Update() override;
	void LateUpdate() override;
	void Draw() override;

	//自分の設定させたいPlayerとBossの位置を取得する
	void Fire(
		const DirectX::XMFLOAT3 PlayerPos,
		const DirectX::XMFLOAT3 BossPos);

	//雪玉を時間経過で初期の位置に表示する.
	void ResetPosition();

public:

	bool IsAction = false;

	bool IsVisible = true;


private:
	//Playerの位置の取得(P2).
	DirectX::XMFLOAT3 Player_Pos;
	//Bossの位置の取得(P0).
	DirectX::XMFLOAT3 Boss_Pos;

	//二次ベジェ曲線の制御部分(P1).
	DirectX::XMFLOAT3 Current_Pos;

	DirectX::XMFLOAT3 Init_Pos;

	//投擲の時間系統.
	float ThrowingTime = 0.0f;
	//今投擲攻撃をしているのか.

	//投擲攻撃の投擲時間.
	const float Totle_ThrowingTime;

	void Launch();

};