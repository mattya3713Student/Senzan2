#include "System/Singleton/Debug/CollisionVisualizer/CollisionVisualizer.h"

#include "CapsuleCollider.h"

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
    SetDebugInfo();
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
    DirectX::XMMATRIX m_parent_world = p_parent_transform.GetWorldMatrix();

    // オフセット行列.
    DirectX::XMMATRIX m_offset = DirectX::XMMatrixTranslation(
        m_PositionOffset.x, m_PositionOffset.y, m_PositionOffset.z
    );
    // 親のワールド行列にオフセットを乗算してデバッグ用ワールド行列を作成.
    DirectX::XMMATRIX m_debug_world = DirectX::XMMatrixMultiply(m_offset, m_parent_world);

    info.ShapeType = eShapeType::Capsule;
    info.Color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // 仮の色: 赤

    DirectX::XMStoreFloat4x4(&info.WorldMatrix, DirectX::XMMatrixTranspose(m_debug_world));

    // 半径を Data0.x に、高さを Data0.y に設定.
    info.Data0 = DirectX::XMFLOAT4(m_Radius, m_Height, 0.0f, 0.0f);
    info.Data1 = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

    visualizer.RegisterCollider(info);
}
