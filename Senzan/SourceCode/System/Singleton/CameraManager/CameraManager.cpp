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
}

void CameraManager::SetPosition(DirectX::XMFLOAT3 Position)
{
	if (m_wpCamera.expired()) { return; }
	m_wpCamera.lock()->SetPosition(Position);
}

void CameraManager::SetPosition(float x, float y, float z)
{
	if (m_wpCamera.expired()) { return; }
	m_wpCamera.lock()->SetPosition(DirectX::XMFLOAT3(x,y,z));
}

void CameraManager::SetLook(DirectX::XMFLOAT3 Position)
{
	if (m_wpCamera.expired()) { return; }
	m_wpCamera.lock()->SetLook(Position);
}

void CameraManager::SetLook(float x, float y, float z)
{
	if (m_wpCamera.expired()) { return; }
	m_wpCamera.lock()->SetLook(DirectX::XMFLOAT3(x,y,z));
}

void CameraManager::ViewAndProjectionUpdate()
{
	if (m_wpCamera.expired()) { return; }
	m_wpCamera.lock()->ViewAndProjectionUpdate();
}
