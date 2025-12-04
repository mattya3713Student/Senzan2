#pragma once

#include <DirectXMath.h>
#include <string>
#include <sstream>

struct Transform
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Rotation;		// オイラー角(ラジアン).
	DirectX::XMFLOAT4 Quaternion;	// クォータニオン.
	DirectX::XMFLOAT3 Scale;

	Transform()
		: Position(0.0f, 0.0f, 0.0f)
		, Rotation(0.0f, 0.0f, 0.0f)
		, Quaternion(0.0f, 0.0f, 0.0f, 1.0f)
		, Scale(1.0f, 1.0f, 1.0f)
	{
	}

	Transform(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& rotation,
		const DirectX::XMFLOAT4& quaternion,
		const DirectX::XMFLOAT3& scale)
		: Position(position)
		, Rotation(rotation)
		, Quaternion(quaternion)
		, Scale(scale)
	{
	}

	// ゲッター.
	DirectX::XMFLOAT3 GetForward() const;
	DirectX::XMFLOAT3 GetUp() const;
	DirectX::XMFLOAT3 GetRight() const;
	std::array<DirectX::XMFLOAT3, 3> GetAxisVector() const;
	DirectX::XMFLOAT3 GetRotationDegrees() const; // 角度でオイラー角を取得.

	// ワールド行列を生成.
	inline DirectX::XMMATRIX GetWorldMatrix() const
	{
		using namespace DirectX;
		XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&Scale));
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&Quaternion));
		XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&Position));

		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	// セッター.
	void SetPosition(const DirectX::XMFLOAT3& newPosition);
	void SetRotation(const DirectX::XMFLOAT3& eulerAngles); // ラジアンで設定.
	void SetRotationX(float X); // ラジアンで設定.
	void SetRotationY(float Y); // ラジアンで設定.
	void SetRotationZ(float Z); // ラジアンで設定.
	void SetRotationDegrees(const DirectX::XMFLOAT3& eulerAnglesInDegrees); // 角度で回転.
	void SetQuaternion(const DirectX::XMFLOAT4& newQuaternion);
	void SetScale(const DirectX::XMFLOAT3& newScale);

	// 移動・回転.
	void Translate(const DirectX::XMFLOAT3& translation);
	void Rotate(const DirectX::XMFLOAT3& eulerAngles); // ラジアンで回転を適用.
	void RotateDegrees(const DirectX::XMFLOAT3& eulerAnglesInDegrees); // 角度で回転を適用.
	void Rotate(const DirectX::XMFLOAT4& quaternion); // クォータニオンで回転を適用.

	// オイラー角とクォータニオンの同期.
	void UpdateRotationFromQuaternion();
	void UpdateQuaternionFromRotation();




	inline Transform operator+(const Transform& other) const
	{
		using namespace DirectX;
		XMVECTOR pos1 = XMLoadFloat3(&Position);
		XMVECTOR rot1 = XMLoadFloat3(&Rotation);
		XMVECTOR quat1 = XMLoadFloat4(&Quaternion);
		XMVECTOR scale1 = XMLoadFloat3(&Scale);

		XMVECTOR pos2 = XMLoadFloat3(&other.Position);
		XMVECTOR rot2 = XMLoadFloat3(&other.Rotation);
		XMVECTOR quat2 = XMLoadFloat4(&other.Quaternion);
		XMVECTOR scale2 = XMLoadFloat3(&other.Scale);

		XMFLOAT3 newPos, newRot, newScale;
		XMFLOAT4 newQuat;

		XMStoreFloat3(&newPos, XMVectorAdd(pos1, pos2)); // 位置は加算
		XMStoreFloat3(&newRot, XMVectorAdd(rot1, rot2)); // オイラー角は加算 (非推奨だが実装を維持)
		XMStoreFloat4(&newQuat, XMQuaternionMultiply(quat1, quat2)); // クォータニオンは乗算で結合
		XMStoreFloat3(&newScale, XMVectorAdd(scale1, quat2)); // スケールは加算

		return Transform{ newPos, newRot, newQuat, newScale };
	}

	inline Transform operator-(const Transform& other) const
	{
		using namespace DirectX;
		XMVECTOR pos1 = XMLoadFloat3(&Position);
		XMVECTOR rot1 = XMLoadFloat3(&Rotation);
		XMVECTOR quat1 = XMLoadFloat4(&Quaternion);
		XMVECTOR scale1 = XMLoadFloat3(&Scale);

		XMVECTOR pos2 = XMLoadFloat3(&other.Position);
		XMVECTOR rot2 = XMLoadFloat3(&other.Rotation);
		XMVECTOR quat2 = XMLoadFloat4(&other.Quaternion);
		XMVECTOR scale2 = XMLoadFloat3(&other.Scale);

		XMFLOAT3 newPos, newRot, newScale;
		XMFLOAT4 newQuat;

		XMStoreFloat3(&newPos, XMVectorSubtract(pos1, pos2));
		XMStoreFloat3(&newRot, XMVectorSubtract(rot1, rot2));
		// クォータニオンの減算は無意味なため、ここでは単純に成分減算を維持 (ただし非推奨)
		XMStoreFloat4(&newQuat, XMVectorSubtract(quat1, quat2));
		XMStoreFloat3(&newScale, XMVectorSubtract(scale1, scale2));

		return Transform{ newPos, newRot, newQuat, newScale };
	}

	inline Transform operator*(const Transform& other) const
	{
		using namespace DirectX;
		XMVECTOR pos1 = XMLoadFloat3(&Position);
		XMVECTOR rot1 = XMLoadFloat3(&Rotation);
		XMVECTOR quat1 = XMLoadFloat4(&Quaternion);
		XMVECTOR scale1 = XMLoadFloat3(&Scale);

		XMVECTOR pos2 = XMLoadFloat3(&other.Position);
		XMVECTOR rot2 = XMLoadFloat3(&other.Rotation);
		XMVECTOR quat2 = XMLoadFloat4(&other.Quaternion);
		XMVECTOR scale2 = XMLoadFloat3(&other.Scale);

		XMFLOAT3 newPos, newRot, newScale;
		XMFLOAT4 newQuat;

		// 位置とスケールは乗算で結合 (これはベクトル成分の乗算として解釈)
		XMStoreFloat3(&newPos, XMVectorMultiply(pos1, pos2));
		XMStoreFloat3(&newRot, XMVectorMultiply(rot1, rot2));
		XMStoreFloat3(&newScale, XMVectorMultiply(scale1, scale2));

		// クォータニオンは乗算で回転を結合 (これは正しい)
		XMStoreFloat4(&newQuat, XMQuaternionMultiply(quat1, quat2));

		return Transform{ newPos, newRot, newQuat, newScale };
	}

	inline Transform operator*(const float scalar) const
	{
		using namespace DirectX;
		XMVECTOR pos = XMLoadFloat3(&Position);
		XMVECTOR rot = XMLoadFloat3(&Rotation);
		XMVECTOR quat = XMLoadFloat4(&Quaternion);
		XMVECTOR scale = XMLoadFloat3(&Scale);
		XMVECTOR scalarVec = XMVectorReplicate(scalar);

		XMFLOAT3 newPos, newRot, newScale;
		XMFLOAT4 newQuat;

		XMStoreFloat3(&newPos, XMVectorMultiply(pos, scalarVec));
		XMStoreFloat3(&newRot, XMVectorMultiply(rot, scalarVec));
		XMStoreFloat4(&newQuat, XMVectorMultiply(quat, scalarVec));
		XMStoreFloat3(&newScale, XMVectorMultiply(scale, scalarVec));

		return Transform{ newPos, newRot, newQuat, newScale };
	}

	inline Transform operator/(const float scalar) const
	{
		using namespace DirectX;
		XMVECTOR pos = XMLoadFloat3(&Position);
		XMVECTOR rot = XMLoadFloat3(&Rotation);
		XMVECTOR quat = XMLoadFloat4(&Quaternion);
		XMVECTOR scale = XMLoadFloat3(&Scale);
		XMVECTOR scalarVec = XMVectorReplicate(scalar);

		XMFLOAT3 newPos, newRot, newScale;
		XMFLOAT4 newQuat;

		XMStoreFloat3(&newPos, XMVectorDivide(pos, scalarVec));
		XMStoreFloat3(&newRot, XMVectorDivide(rot, scalarVec));
		XMStoreFloat4(&newQuat, XMVectorDivide(quat, scalarVec));
		XMStoreFloat3(&newScale, XMVectorDivide(scale, scalarVec));

		return Transform{ newPos, newRot, newQuat, newScale };
	}

	inline Transform& operator+=(const Transform& other)
	{
		using namespace DirectX;
		XMVECTOR pos1 = XMLoadFloat3(&Position);
		XMVECTOR rot1 = XMLoadFloat3(&Rotation);
		XMVECTOR quat1 = XMLoadFloat4(&Quaternion);
		XMVECTOR scale1 = XMLoadFloat3(&Scale);

		XMVECTOR pos2 = XMLoadFloat3(&other.Position);
		XMVECTOR rot2 = XMLoadFloat3(&other.Rotation);
		XMVECTOR quat2 = XMLoadFloat4(&other.Quaternion);
		XMVECTOR scale2 = XMLoadFloat3(&other.Scale);

		// 位置、回転(オイラー角)、スケールは加算
		XMStoreFloat3(&Position, XMVectorAdd(pos1, pos2));
		XMStoreFloat3(&Rotation, XMVectorAdd(rot1, rot2));
		XMStoreFloat3(&Scale, XMVectorAdd(scale1, scale2));

		// クォータニオンは乗算 (回転の結合)
		XMStoreFloat4(&Quaternion, XMQuaternionMultiply(quat1, quat2));

		// オイラー角をクォータニオンから再同期する（オプションだが推奨）
		// UpdateRotationFromQuaternion();

		return *this;
	}

	inline Transform& operator-=(const Transform& other)
	{
		using namespace DirectX;
		XMVECTOR pos1 = XMLoadFloat3(&Position);
		XMVECTOR rot1 = XMLoadFloat3(&Rotation);
		XMVECTOR quat1 = XMLoadFloat4(&Quaternion);
		XMVECTOR scale1 = XMLoadFloat3(&Scale);

		XMVECTOR pos2 = XMLoadFloat3(&other.Position);
		XMVECTOR rot2 = XMLoadFloat3(&other.Rotation);
		XMVECTOR quat2 = XMLoadFloat4(&other.Quaternion);
		XMVECTOR scale2 = XMLoadFloat3(&other.Scale);

		XMStoreFloat3(&Position, XMVectorSubtract(pos1, pos2));
		XMStoreFloat3(&Rotation, XMVectorSubtract(rot1, rot2));
		XMStoreFloat4(&Quaternion, XMVectorSubtract(quat1, quat2)); // (非推奨だが実装を維持)
		XMStoreFloat3(&Scale, XMVectorSubtract(scale1, scale2));

		return *this;
	}

	inline Transform& operator*=(const float scalar)
	{
		using namespace DirectX;
		XMVECTOR pos = XMLoadFloat3(&Position);
		XMVECTOR rot = XMLoadFloat3(&Rotation);
		XMVECTOR quat = XMLoadFloat4(&Quaternion);
		XMVECTOR scale = XMLoadFloat3(&Scale);
		XMVECTOR scalarVec = XMVectorReplicate(scalar);

		XMStoreFloat3(&Position, XMVectorMultiply(pos, scalarVec));
		XMStoreFloat3(&Rotation, XMVectorMultiply(rot, scalarVec));
		XMStoreFloat4(&Quaternion, XMVectorMultiply(quat, scalarVec));
		XMStoreFloat3(&Scale, XMVectorMultiply(scale, scalarVec));

		return *this;
	}

	inline Transform& operator/=(const float scalar)
	{
		using namespace DirectX;
		XMVECTOR pos = XMLoadFloat3(&Position);
		XMVECTOR rot = XMLoadFloat3(&Rotation);
		XMVECTOR quat = XMLoadFloat4(&Quaternion);
		XMVECTOR scale = XMLoadFloat3(&Scale);
		XMVECTOR scalarVec = XMVectorReplicate(scalar);

		XMStoreFloat3(&Position, XMVectorDivide(pos, scalarVec));
		XMStoreFloat3(&Rotation, XMVectorDivide(rot, scalarVec));
		XMStoreFloat4(&Quaternion, XMVectorDivide(quat, scalarVec));
		XMStoreFloat3(&Scale, XMVectorDivide(scale, scalarVec));

		return *this;
	}

	//---------------------------------------------------------------------------------
	// 比較演算子オーバーロードの実装
	//---------------------------------------------------------------------------------

	inline bool operator==(const Transform& other) const
	{
		using namespace DirectX;
		return XMVector3Equal(XMLoadFloat3(&Position), XMLoadFloat3(&other.Position)) &&
			XMVector3Equal(XMLoadFloat3(&Rotation), XMLoadFloat3(&other.Rotation)) &&
			XMVector4Equal(XMLoadFloat4(&Quaternion), XMLoadFloat4(&other.Quaternion)) &&
			XMVector3Equal(XMLoadFloat3(&Scale), XMLoadFloat3(&other.Scale));
	}

	inline bool operator!=(const Transform& other) const
	{
		return !(*this == other);
	}


#if _DEBUG
	// デバッグ用に内容を文字列化.
	std::string ToString() const;
#endif
};