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

    // 形状ごとのメッシュデータをまとめる構造体.
    struct ShapeData
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
        std::vector<WORD> Indices;
    };

public:

    CollisionVisualizer();
    ~CollisionVisualizer();

    // 描画する当たり判定の登録(毎フレーム実行).
    void RegisterCollider(const DebugColliderInfo& info);
    void Draw();
private:
    struct SimpleVertex { DirectX::XMFLOAT3 Position; };    // 頂点.
    struct CBuffer {DirectX::XMMATRIX ViewProj; };          // コンスタントバッファ.

    // コンスタントバッファの作成.
    void CreateConstantBuffer();
    // シェーダーの作成.
    void CreateShader();
    // バッファの作成.
    void CreateD3D11Buffer(
        const std::vector<SimpleVertex>& Vertices,
        const std::vector<WORD>& Indices,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& oVertexBuffer,
        Microsoft::WRL::ComPtr<ID3D11Buffer>& oIndexBuffer);

    // 形状ごとの単位形状リソース作成関数.
    void CreateBoxResources(ShapeData& out_data);
    void CreateSphereResources(ShapeData& out_data, int Segments = 16);
    void CreateCapsuleResources(ShapeData& out_data, float HalfHeight = 0.5f, int Segments = 16);

private:
    // このフレームで描画するリスト.
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