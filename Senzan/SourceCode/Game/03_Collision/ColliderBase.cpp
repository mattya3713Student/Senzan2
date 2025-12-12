#include "ColliderBase.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"

#include "System/Utility/Transform/Transform.h"
#include "System/Singleton/CameraManager/CameraManager.h"

#include "Box/BoxCollider.h"
#include "Capsule/CapsuleCollider.h"
#include "Sphere/SphereCollider.h"

ColliderBase::ColliderBase()
	: m_wpTransform     (  )
	, m_PositionOffset	( 0.0f, 0.0f, 0.0f )
	, m_Color			( 1.0f, 1.0f, 1.0f, 0.0f )	// デフォルトは白表示.
{
}
ColliderBase::ColliderBase(std::weak_ptr<const Transform> parentTransform)
	: m_wpTransform     ( parentTransform )
	, m_PositionOffset	( 0.0f, 0.0f, 0.0f )
	, m_Color(1.0f, 1.0f, 1.0f, 0.0f)	// デフォルトは白表示.
{
}

ColliderBase::~ColliderBase()
{
}
