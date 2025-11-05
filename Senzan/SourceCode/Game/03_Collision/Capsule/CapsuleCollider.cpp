
#include "CapsuleCollider.h"
#include "../CollisionDetector.h"

#include "System/Singleton/Debug/CollisionVisualizer/CollisionVisualizer.h"

namespace {
// 分割数.
static constexpr int DIVIDE_X = 16;
static constexpr int DIVIDE_Y = 16;
}


CapsuleCollider::CapsuleCollider()
	: ColliderBase	()
	, m_Radius		( 0.5f )
	, m_Height		( 2.0f )
{
}

CapsuleCollider::CapsuleCollider(std::weak_ptr<const Transform> parentTransform)
	: ColliderBase	(parentTransform)
	, m_Radius		( 0.5f )
	, m_Height		( 2.0f )
{
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::Update()
{

}

bool CapsuleCollider::CheckCollision(const ColliderBase& other) const
{
	// フィルター判断.
    if (!ShouldCollide(other)) {
        return false;
    }

    return CollisionDetector::CheckCollision(*this, other);
}

bool CapsuleCollider::DispatchCollision(const SphereCollider& other) 
{
    return false;
}

bool CapsuleCollider::DispatchCollision(const CapsuleCollider& other)
{
    return false;
}

bool CapsuleCollider::DispatchCollision(const BoxCollider& other)
{
    return false;
}

// デバッグ描画用設定.
void CapsuleCollider::SetDebugInfo()
{
    CollisionVisualizer& visualizer = CollisionVisualizer::GetInstance();
    DebugColliderInfo info = {};

    std::shared_ptr<const Transform> spParentTransform = m_wpTransform.lock();
    if (!spParentTransform) { return; }
    const Transform& p_parent_transform = *spParentTransform;

    // 親のワールド行列.
    DirectX::XMMATRIX mat_parent_world = p_parent_transform.GetWorldMatrix();

    // オフセット行列.
    DirectX::XMMATRIX mat_offset = DirectX::XMMatrixTranslation(
        m_PositionOffset.x, m_PositionOffset.y, m_PositionOffset.z
    );
    // 親のワールド行列にオフセットを乗算してデバッグ用ワールド行列を作成.
    DirectX::XMMATRIX mat_debug_world = DirectX::XMMatrixMultiply(mat_offset, mat_parent_world);

    info.ShapeType = eShapeType::Capsule;
    info.Color = m_Color; 

    DirectX::XMStoreFloat4x4(&info.WorldMatrix, mat_debug_world);

    // 半径を Data0.x に、高さを Data0.y に設定.
    info.Data0 = DirectX::XMFLOAT4(m_Radius, m_Height, 0.0f, 0.0f);
    info.Data1 = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

    visualizer.RegisterCollider(info);
}
