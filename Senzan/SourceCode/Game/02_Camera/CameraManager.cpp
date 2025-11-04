#include "CameraManager.h"
#include "CameraBase.h"

CameraManager::CameraManager()
	: m_Camera()
{
}

//-------------------------------------------------------------------------------.

CameraManager::~CameraManager()
{
}

//-------------------------------------------------------------------------------.

void CameraManager::AttachCamera(std::shared_ptr<CameraBase> pCamera)
{
	if (pCamera == nullptr) { return; }
	GetInstance().m_Camera = pCamera;
}

//-------------------------------------------------------------------------------.

void CameraManager::Update()
{
	if (GetInstance().m_Camera.expired()) { return; }
	GetInstance().m_Camera.lock()->Update();
}

//-------------------------------------------------------------------------------.

void CameraManager::ViewAndProjectionUpdate()
{
	if (GetInstance().m_Camera.expired()) { return; }
	GetInstance().m_Camera.lock()->ViewAndProjectionUpdate();
}

//-------------------------------------------------------------------------------.

bool CameraManager::IsPointInFrustum(const DirectX::XMFLOAT3& point)
{
	if (GetInstance().m_Camera.expired()) { return false; }
	return GetInstance().m_Camera.lock()->IsPointInFrustum(point);
}

bool CameraManager::VFCulling(const DirectX::XMFLOAT3& Position, const float radius)
{
	if (GetInstance().m_Camera.expired()) { return false; }
	return GetInstance().m_Camera.lock()->VFCulling(Position, radius);
}

//-------------------------------------------------------------------------------.

void CameraManager::SetPosition(const DirectX::XMFLOAT3& Position)
{
	if (GetInstance().m_Camera.expired()) { return; }
	GetInstance().m_Camera.lock()->SetPosition(Position);
}

//-------------------------------------------------------------------------------.

DirectX::XMFLOAT3 CameraManager::GetPosition()
{
	if (GetInstance().m_Camera.expired()) { return Axis::ZERO; }
	return GetInstance().m_Camera.lock()->GetPosition();
}

//-------------------------------------------------------------------------------.

void CameraManager::SetLook(const DirectX::XMFLOAT3& look)
{
	if (GetInstance().m_Camera.expired()) { return; }
	GetInstance().m_Camera.lock()->SetLook(look);
}

//-------------------------------------------------------------------------------.

DirectX::XMFLOAT3 CameraManager::GetLook()
{
	if (GetInstance().m_Camera.expired()) { return Axis::ZERO; }
	return GetInstance().m_Camera.lock()->GetLook();
}

//-------------------------------------------------------------------------------.

const DirectX::XMFLOAT3 CameraManager::GetLookDirection()
{
	if (GetInstance().m_Camera.expired()) { return Axis::ZERO; }
	return GetInstance().m_Camera.lock()->GetLookDirection();
}

//-------------------------------------------------------------------------------.

std::shared_ptr<CameraBase> CameraManager::GetCurrentCamera()
{
	if (GetInstance().m_Camera.expired()) { return nullptr; }
	return GetInstance().m_Camera.lock();
}

DirectX::XMMATRIX CameraManager::GetViewMatrix()
{
	if (GetInstance().m_Camera.expired()) { return DirectX::XMMATRIX(); }
	return GetInstance().m_Camera.lock()->GetViewMatrix();
}

//-------------------------------------------------------------------------------.

DirectX::XMMATRIX CameraManager::GetProjMatrix()
{
	if (GetInstance().m_Camera.expired()) { return DirectX::XMMATRIX(); }
	return GetInstance().m_Camera.lock()->GetProjMatrix();
}

//-------------------------------------------------------------------------------.

DirectX::XMMATRIX CameraManager::GetViewProjMatrix()
{
	if (GetInstance().m_Camera.expired()) { return DirectX::XMMATRIX(); }
	return GetInstance().m_Camera.lock()->GetViewProjMatrix();
}

//-------------------------------------------------------------------------------.

DirectX::XMFLOAT3 CameraManager::GetForwardVec()
{
	if (GetInstance().m_Camera.expired()) { return Axis::ZERO; }
	return GetInstance().m_Camera.lock()->GetForwardVec();
}

//-------------------------------------------------------------------------------.

DirectX::XMFLOAT3 CameraManager::GetRightVec()
{
	if (GetInstance().m_Camera.expired()) { return Axis::ZERO; }
	return GetInstance().m_Camera.lock()->GetRightVec();
}

//-------------------------------------------------------------------------------.

float CameraManager::GetYaw()
{
	if (GetInstance().m_Camera.expired()) { return 0.0f; }
	return GetInstance().m_Camera.lock()->GetYaw();
}

//-------------------------------------------------------------------------------.

float CameraManager::GetPitch()
{
	if (GetInstance().m_Camera.expired()) { return 0.0f; }
	return GetInstance().m_Camera.lock()->GetPitch();
}

