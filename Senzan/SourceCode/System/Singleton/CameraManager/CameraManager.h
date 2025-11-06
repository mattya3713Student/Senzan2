#pragma once
#include "System/Singleton/SingletonTemplate.h"

class CameraBase;

/************************************
*   カメラ管理クラス.
************************************/
class CameraManager final
	: public Singleton<CameraManager>
{
private:
	friend class Singleton<CameraManager>;
	CameraManager();
public:
	~CameraManager();

	/****************************************************
	* @brief カメラの接続.
	* @param pCamera：使用するカメラの共有ポインタ.
	****************************************************/
	static void SetCamera(std::shared_ptr<CameraBase> pCamera);


	/****************************************************
	* @brief カメラの更新.
	****************************************************/
	static void LateUpdate();


	/****************************************************
	* @brief ビューとプロジェクションの更新.
	****************************************************/
	static void ViewAndProjectionUpdate();


	/****************************************************
	* @brief 指定座標が視錐体内にあるか.
	* @param point：判定したい座標.
	****************************************************/
	static bool IsPointInFrustum(const DirectX::XMFLOAT3& point);

	static bool VFCulling(const DirectX::XMFLOAT3& Position, const float radius);

	/****************************************************
	* @brief 視点を設定.
	****************************************************/
	static void SetPosition(const DirectX::XMFLOAT3& Position);

	/****************************************************
	* @brief 視点を取得.
	****************************************************/
	static DirectX::XMFLOAT3 GetPosition();


	/****************************************************
	* @brief  注視点を設定.
	****************************************************/
	static void SetLook(const DirectX::XMFLOAT3& look);

	/****************************************************
	* @brief  注視点を取得.
	****************************************************/
	static DirectX::XMFLOAT3 GetLook();

	/****************************************************
	* @brief  視線の方向を取得.
	****************************************************/
	static const DirectX::XMFLOAT3 GetLookDirection();


	/****************************************************
	* @brief  現在のを取得
	****************************************************/
	static std::shared_ptr<CameraBase> GetCurrentCamera();

	/****************************************************
	* @brief  ビュー行列を取得
	****************************************************/
	static DirectX::XMMATRIX GetViewMatrix();

	/****************************************************
	* @brief プロジェクション行列を取得.
	****************************************************/
	static DirectX::XMMATRIX GetProjMatrix();

	/****************************************************
	* @brief ビュー・プロジェクションの合成行列を取得.
	****************************************************/
	static DirectX::XMMATRIX GetViewProjMatrix();


	/****************************************************
	* @brief 前方向ベクトルを取得.
	****************************************************/
	static DirectX::XMFLOAT3 GetForwardVec();

	/****************************************************
	* @breif 右方向ベクトルを取得.
	****************************************************/
	static DirectX::XMFLOAT3 GetRightVec();

	/****************************************************
	* @breif Yawを取得.
	****************************************************/
	static float GetYaw();

	/****************************************************
	* @breif Pitchを取得.
	****************************************************/
	static float GetPitch();
private:
	std::weak_ptr<CameraBase> m_Camera;
};