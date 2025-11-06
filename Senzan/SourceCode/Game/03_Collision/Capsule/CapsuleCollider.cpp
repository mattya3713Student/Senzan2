#include "System/Singleton/Debug/CollisionVisualizer/CollisionVisualizer.h"

#include "CapsuleCollider.h"
#include "Game/03_Collision/Box/BoxCollider.h"
#include "Game/03_Collision/Sphere/SphereCollider.h"

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

	// 形状ごとの衝突判定へディスパッチ.
    return other.DispatchCollision(*this);
}

bool CapsuleCollider::DispatchCollision(const SphereCollider& other) const
{
    // 自分のカプセルの線分 P1, P2 の終点を計算.
    const DirectX::XMVECTOR p1 = GetCulcCapsuleSegmentStart(this);
    const DirectX::XMVECTOR p2 = GetCulcCapsuleSegmentEnd(this);

    // 相手 (球体) の中心座標 Q.
	const DirectX::XMFLOAT3 spherePos = other.GetPosition();
    const DirectX::XMVECTOR q = DirectX::XMLoadFloat3(&spherePos);

    // 線分 P1P2 と 点 Q の最短距離の二乗を計算.
    const float distSq = GetCulcClosestPtPointSegmentSq(q, p1, p2);

    // 許容距離（半径の合計）を計算し、二乗して比較.
    const float requiredDistance = GetRadius() + other.GetRadius();

    return distSq <= (requiredDistance * requiredDistance);
}

bool CapsuleCollider::DispatchCollision(const CapsuleCollider& other) const
{
    // 自分の線分 P1, P2 の終点を計算.
    const DirectX::XMVECTOR p1 = GetCulcCapsuleSegmentStart(this);
    const DirectX::XMVECTOR p2 = GetCulcCapsuleSegmentEnd(this);

    // 相手の線分 Q1, Q2 の終点を計算.
    const DirectX::XMVECTOR q1 = GetCulcCapsuleSegmentStart(&other);
    const DirectX::XMVECTOR q2 = GetCulcCapsuleSegmentEnd(&other);

    // 3. 線分 P1P2 と 線分 Q1Q2 の最短距離の二乗を計算.
    const float distSq = GetCulcClosestPtSegmentSegmentSq(p1, p2, q1, q2);

    // 4. 許容距離（半径の合計）を計算し、二乗して比較.
    const float requiredDistance = GetRadius() + other.GetRadius();

    return distSq <= (requiredDistance * requiredDistance);
}

bool CapsuleCollider::DispatchCollision(const BoxCollider& other) const
{
    // 1. 自分のカプセルの線分 P1, P2 の終点を計算
      // const DirectX::XMVECTOR p1 = GetCulcCapsuleSegmentStart(this);
      // const DirectX::XMVECTOR p2 = GetCulcCapsuleSegmentEnd(this);

      // 2. OBB（other）と線分（P1P2）の最短距離の二乗を計算する
      // OBB-Segment の最短距離アルゴリズム（Boxのローカル空間への変換とクランプ）が必要です。

      // const float distSq = 9999.0f; // OBB-Segment::ClosestPtSegmentOBBSq(...)

      // 3. 判定: 最短距離がカプセルの半径の二乗以下なら衝突
      // const float requiredRadius = GetRadius();

      // return distSq <= (requiredRadius * requiredRadius);

      // 💡 Boxとの判定ロジックが未実装のため、一旦 false を返す
    return false;
}

// カプセルの中心線分の終点 P1 を「計算し」取得.
DirectX::XMVECTOR CapsuleCollider::GetCulcCapsuleSegmentStart(const CapsuleCollider* capsule)
{
    // 相手 (球体) の中心座標 Q.
    const DirectX::XMFLOAT3 sphere_pos = capsule->GetPosition();
    const DirectX::XMVECTOR v_pos = DirectX::XMLoadFloat3(&sphere_pos);
    const float radius = capsule->GetRadius();
    const float height = capsule->GetHeight();

    // カプセルの線分長 = 全体の高さ - 両端の半球半径 (2 * Radius).
    const float half_segment_length = (height - 2.0f * radius) * 0.5f;

    // Y軸沿いの線分と仮定.
    const DirectX::XMVECTOR v_y = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return DirectX::XMVectorSubtract(v_pos, DirectX::XMVectorScale(v_y, half_segment_length));
}

// カプセルの中心線分の終点 P2 を「計算し」取得.
DirectX::XMVECTOR CapsuleCollider::GetCulcCapsuleSegmentEnd(const CapsuleCollider* capsule)
{
    const DirectX::XMFLOAT3 sphere_pos = capsule->GetPosition();
    const DirectX::XMVECTOR v_pos = DirectX::XMLoadFloat3(&sphere_pos);
    const float radius = capsule->GetRadius();
    const float height = capsule->GetHeight();

    // カプセルの線分長 = 全体の高さ - 両端の半球半径 (2 * Radius).
    const float half_segment_length = (height - 2.0f * radius) * 0.5f;

    // Y軸沿いの線分と仮定.
    const DirectX::XMVECTOR v_y = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return DirectX::XMVectorAdd(v_pos, DirectX::XMVectorScale(v_y, half_segment_length));
}

// 点 P から線分 AB への最短距離の二乗を「計算し」取得.
float CapsuleCollider::GetCulcClosestPtPointSegmentSq(DirectX::XMVECTOR p, DirectX::XMVECTOR a, DirectX::XMVECTOR b)
{
    DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a); // 線分 AB のベクトル
    DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(p, a); // 点 A から点 P へのベクトル

    // ap を ab に投影し、スカラー値 e (Dot(ap, ab)) を求める.
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ap, ab));

    // 1. e <= 0.0f の場合: 点 A の外側 (A が最短).
    if (e <= 0.0f) {
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ap));
    }

    float f = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ab)); // 線分 AB の長さの二乗.

    // 2. e >= f の場合: 点 B の外側 (B が最短).
    if (e >= f) {
        DirectX::XMVECTOR bp = DirectX::XMVectorSubtract(p, b);
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(bp));
    }

    // 3. 0 < e < f の場合: 線分上に最短点がある.
    // 最短距離の二乗 = |ap|^2 - (e^2 / f).
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ap)) - e * e / f;
}

//  線分 AB と線分 CD の最短距離の二乗を「計算し」取得.
float CapsuleCollider::GetCulcClosestPtSegmentSegmentSq(
    DirectX::XMVECTOR a,
    DirectX::XMVECTOR b,
    DirectX::XMVECTOR c,
    DirectX::XMVECTOR d)
{
    // r = B - A (線分 AB の方向ベクトル).
    const DirectX::XMVECTOR r = DirectX::XMVectorSubtract(b, a);
    // s = D - C (線分 CD の方向ベクトル).
    const DirectX::XMVECTOR s = DirectX::XMVectorSubtract(d, c);
    // e = A - C (線分 AC のベクトル).
    const DirectX::XMVECTOR e = DirectX::XMVectorSubtract(a, c);

    // 各種のドット積を計算.
    const float a_val = DirectX::XMVectorGetX(DirectX::XMVector3Dot(r, r)); // |r|^2 (a = r . r).
    const float e_val = DirectX::XMVectorGetX(DirectX::XMVector3Dot(r, s)); // r . s.
    const float f_val = DirectX::XMVectorGetX(DirectX::XMVector3Dot(s, s)); // |s|^2 (f = s . s).

    const float g_val = DirectX::XMVectorGetX(DirectX::XMVector3Dot(r, e)); // r . e.
    const float h_val = DirectX::XMVectorGetX(DirectX::XMVector3Dot(s, e)); // s . e.

    // 許容誤差（ゼロ除算防止）.
    const float EPSILON = 1e-6f;

    float s_param, t_param;

    // k = a*f - e^2 (2x2行列の行列式).
    const float k = a_val * f_val - e_val * e_val;

    if (k < EPSILON) {
        // 線分 AB と CD がほぼ平行な場合 (k ≈ 0).
        // s=0 (点A)に固定して t を解く.
        s_param = 0.0f;

        // t を [0, 1] にクランプ
        if (f_val < EPSILON) {
            t_param = 0.0f; // 線分 CD が点に近い場合.
        }
        else {
            t_param = h_val / f_val;

            // クランプ処理 (std::min/maxを使わずif文で処理).
            if (t_param < 0.0f) t_param = 0.0f;
            if (t_param > 1.0f) t_param = 1.0f;
        }

    }
    else {
        // 線分が平行でない一般のケース.
        float inv_k = 1.0f / k;
        s_param = (e_val * h_val - f_val * g_val) * inv_k;

        // 1. s の値を [0, 1] の範囲にクランプ.
        if (s_param < 0.0f) {
            s_param = 0.0f;
        }
        else if (s_param > 1.0f) {
            s_param = 1.0f;
        }

        // 2. s のクランプ値を使って t を再計算し、t をクランプ.
        if (f_val < EPSILON) {
            t_param = 0.0f;
        }
        else {
            t_param = (s_param * e_val + h_val) / f_val;
        }

        if (t_param < 0.0f) {
            t_param = 0.0f;
        }
        else if (t_param > 1.0f) {
            t_param = 1.0f;
        }

        // 3. t のクランプ値を使って s を再計算し、s をクランプ (最短点チェック).
        if (t_param == 0.0f || t_param == 1.0f) {
            if (t_param == 0.0f) {
                // t = 0 (点C)からの最短点.
                s_param = -g_val / a_val;
            }
            else { // t = 1.0f (点D)からの最短点.
                s_param = (e_val - g_val) / a_val;
            }

            // s を最終的に [0, 1] にクランプ.
            if (s_param < 0.0f) s_param = 0.0f;
            if (s_param > 1.0f) s_param = 1.0f;
        }
    }

    // 最短点 P(s) と Q(t) を計算.
    const DirectX::XMVECTOR closest_p = DirectX::XMVectorAdd(a, DirectX::XMVectorScale(r, s_param));
    const DirectX::XMVECTOR closest_q = DirectX::XMVectorAdd(c, DirectX::XMVectorScale(s, t_param));

    // 距離の二乗を計算し、返す.
    const DirectX::XMVECTOR pq = DirectX::XMVectorSubtract(closest_p, closest_q);
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(pq));
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
