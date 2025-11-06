#pragma once

#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <D3D11.h>
#include <D3DX11async.h> // D3DX11CompileFromFile のため
#include <cassert>

#include "Game/03_Collision/ColliderBase.h"

class DirectX11;
class CameraManager;



// インスタンスデータ.
struct DebugColliderInfo
{
    DirectX::XMFLOAT4X4      WorldMatrix;   
    DirectX::XMFLOAT4        Color;      
    ColliderBase::eShapeType ShapeType;     
    float                    Padding[3];    
    DirectX::XMFLOAT4        Data0;     
    DirectX::XMFLOAT4        Data1;         

    DebugColliderInfo()
        : ShapeType{}
        , Padding{}
        , WorldMatrix{}
        , Color{}
        , Data0{}
        , Data1{}
    {
    }
};


/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/11/4.
* @brief     : 当たり判定描画クラス, インスタンス描画により軽量化.
* @pattern   : Singleton.
**********************************************************************************/

class CollisionVisualizer
    : public Singleton<CollisionVisualizer>
{
public:
    friend Singleton<CollisionVisualizer>;
private:

    // 形状ごとのメッシュデータをまとめる構造体
    struct ShapeData
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
        std::vector<WORD> Indices; // DrawIndexedInstanced のサイズ決定用
    };

public:

    CollisionVisualizer();
    ~CollisionVisualizer();

    void RegisterCollider(const DebugColliderInfo& info);
    void Draw();
private:
    struct SimpleVertex { DirectX::XMFLOAT3 Position; }; // 頂点構造体

    // CBuffer構造体 (HLSLの ViewProjCBuffer に合わせ、ViewProj行列のみ)
    struct CBuffer
    {
        DirectX::XMMATRIX ViewProj; // 64 bytes
    };

    void CreateConstantBuffer();
    void CreateShader();
    void CreateD3D11Buffer(
        const std::vector<SimpleVertex>& vertices,
        const std::vector<WORD>& indices,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& vb_out,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& ib_out);

    // 形状ごとのリソース作成関数
    void CreateBoxResources(ShapeData& out_data);
    void CreateSphereResources(ShapeData& out_data, int segments = 16);
    void CreateCapsuleResources(ShapeData& out_data, float halfHeight = 0.5f, int segments = 16);

private:
    std::vector<DebugColliderInfo> m_DebugInfoQueue;

    // 形状タイプ（eShapeType）をキーにして、リソースを保持するマップ
    std::map<ColliderBase::eShapeType, ShapeData> m_ShapeResources;

    // 共通リソース
    Microsoft::WRL::ComPtr<ID3D11Buffer>    m_ConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  m_VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_PixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   m_InputLayout;

    // **インスタンス描画用**
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_InstanceBuffer;
    size_t m_InstanceBufferSize = 0; // 現在確保されているインスタンスバッファのバイトサイズ
};