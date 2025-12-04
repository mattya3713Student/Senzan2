#pragma once
#include "Constant.h"
#include <cmath>
#include <DirectXMath.h>

namespace MyMath {

	// floatなどの!=を近似値比較する.
	template <typename T>
	inline bool IsNearlyEqual(T a, T b, T epsilon = EPSILON_E4) noexcept
	{
		// aとbの差の絶対値が、許容誤差以内であればtrue.
		return std::abs(a - b) <= epsilon;
	}

	// 2Dベクトルがゼロに近いかをチェック.
	inline bool IsVector2NearlyZero(const DirectX::XMFLOAT2& a, float b)
	{
		return IsNearlyEqual(a.x, b) && IsNearlyEqual(a.y, b);
	}

	// 3Dベクトルがゼロに近いかをチェック.
	inline bool IsVector3NearlyZero(const DirectX::XMFLOAT3& a, float b)
	{
		return IsNearlyEqual(a.x, b) && IsNearlyEqual(a.y, b) && IsNearlyEqual(a.z, b);
	}

	inline float NormalizeAngleDegrees(float angle);

	/*************************************************************
	* @brief	値を減少(増加)させる.
	* @return	中央値になったかどうか.	
	* @param[inout] Value	：増減させたい値.
	* @param[in]	Median	：中央値.
	* @param[in]	Amount	：増減させる量.
	* ************************************************************/
	template<typename T>
	inline bool DecreaseToValue(T& Value, T Median, T Amount) noexcept
	{
		if (std::abs(Value - Median) <= Amount)
		{
			Value = Median;
			return true;
		}

		if (Value >= Median)
		{
			Value -= Amount;
		}
		else
		{
			Value += Amount;
		}
		return false;
	}

	/*************************************************************
	* @brief ワールドをスクリーン座標に変換して返す.
	* @param[in]	worldPos	：ワールド座標.
	* ************************************************************/
	const DirectX::XMFLOAT2 ConvertWorldPosToScreenPos(
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj,
		const DirectX::XMFLOAT3& worldPos);

	/*************************************************************
	* @brief スクリーン座標をワールド座標に変換して返す.
	* @param[in]	screenPos	：スクリーン座標.
	* ************************************************************/
	const DirectX::XMFLOAT3 ConvertScreenPosToWorldPos(
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj,
		const DirectX::XMFLOAT3& screenPos);

	/*************************************************************
	* @brief 目標の座標に向かって回転する値を計算して返す.
	* @param[in]	currentPosition	：現在座標.
	* @param[in]	targetPosition	：目標座標.
	* @param[out]	outQuaternion	：現在のクォータニオン.
	* @param[in]	speed			：補間速度.
	*************************************************************/
	void CalcLookAtRotation(
		const DirectX::XMFLOAT3& currentPosition,
		const DirectX::XMFLOAT3& targetPosition,
		DirectX::XMFLOAT4& outQuaternion,
		const float& speed = 0.1f);

	/*************************************************************
	* @brief 目標の方向に向かって回転するクォータニオンを計算する.
	* @param[in]	targetDirection	：目標の方向.
	* @param[out]	outQuaternion	：現在のクォータニオン.
	* @param[in]	speed			：補間速度.
	*************************************************************/
	void CalcLookAtRotation(
		const DirectX::XMFLOAT3& targetDirection,
		DirectX::XMFLOAT4& outQuaternion,
		const float& speed = 0.1f);
}
#include "Math.inl"