#pragma once

#include "Game//01_GameObject//00_MeshObject//MeshObject.h"

/***********************************************************
*	雪玉クラス.
*	投擲の時にプレイヤーの方に飛ばす.
**/

class SnowBall
	: public MeshObject
{
public:
	SnowBall();
	~SnowBall() override;

	void Update() override;
	void LateUpdate() override;
	void Draw() override;

private:

};