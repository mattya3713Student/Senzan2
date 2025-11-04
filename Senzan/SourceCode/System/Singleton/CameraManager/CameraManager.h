#pragma once
#include "System/Singleton/SingletonTemplate.h"

class CameraBase;


/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/11/4.
* @brief     : カメラ管理クラス.
* @patarm    : Singleton.
**********************************************************************************/

class CameraManager final
	: public Singleton<CameraManager>
{
private:
	friend class Singleton<CameraManager>;
	CameraManager();
public:
	~CameraManager();
	void Update();

	// カメラの設定.
	inline void SetCamera(std::shared_ptr<CameraBase> spCamera) noexcept { m_wpCamera = spCamera; };

    // 座標の取得.
    inline const DirectX::XMFLOAT3 GetPosition() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetPosition();
        }
        return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    }

    // 座標の設定.
    inline void SetPosition(DirectX::XMFLOAT3 NewPosition) noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            spCamera->SetPosition(NewPosition);
        } 
    }
    inline void SetPositionOffset(float NewPosition_x, float NewPosition_y, float NewPosition_z) noexcept   {
        if (auto spCamera = m_wpCamera.lock()) {
            spCamera->SetPosition(DirectX::XMFLOAT3(NewPosition_x, NewPosition_y, NewPosition_z));
        }
    }

    // カメラの設定.
    inline void SetCamera(std::shared_ptr<CameraBase> spCamera) noexcept {
        m_wpCamera = spCamera; 
    }

    // 注視点を設定.
    inline void SetLook(const DirectX::XMFLOAT3& look) noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            spCamera->SetLook(look);
        }
    }

    // 注視点を取得.
    inline DirectX::XMFLOAT3 GetLook() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetLook();
        }
        return DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f); 
    }

    // 視線の方向を取得.
    inline const DirectX::XMFLOAT3 GetLookDirection() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetLookDirection();
        }
        return DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    }

    // 現在のカメラを取得
    inline std::shared_ptr<CameraBase> GetCurrentCamera() const noexcept { return m_wpCamera.lock(); }

    // ビュー・プロジェクションの合成行列を取得.
    inline DirectX::XMMATRIX GetViewProjMatrix() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetViewProjMatrix();
        }
        return DirectX::XMMatrixIdentity(); 
    }

    // 前方向ベクトルを取得.
    inline DirectX::XMFLOAT3 GetForwardVec() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetForwardVec();
        }
        return DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f); 
    }

    // 右方向ベクトルを取得.
    inline DirectX::XMFLOAT3 GetRightVec() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetRightVec();
        }
        return DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    }

    // Yawを取得.
    inline float GetYaw() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetYaw();
        }
        return 0.0f;
    }

    // Pitchを取得.
    inline float GetPitch() const noexcept {
        if (auto spCamera = m_wpCamera.lock()) {
            return spCamera->GetPitch();
        }
        return 0.0f; 
    }
private:
	void ViewAndProjectionUpdate();

private:
	std::weak_ptr<CameraBase> m_wpCamera;
};