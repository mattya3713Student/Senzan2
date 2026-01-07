#include "GameObject.h"
#include "System/Utility/Transform/Transform.h"
#include "Utility\Math\Math.h"
#include "Game/04_Time/Time.h"   

GameObject::GameObject()
	: m_spTransform		( std::make_shared<Transform>() )
	, m_Tag			( "Untagged" )
	, m_IsActive		( true )
	, m_IsRenderActive	( true )
	, m_TimeScale		( -1.0f )
{
}

GameObject::~GameObject()
{
}

// �R�s�[.
void GameObject::SetTransform(const Transform& transform) 
{
	// m_spTransform���L���ȃ|�C���^������Ă��邩�m�F���A���g��㏑������.
	if (m_spTransform) {
		* m_spTransform = transform;
	}
	else {
		// null�̏ꍇ�͐V�����쐬.
		m_spTransform = std::make_shared<Transform>(transform);
	}
}

// ====================================================================================================
// ���W�ݒ� (SetPosition)
// ====================================================================================================

void GameObject::SetPosition(const DirectX::XMFLOAT3& Position)
{
    m_spTransform->Position = Position;
}

void GameObject::SetPosition(float X, float Y, float Z)
{
    m_spTransform->Position = DirectX::XMFLOAT3(X, Y, Z);
}

void GameObject::SetPositionX(float X)
{
    m_spTransform->Position.x = X;
}

void GameObject::SetPositionY(float Y)
{
    m_spTransform->Position.y = Y;
}

void GameObject::SetPositionZ(float Z)
{
    m_spTransform->Position.z = Z;
}


// ====================================================================================================
// ���W���Z (AddPosition)
// ====================================================================================================

void GameObject::AddPosition(const DirectX::XMFLOAT3& Position)
{
    // SIMD���Z��g�p���ĉ��Z
    DirectX::XMVECTOR v_pos = DirectX::XMLoadFloat3(&m_spTransform->Position);
    DirectX::XMVECTOR v_add = DirectX::XMLoadFloat3(&Position);
    DirectX::XMVECTOR v_new = DirectX::XMVectorAdd(v_pos, v_add);
    DirectX::XMStoreFloat3(&m_spTransform->Position, v_new);
}

void GameObject::AddPosition(float X, float Y, float Z)
{
    m_spTransform->Position.x += X;
    m_spTransform->Position.y += Y;
    m_spTransform->Position.z += Z;
}

void GameObject::AddPositionX(float X)
{
    m_spTransform->Position.x += X;
}

void GameObject::AddPositionY(float Y)
{
    m_spTransform->Position.y += Y;
}

void GameObject::AddPositionZ(float Z)
{
    m_spTransform->Position.z += Z;
}


// ====================================================================================================
// ��]�ݒ� (SetRotation)
// ====================================================================================================

void GameObject::SetRotation(const DirectX::XMFLOAT3& Rotation)
{
    m_spTransform->Rotation = Rotation;
    m_spTransform->UpdateQuaternionFromRotation();
}

void GameObject::SetRotation(float X, float Y, float Z)
{
    m_spTransform->Rotation = DirectX::XMFLOAT3(X, Y, Z);
    m_spTransform->UpdateQuaternionFromRotation(); 
}

void GameObject::SetRotationX(float X)
{
    m_spTransform->Rotation.x = X;
    m_spTransform->UpdateQuaternionFromRotation(); 
}

void GameObject::SetRotationY(float Y)
{
    m_spTransform->Rotation.y = Y;
    m_spTransform->UpdateQuaternionFromRotation(); 
}

void GameObject::SetRotationZ(float Z)
{
    m_spTransform->Rotation.z = Z;
    m_spTransform->UpdateQuaternionFromRotation();
}

void GameObject::SetRotationAroundAxis(const DirectX::XMFLOAT3& Axis, float Angle)
{
    DirectX::XMVECTOR v_axis = DirectX::XMLoadFloat3(&Axis);
    DirectX::XMVECTOR q_axis = DirectX::XMQuaternionRotationAxis(v_axis, Angle);
    DirectX::XMVECTOR q_current = DirectX::XMLoadFloat4(&m_spTransform->Quaternion);

    // ���݂̃N�H�[�^�j�I���ɐV������]���Z
    DirectX::XMVECTOR q_new = DirectX::XMQuaternionMultiply(q_current, q_axis);
    DirectX::XMStoreFloat4(&m_spTransform->Quaternion, q_new);

    m_spTransform->UpdateRotationFromQuaternion();
}


// ====================================================================================================
// �g�k�ݒ� (SetScale)
// ====================================================================================================

void GameObject::SetScale(const DirectX::XMFLOAT3& Scale)
{
    m_spTransform->Scale = Scale;
}

void GameObject::SetScale(float XYZ)
{
    m_spTransform->Scale = DirectX::XMFLOAT3(XYZ, XYZ, XYZ);
}

void GameObject::SetScale(float X, float Y, float Z)
{
    m_spTransform->Scale = DirectX::XMFLOAT3(X, Y, Z);
}

void GameObject::SetScaleX(float X)
{
    m_spTransform->Scale.x = X;
}

void GameObject::SetScaleY(float Y)
{
    m_spTransform->Scale.y = Y;
}

void GameObject::SetScaleZ(float Z)
{
    m_spTransform->Scale.z = Z;
}

//-----------------------------------------------------------------------.

const std::string& GameObject::GetTag() const
{
	return m_Tag;
}

//-----------------------------------------------------------------------.

void GameObject::SetTag(const std::string& tag)
{
	m_Tag = tag;
}

//-----------------------------------------------------------------------.

const bool GameObject::IsActive() const
{
	return m_IsActive;
}

//-----------------------------------------------------------------------.

void GameObject::SetIsActive(const bool isActive)
{
	m_IsActive = isActive;
}

//-----------------------------------------------------------------------.

const bool GameObject::IsRenderActive() const
{
	return m_IsRenderActive;
}

//-----------------------------------------------------------------------.

void GameObject::SetIsRenderActive(const bool isActive)
{
    m_IsRenderActive = isActive;
}

void GameObject::SetTimeScale(const float NewTimeScale)
{
	m_TimeScale = NewTimeScale;
}

float GameObject::GetTimeScale()
{
    if (MyMath::IsNearlyEqual(m_TimeScale, -1.f))
    {
        return Time::GetInstance().GetWorldTimeScale();
    }
    else
    {
        return m_TimeScale;
    }
}

// �ŏI�I��DeltaTime�̎擾.
float GameObject::GetDelta()
{
    // Time::GetDeltaTime() now returns the raw elapsed seconds.
    float delta_time = Time::GetInstance().GetDeltaTime();

    // If object uses world scale (m_TimeScale == -1), apply world time scale.
    if (MyMath::IsNearlyEqual(m_TimeScale, -1.f))
    {
        return delta_time * Time::GetInstance().GetWorldTimeScale();
    }
    else
    {
        // Apply object's custom time scale.
        return delta_time * m_TimeScale;
    }
}

