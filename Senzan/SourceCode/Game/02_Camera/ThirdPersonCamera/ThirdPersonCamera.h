#pragma once
#include "Game/02_Camera/CameraBase.h"

class Player;

/******************************
*   三人称カメラ.
******************************/
class ThirdPersonCamera
	: public CameraBase
{
public:
	ThirdPersonCamera();
	virtual ~ThirdPersonCamera()override;

	virtual void Update()override;

private:
	DirectX::XMFLOAT2 m_MoveVec; // カメラの移動方向.
};
