#include "CameraManager.h"
#include "Game\02_Camera\CameraBase.h"

CameraManager::CameraManager()
	: m_wpCamera()
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::LateUpdate()
{
	m_wpCamera.lock()->Update();
}
