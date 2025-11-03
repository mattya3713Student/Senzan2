#include "Transform.h"
#include "System/Utility/Math/Math.h"	

using namespace DirectX;

//---------------------------------------------------------------------------------
// ヘルパーメソッドの実装
//---------------------------------------------------------------------------------

void Transform::SetPosition(const DirectX::XMFLOAT3& newPosition)
{
	Position = newPosition;
}

void Transform::SetRotation(const DirectX::XMFLOAT3& eulerAngles)
{
	Rotation = eulerAngles;
	UpdateQuaternionFromRotation();
}

// 角度（度）で回転を設定.
void Transform::SetRotationDegrees(const DirectX::XMFLOAT3& eulerAnglesInDegrees)
{
	SetRotation(DirectX::XMFLOAT3(
		DirectX::XMConvertToDegrees(eulerAnglesInDegrees.x),
		DirectX::XMConvertToDegrees(eulerAnglesInDegrees.y),
		DirectX::XMConvertToDegrees(eulerAnglesInDegrees.z)
	));
}

void Transform::SetQuaternion(const DirectX::XMFLOAT4& newQuaternion)
{
	Quaternion = newQuaternion;
	UpdateRotationFromQuaternion();
}

void Transform::SetScale(const DirectX::XMFLOAT3& newScale)
{
	Scale = newScale;
}

void Transform::Translate(const DirectX::XMFLOAT3& translation)
{
	XMVECTOR pos = XMLoadFloat3(&Position);
	XMVECTOR trans = XMLoadFloat3(&translation);
	XMStoreFloat3(&Position, XMVectorAdd(pos, trans));
}

void Transform::Rotate(const DirectX::XMFLOAT3& eulerAngles)
{
	XMVECTOR currentRot = XMLoadFloat3(&Rotation);
	XMVECTOR newRot = XMLoadFloat3(&eulerAngles);
	XMStoreFloat3(&Rotation, XMVectorAdd(currentRot, newRot));
	UpdateQuaternionFromRotation();
}

void Transform::RotateDegrees(const DirectX::XMFLOAT3& eulerAnglesInDegrees)
{
	Rotate(DirectX::XMFLOAT3(
		DirectX::XMConvertToDegrees(eulerAnglesInDegrees.x),
		DirectX::XMConvertToDegrees(eulerAnglesInDegrees.y),
		DirectX::XMConvertToDegrees(eulerAnglesInDegrees.z)
	));
}

void Transform::Rotate(const DirectX::XMFLOAT4& quaternion)
{
	XMVECTOR currentQuat = XMLoadFloat4(&Quaternion);
	XMVECTOR newQuat = XMLoadFloat4(&quaternion);
	XMStoreFloat4(&Quaternion, XMQuaternionMultiply(currentQuat, newQuat));
	UpdateRotationFromQuaternion();
}

// ローカル座標系の前方ベクトルを取得.
DirectX::XMFLOAT3 Transform::GetForward() const
{
	// ローカル前方ベクトル(0, 0, 1)をクォータニオンで回転.
	XMVECTOR localForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR rotatedVector = XMVector3Rotate(localForward, XMLoadFloat4(&Quaternion));
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, rotatedVector);
	return forward;
}

// ローカル座標系の上方ベクトルを取得.
DirectX::XMFLOAT3 Transform::GetUp() const
{
	// ローカル上方ベクトル(0, 1, 0)をクォータニオンで回転.
	XMVECTOR localUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotatedVector = XMVector3Rotate(localUp, XMLoadFloat4(&Quaternion));
	XMFLOAT3 up;
	XMStoreFloat3(&up, rotatedVector);
	return up;
}

// ローカル座標系の右方ベクトルを取得.
DirectX::XMFLOAT3 Transform::GetRight() const
{
	// ローカル右方ベクトル(1, 0, 0)をクォータニオンで回転.
	XMVECTOR localRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotatedVector = XMVector3Rotate(localRight, XMLoadFloat4(&Quaternion));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rotatedVector);
	return right;
}

std::array<DirectX::XMFLOAT3, 3>Transform::GetAxisVector() const
{
	std::array<DirectX::XMFLOAT3, 3> axes;
	axes[0] = GetRight();
	axes[1] = GetUp();
	axes[2] = GetForward();
	return axes;
}

// 角度（度）でオイラー角を取得.
DirectX::XMFLOAT3 Transform::GetRotationDegrees() const
{
	return DirectX::XMFLOAT3(
		DirectX::XMConvertToDegrees(Rotation.x),
		DirectX::XMConvertToDegrees(Rotation.y),
		DirectX::XMConvertToDegrees(Rotation.z)
	);
}

// クォータニオンからオイラー角へ変換(ジンバルロックに注意).
void Transform::UpdateRotationFromQuaternion()
{
	XMVECTOR quat = XMLoadFloat4(&Quaternion);

	// 正規化されていないクォータニオンの場合、0除算を防ぐ.
	// 浮動小数点誤差を防ぐためにクォータニオンを正規化.
	quat = XMQuaternionNormalize(quat);

	XMFLOAT4 qf;
	XMStoreFloat4(&qf, quat);

	float roll, pitch, yaw;
	// 2* (qy*qz + qw*qx)
	float test = qf.y * qf.z + qf.w * qf.x;

	if (test > 0.499f) // 北極での特異点 (y=90°)
	{
		roll = 2.0f * atan2f(qf.y, qf.w); // roll (z) の計算
		pitch = XM_PIDIV2; // pitch (x) は π/2
		yaw = 0.0f; // yaw (y) は 0
	}
	else if (test < -0.499f) // 南極での特異点 (y=-90°)
	{
		roll = -2.0f * atan2f(qf.y, qf.w); // roll (z) の計算
		pitch = -XM_PIDIV2; // pitch (x) は -π/2
		yaw = 0.0f; // yaw (y) は 0
	}
	else
	{
		float sqx = qf.x * qf.x;
		float sqy = qf.y * qf.y;
		float sqz = qf.z * qf.z;

		// roll (z)
		roll = atan2f(2.0f * qf.w * qf.z - 2.0f * qf.x * qf.y, 1.0f - 2.0f * sqz - 2.0f * sqx);
		// pitch (x)
		pitch = asinf(2.0f * test);
		// yaw (y)
		yaw = atan2f(2.0f * qf.w * qf.y - 2.0f * qf.x * qf.z, 1.0f - 2.0f * sqy - 2.0f * sqx);
	}

	// 注意: オイラー角の順序が PITCH(X) -> YAW(Y) -> ROLL(Z) の順であると仮定しています。
	Rotation = XMFLOAT3(pitch, yaw, roll);
}

// オイラー角からクォータニオンへ変換.
void Transform::UpdateQuaternionFromRotation()
{
	XMVECTOR rot = XMLoadFloat3(&Rotation);
	XMStoreFloat4(&Quaternion, XMQuaternionRotationRollPitchYawFromVector(rot));
}

// デバッグ用 ToString
#if _DEBUG
std::string Transform::ToString() const
{
	std::stringstream ss;
	ss << "Pos: (" << Position.x << ", " << Position.y << ", " << Position.z << ") "
		<< "Rot(Rad): (" << Rotation.x << ", " << Rotation.y << ", " << Rotation.z << ") "
		<< "Quat: (" << Quaternion.x << ", " << Quaternion.y << ", " << Quaternion.z << ", " << Quaternion.w << ") "
		<< "Scale: (" << Scale.x << ", " << Scale.y << ", " << Scale.z << ")";
	return ss.str();
}
#endif