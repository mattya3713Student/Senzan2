#pragma once
class ColliderBase;

// ColliderSpec: コライダー作成時に使用する仕様構造体 (生成用)
struct ColliderSpec
{
	float Radius = 1.0f;                    // カプセル半径
	float Height = 1.0f;                    // カプセル高さ
	DirectX::XMFLOAT3 Offset = {0.0f,0.0f,0.0f}; // 親からのオフセット (ContactPoint 相当)

	// 攻撃力などの動作値 (当面は AttackAmount を利用)
	float AttackAmount = 0.0f;

	// マスクは uint32_t で保持 (eCollisionGroup のビットを格納する)
	uint32_t MyMask = 0;
	uint32_t TargetMask = 0;

	// デバッグ描画用カラー
	DirectX::XMFLOAT4 DebugColor = {1.0f, 0.0f, 0.0f, 1.0f};

	bool Active = false; // 初期アクティブ状態
};

// 衝突結果構造体 (response)
struct CollisionInfo
{
	// 衝突が発生したかどうか.
	bool IsHit = false;

	// 衝突法線.
	DirectX::XMFLOAT3 Normal = {};

	// めり込みの深さ.
	float PenetrationDepth = 0.0f;

	// 接触点（ワールド座標）
	DirectX::XMFLOAT3 ContactPoint = {};

	// 攻撃力を入れる.
    // TODO : 当たり判定間で数字のやり取りがしやすいようにするための仕組みを検討.
	float AttackAmount = 0.0f;

	// 衝突に関わったコライダーへのポインタ.
	const ColliderBase* ColliderA = nullptr;
	const ColliderBase* ColliderB = nullptr;
};

