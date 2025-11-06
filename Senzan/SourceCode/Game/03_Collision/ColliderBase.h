#pragma once
#include "CollisionInfo.h"

class CollisionDetector;
class BoxCollider;
class CapsuleCollider;
class SphereCollider;

/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/10/5.
* @brief     : 当たり判定(基底).
**********************************************************************************/

class ColliderBase
{
public:
	// 形状派生クラスから判定関数(DispatchCollision)を2重ディスパッチするためフレンドに追加.
	friend class BoxCollider;
	friend class CapsuleCollider;
	friend class SphereCollider;

public:
	// 当たり判定.
	enum class eCollisionGroup : uint32_t
	{
		None = 0,

		Player_Attack		= 1 << 0,
		Player_Damage		= 1 << 1,
		Player_Dodge		= 1 << 2,
		Player_JustDodge	= 1 << 3,
		Enemy_Attack		= 1 << 4,
		Enemy_Damage		= 1 << 5,
		Press				= 1 << 6,

		_Max = 0xFFFFFFFF,
	}; 

	// 当たり判定の形.
	enum class eShapeType : uint32_t
	{
		Box = 0,
		Sphere,
		Capsule,
		_Max
	};

public:
	ColliderBase();
	ColliderBase(std::weak_ptr<const Transform> parentTransform);
	virtual ~ColliderBase();	

	virtual void Update() = 0;

	// 親のトランスフォームを取得する.
	//inline const std::weak_ptr<Transform>& GetParentTransform() const noexcept { return m_wpTransform; }

	// 座標を取得する.
	inline const DirectX::XMFLOAT3 GetPosition() const noexcept; 

	// 自身の形状を取得する.
	inline virtual const eShapeType GetShapeType() const noexcept = 0;

	// 座標のオフセット値を取得する.
	inline const DirectX::XMFLOAT3 GetPositionOffset() const noexcept { return m_PositionOffset; }
	inline void SetPositionOffset(DirectX::XMFLOAT3 NewOffset) noexcept { m_PositionOffset = NewOffset; }
	inline void SetPositionOffset(float NewOffset_x, float NewOffset_y, float NewOffset_z) noexcept { m_PositionOffset = DirectX::XMFLOAT3(NewOffset_x, NewOffset_y, NewOffset_z); }

	// 大きさ取得(派生で実装).
	virtual float GetRadius() const noexcept { return 0.0f; }
	virtual float GetHeight() const noexcept { return 0.0f; }
	virtual DirectX::XMFLOAT3 GetSize() const noexcept { return DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f); }

	// 大きさ設定(派生で実装).
	virtual void SetRadius(float Radius) noexcept {}
	virtual void SetHeight(float Height) noexcept {}
	virtual void SetSize(DirectX::XMFLOAT3 Size) noexcept {}

	// 色の設定.
	void SetColor(Color::eColor NewColor) noexcept { m_Color = Color::GetColorValue(NewColor); }
	inline void SetColor(DirectX::XMFLOAT4 NewColor) noexcept { m_Color = NewColor; }
	inline void SetColor(float NewR, float NewG, float NewB, float NewA) noexcept { m_Color = DirectX::XMFLOAT4(NewR, NewG, NewB, NewA); }

	// 衝突情報を追加する.
	inline void AddCollisionInfo(const CollisionInfo& info) noexcept { m_CollisionEvents.push_back(info); }

	// 衝突情報を取得する.
	inline const std::vector<CollisionInfo>& GetCollisionEvents() const noexcept { return m_CollisionEvents; }

	// 情報をクリアするメソッド.
	inline void ClearCollisionEvents() noexcept { m_CollisionEvents.clear(); }
	
	// 自身のグループを設定.
	inline void SetGroup(eCollisionGroup group) noexcept { m_MyGroup = group; }
	// 衝突を許可するグループを設定.
	inline void SetMask(eCollisionGroup mask) noexcept { m_CollisionMask = mask; }

	inline eCollisionGroup GetGroup() const noexcept { return m_MyGroup; }
	inline eCollisionGroup GetMask() const noexcept { return m_CollisionMask; }

	// 相手と衝突すべきか.
	inline bool ShouldCollide(const ColliderBase& other) const noexcept
	{
		bool A_collides_with_B = (m_CollisionMask & other.m_MyGroup) != 0;
		bool B_collides_with_A = (other.m_CollisionMask & m_MyGroup) != 0;
		return A_collides_with_B && B_collides_with_A;
	}

	// 他のColliderとの衝突.
	virtual CollisionInfo CheckCollision(const ColliderBase& other) const = 0;

protected:
	// 形状ごとの衝突処理.
	virtual CollisionInfo DispatchCollision(const SphereCollider& other) const = 0;
	virtual CollisionInfo DispatchCollision(const CapsuleCollider& other) const = 0;
	virtual CollisionInfo DispatchCollision(const BoxCollider& other) const = 0;

protected:

	std::weak_ptr<const Transform> m_wpTransform;	// 持ち主のトランスフォーム.
	DirectX::XMFLOAT3	m_PositionOffset;			// オフセット位置.

	// コリジョンフィルター用.
	eCollisionGroup m_MyGroup = eCollisionGroup::_Max;		// 自身が所属するグループ.
	eCollisionGroup m_CollisionMask = eCollisionGroup::_Max;	// 衝突対象とするグループ.

	// 検出された衝突情報のリスト.
	std::vector<CollisionInfo> m_CollisionEvents;

#if _DEBUG
public:

	//------------デバッグ描画用-----------.

	virtual void SetDebugInfo() = 0;

protected:
	DirectX::XMFLOAT4	m_Color;					// 表示色.

#endif // _DEBUG.

}; 

// 座標を取得する.
inline const DirectX::XMFLOAT3 ColliderBase::GetPosition() const noexcept
{
	if (auto spTransform = m_wpTransform.lock())
	{
		DirectX::XMVECTOR v_position = DirectX::XMLoadFloat3(&spTransform->Position);
		DirectX::XMVECTOR v_offset = DirectX::XMLoadFloat3(&m_PositionOffset);

		DirectX::XMVECTOR v_result_pos = DirectX::XMVectorAdd(v_position, v_offset);

		DirectX::XMFLOAT3 result_pos = {};
		DirectX::XMStoreFloat3(&result_pos, v_result_pos);
		return result_pos;
	}
	return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}