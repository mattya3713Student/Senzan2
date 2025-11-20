#include "CollisionVisualizer.h"       
#include "Graphic/DirectX/DirectX11/DirectX11.h"       
#include "System/Singleton/CameraManager/CameraManager.h"       

// TODO : リファタリング前.


CollisionVisualizer::CollisionVisualizer()
    : m_DebugInfoQueue()
{
    /*try
        catch()*/
    // 共通リソースの作成.
    CreateShader();
    CreateConstantBuffer();

    // 形状ごとのメッシュリソースの作成と登録.
    CreateBoxResources(m_ShapeResources[ColliderBase::eShapeType::Box]);
    CreateSphereResources(m_ShapeResources[ColliderBase::eShapeType::Sphere], 12);
    CreateCapsuleResources(m_ShapeResources[ColliderBase::eShapeType::Capsule], 1.0f, 12);

    assert(!m_ShapeResources.empty() && "デバッグ描画リソースが作成されていません。");
}

CollisionVisualizer::~CollisionVisualizer()
{
}

// 描画する当たり判定の登録(毎フレーム登録).
void CollisionVisualizer::RegisterCollider(const DebugColliderInfo& info)
{
    m_DebugInfoQueue.push_back(info);
}

void CollisionVisualizer::CreateConstantBuffer()
{
    ID3D11Device* pDevice = DirectX11::GetInstance().GetDevice();
    HRESULT hr;

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = sizeof(CBuffer); 
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;

    hr = pDevice->CreateBuffer(&desc, nullptr, m_ConstantBuffer.GetAddressOf());
    if (FAILED(hr)) { assert(0 && "コンスタントバッファの作成失敗"); }
}

void CollisionVisualizer::CreateShader()
{
    ID3D11Device* pDevice = DirectX11::GetInstance().GetDevice();
    Microsoft::WRL::ComPtr<ID3DBlob> compiledShader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;
    UINT uCompileFlag = 0;
#ifdef _DEBUG
    uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
    HRESULT hr;

    MyAssert::IsFailed(
        _T("頂点シェーダーの読み込み."),
        &D3DX11CompileFromFileA,
        _T("Data\\Shader\\Wire\\WireVS.hlsl"),
        nullptr,
        nullptr,
        "VS_Main",
        "vs_5_0",
        uCompileFlag,
        0,
        nullptr,
        compiledShader.GetAddressOf(),
        errors.GetAddressOf(),
        nullptr);

    // HLSLの VS_INPUT_VERTEX (スロット0) と VS_INPUT_INSTANCE (スロット1) に対応
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
    {
        // スロット0: 頂点データ.
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

        // 転倒させる為,行ごとに送る.
        // World Matrix (Offset 0 - 64).
        {"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}, 

        // Color (Offset 64).
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1}, 

        // ShapeType + Padding (Offset 80).
        {"SHAPETYPE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1}, 

        // Data0 (Offset 96).
        {"DATA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1}, 

        // Data1 (Offset 112).
        {"DATA", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    };

    pDevice->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf());

    // インプットレイアウトの作成 (失敗チェック推奨)
    pDevice->CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()), compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), m_InputLayout.GetAddressOf());


    // ピクセルシェーダー
    hr = D3DX11CompileFromFile(_T("Data\\Shader\\Wire\\WirePS.hlsl"), nullptr, nullptr, "PS_Main", "ps_5_0", uCompileFlag, 0, nullptr, compiledShader.ReleaseAndGetAddressOf(), errors.ReleaseAndGetAddressOf(), nullptr);
    if (FAILED(hr) && errors) {
        // OutputDebugStringA((char*)errors->GetBufferPointer());
        assert(0 && "ピクセルシェーダーのコンパイル失敗");
        return;
    }

    pDevice->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf());
}

void CollisionVisualizer::CreateD3D11Buffer(
    const std::vector<SimpleVertex>& vertices,
    const std::vector<WORD>& indices,
    Microsoft::WRL::ComPtr<ID3D11Buffer>& vb_out,
    Microsoft::WRL::ComPtr<ID3D11Buffer>& ib_out)
{
    ID3D11Device* pDevice = DirectX11::GetInstance().GetDevice();
    HRESULT hr;

    // 1. 頂点バッファの作成
    D3D11_BUFFER_DESC vb_desc = {};
    vb_desc.Usage = D3D11_USAGE_DEFAULT;
    vb_desc.ByteWidth = sizeof(SimpleVertex) * static_cast<UINT>(vertices.size());
    vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vb_initData = { vertices.data() };
    hr = pDevice->CreateBuffer(&vb_desc, &vb_initData, vb_out.GetAddressOf());
    if (FAILED(hr)) { assert(0 && "頂点バッファの作成失敗"); return; }

    // 2. インデックスバッファの作成
    D3D11_BUFFER_DESC ib_desc = {};
    ib_desc.Usage = D3D11_USAGE_DEFAULT;
    ib_desc.ByteWidth = sizeof(WORD) * static_cast<UINT>(indices.size());
    ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ib_initData = { indices.data() };
    hr = pDevice->CreateBuffer(&ib_desc, &ib_initData, ib_out.GetAddressOf());
    if (FAILED(hr)) { assert(0 && "インデックスバッファの作成失敗"); }
}

void CollisionVisualizer::Draw()
{
    if (m_DebugInfoQueue.empty()) { return; }

    DirectX11& pDirectX11 = DirectX11::GetInstance();
    ID3D11DeviceContext* pDeviceContext = pDirectX11.GetContext();
    CameraManager& pCamera = CameraManager::GetInstance();

    const size_t instanceCount = m_DebugInfoQueue.size();
    const size_t requiredSize = instanceCount * sizeof(DebugColliderInfo);

    // ---------------------------------------------
    // 1. インスタンスバッファの作成/リサイズと更新
    // ---------------------------------------------
    // バッファが未作成、またはサイズが不足している場合は再作成
    if (!m_InstanceBuffer || requiredSize > m_InstanceBufferSize)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.ByteWidth = static_cast<UINT>(requiredSize);
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_DYNAMIC;

        HRESULT hr = pDirectX11.GetDevice()->CreateBuffer(&desc, nullptr, m_InstanceBuffer.ReleaseAndGetAddressOf());
        if (FAILED(hr)) { assert(0 && "インスタンスバッファの作成失敗"); m_DebugInfoQueue.clear(); return; }
        m_InstanceBufferSize = requiredSize;
    }

    // インスタンスデータをバッファにマップして書き込み
    D3D11_MAPPED_SUBRESOURCE pData;
    if (SUCCEEDED(pDeviceContext->Map(
        m_InstanceBuffer.Get(), 0, 
        D3D11_MAP_WRITE_DISCARD, 0, &pData)))
    {
        memcpy_s(pData.pData, pData.RowPitch, m_DebugInfoQueue.data(), requiredSize);
        pDeviceContext->Unmap(m_InstanceBuffer.Get(), 0);
    }
    else { m_DebugInfoQueue.clear(); return; }

    // ---------------------------------------------
    // 2. CBufferの更新とセット (ViewProj行列のみ)
    // ---------------------------------------------
    if (SUCCEEDED(pDeviceContext->Map(m_ConstantBuffer.Get(),
        0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
    {
        CBuffer cb;
        DirectX::XMMATRIX mView = pCamera.GetViewMatrix();
        DirectX::XMMATRIX mProj = pCamera.GetProjMatrix();

        // View * Proj を計算
        DirectX::XMMATRIX mViewProj_temp = DirectX::XMMatrixMultiply(mView, mProj);

        // 成功コードに合わせて、C++の行優先行列を転置して列優先にする
        cb.ViewProj = DirectX::XMMatrixTranspose(mViewProj_temp);

        // 2. メモリへのコピー
        // pData.pData には cb の中身（転置済みの ViewProj 行列）をコピーします。
        // RowPitchはバッファ全体の行ピッチ（通常はバイト幅）ですが、コピーサイズが合っていれば問題ありません。
        memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(CBuffer));

        pDeviceContext->Unmap(m_ConstantBuffer.Get(), 0);
    }
    pDeviceContext->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());
    pDeviceContext->PSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());


    // ---------------------------------------------
    // 3. 共通レンダリングステートの設定
    // ---------------------------------------------
    pDeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    pDeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
    pDeviceContext->IASetInputLayout(m_InputLayout.Get());
    pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pDirectX11.SetRasterizerState(eRasterizerMode::Wireframe | eRasterizerMode::None); // ワイヤーフレーム設定


    // ---------------------------------------------
    // 4. 描画ループ (DrawIndexedInstanced)
    // ---------------------------------------------

    // 描画効率のため、キューを形状タイプでソート
    std::sort(m_DebugInfoQueue.begin(), m_DebugInfoQueue.end(), [](const auto& a, const auto& b) {
        return a.ShapeType < b.ShapeType;
        });

    size_t instanceStartOffset = 0;

    // 形状リソースマップをループ
    for (const auto& pair : m_ShapeResources)
    {
        const ColliderBase::eShapeType type = pair.first;
        const ShapeData& currentShape = pair.second;

        // 現在の形状タイプに該当するインスタンスの範囲を検索
        auto first = std::find_if(m_DebugInfoQueue.begin() + instanceStartOffset, m_DebugInfoQueue.end(),
            [&type](const DebugColliderInfo& info) { return info.ShapeType == type; });
        auto last = std::find_if_not(first, m_DebugInfoQueue.end(),
            [&type](const DebugColliderInfo& info) { return info.ShapeType == type; });

        const UINT drawInstanceCount = static_cast<UINT>(std::distance(first, last));

        if (drawInstanceCount == 0) continue;

        // 頂点バッファ (スロット0) と インスタンスバッファ (スロット1) を同時にセット
        UINT stride_geom = sizeof(SimpleVertex);
        UINT offset_geom = 0;

        UINT stride_inst = sizeof(DebugColliderInfo);
        UINT offset_inst = static_cast<UINT>(instanceStartOffset * sizeof(DebugColliderInfo));

        ID3D11Buffer* vbs[] = { currentShape.VertexBuffer.Get(), m_InstanceBuffer.Get() };
        UINT strides[] = { stride_geom, stride_inst };
        UINT offsets[] = { offset_geom, offset_inst };

        pDeviceContext->IASetVertexBuffers(
            0, 2, vbs,
            strides, offsets);

        pDeviceContext->IASetIndexBuffer(
            currentShape.IndexBuffer.Get(), 
            DXGI_FORMAT_R16_UINT, 0);

        // DrawIndexedInstanced で一括描画
        pDeviceContext->DrawIndexedInstanced(
            static_cast<UINT>(currentShape.Indices.size()), // IndexCountPerInstance
            drawInstanceCount,                             // InstanceCount
            0,                                             // StartIndexLocation
            0,                                             // BaseVertexLocation
            0);                                            // StartInstanceLocation

        // 次の形状タイプへオフセットを更新
        instanceStartOffset += drawInstanceCount;
    }

    // ---------------------------------------------
    // 5. 描画後のクリーンアップ
    // ---------------------------------------------
    pDirectX11.SetRasterizerState(eRasterizerMode::Solid | eRasterizerMode::Front);
    m_DebugInfoQueue.clear();
}

// ----------------------------------------------------
// 形状リソース作成関数の実装
// ----------------------------------------------------
void CollisionVisualizer::CreateBoxResources(ShapeData& out_data)
{
    // 各辺の半分の長さは 0.5f に固定
    float half_size = 0.5f;

    // 1. 頂点データ (8頂点): BoxCollider::CreateVerticesの順序を適用
    std::vector<SimpleVertex> vertices = {
        // 0: 左上奥
        { DirectX::XMFLOAT3(-half_size,  half_size,  half_size) },
        // 1: 右上奥
        { DirectX::XMFLOAT3(half_size,  half_size,  half_size) },
        // 2: 左下奥
        { DirectX::XMFLOAT3(-half_size, -half_size,  half_size) },
        // 3: 右下奥
        { DirectX::XMFLOAT3(half_size, -half_size,  half_size) },
        // 4: 左上前
        { DirectX::XMFLOAT3(-half_size,  half_size, -half_size) },
        // 5: 右上前
        { DirectX::XMFLOAT3(half_size,  half_size, -half_size) },
        // 6: 左下前
        { DirectX::XMFLOAT3(-half_size, -half_size, -half_size) },
        // 7: 右下前
        { DirectX::XMFLOAT3(half_size, -half_size, -half_size) }
    };

    // 2. インデックスデータ (TRIANGLELISTとして36個のインデックスを生成): BoxCollider::CreateIndicesの順序を適用
    std::vector<WORD> indices = {
        0, 1, 2, 2, 1, 3,
        4, 6, 5, 5, 6, 7,
        4, 5, 0, 0, 5, 1,
        2, 3, 6, 6, 3, 7,
        4, 0, 6, 6, 0, 2,
        1, 5, 3, 3, 5, 7
    };

    out_data.Indices = indices;
    CreateD3D11Buffer(vertices, out_data.Indices, out_data.VertexBuffer, out_data.IndexBuffer);
}

void CollisionVisualizer::CreateSphereResources(ShapeData& out_data, int segments)
{
    // 緯度・経度の分割数を均一にする (segmentsは水平分割数を意味すると仮定)
    const int stacks = segments / 2; // 垂直方向の層の数 (半球ごとに segments/4 を目安)
    const int slices = segments;    // 水平方向の分割数
    const float radius = 0.5f;      // 単位球の半径

    std::vector<SimpleVertex> vertices;
    std::vector<WORD> indices;

    // ----------------------------------------------------
    // 1. 頂点の生成 (UV Sphere)
    // ----------------------------------------------------

    // 天面の極 (North Pole)
    vertices.emplace_back(DirectX::XMFLOAT3(0.0f, radius, 0.0f));

    // 側面のリング (i = 1 から stacks-1 まで)
    for (int i = 1; i < stacks; ++i)
    {
        // 緯度角 (Phi) を計算: 0 ～ PI (90度から-90度)
        float phi = DirectX::XM_PI * (float)i / (float)stacks;
        float sin_phi = std::sin(phi);
        float cos_phi = std::cos(phi);

        // Y座標を計算
        float y = cos_phi * radius;
        // 現在のリングの半径 (xz平面上の距離)
        float current_radius = sin_phi * radius;

        for (int j = 0; j < slices; ++j)
        {
            // 経度角 (Theta) を計算: 0 ～ 2PI
            float theta = 2.0f * DirectX::XM_PI * (float)j / (float)slices;
            float sin_theta = std::sin(theta);
            float cos_theta = std::cos(theta);

            // X, Z座標を計算
            float x = current_radius * sin_theta;
            float z = current_radius * cos_theta;

            vertices.emplace_back(DirectX::XMFLOAT3(x, y, z));
        }
    }

    // 底面の極 (South Pole)
    vertices.emplace_back(DirectX::XMFLOAT3(0.0f, -radius, 0.0f));


    // ----------------------------------------------------
    // 2. インデックスの生成
    // ----------------------------------------------------

    // 総頂点数を確認
    const int num_vertices = static_cast<int>(vertices.size());
    const int last_pole_index = num_vertices - 1; // 底面の極のインデックス

    // A. 天面キャップ (北極と最初のリングを接続)
    // 最初のリングの開始インデックス: 1
    for (int j = 0; j < slices; ++j)
    {
        int i0 = 0;                          // 北極
        int i1 = 1 + j;                      // 現在の頂点
        int i2 = 1 + (j + 1) % slices;       // 次の頂点

        // (i0, i2, i1) の順で三角形を生成 (外側から見て反時計回り)
        indices.emplace_back(i0);
        indices.emplace_back(i2);
        indices.emplace_back(i1);
    }

    // B. 側面 (リング i と リング i+1 の間の四角形を接続)
    // リングの総数: stacks - 2 (両極を除く)
    for (int i = 0; i < stacks - 2; ++i)
    {
        int ring_start_a = 1 + i * slices;       // リング A の開始インデックス
        int ring_start_b = 1 + (i + 1) * slices; // リング B の開始インデックス

        for (int j = 0; j < slices; ++j)
        {
            // リング A の頂点
            int iA = ring_start_a + j;
            int iB = ring_start_a + (j + 1) % slices;

            // リング B の頂点
            int iC = ring_start_b + j;
            int iD = ring_start_b + (j + 1) % slices;

            // 1. 下側の三角形 (iA, iD, iC)
            indices.emplace_back(iA);
            indices.emplace_back(iD);
            indices.emplace_back(iC);

            // 2. 上側の三角形 (iA, iB, iD)
            indices.emplace_back(iA);
            indices.emplace_back(iB);
            indices.emplace_back(iD);
        }
    }

    // C. 底面キャップ (最後のリングと南極を接続)
    // 最後のリングの開始インデックス: last_pole_index - slices
    int last_ring_start = last_pole_index - slices;

    for (int j = 0; j < slices; ++j)
    {
        int i0 = last_pole_index;           // 南極
        int i1 = last_ring_start + j;       // 現在の頂点
        int i2 = last_ring_start + (j + 1) % slices; // 次の頂点

        // (i0, i1, i2) の順で三角形を生成 (外側から見て反時計回り)
        indices.emplace_back(i0);
        indices.emplace_back(i1);
        indices.emplace_back(i2);
    }

    out_data.Indices = indices;
    CreateD3D11Buffer(vertices, out_data.Indices, out_data.VertexBuffer, out_data.IndexBuffer);
}

void CollisionVisualizer::CreateCapsuleResources(ShapeData& out_data, float halfHeight, int segments)
{
    // ユニットカプセルとして、半径を 0.5f に固定します。
      // halfHeight は円筒部分の半分の高さ (カプセル全体の半分の長さではない) と仮定
    const float radius = 0.5f;
    const int N = segments; // DIVIDE_X (水平分割数)
    const int M = segments / 2; // DIVIDE_Y / 2 (半球の層数)

    std::vector<SimpleVertex> vertices;
    std::vector<WORD> indices;

    // 中心角
    const float centerRadianX = 2.0f * DirectX::XM_PI / static_cast<float>(N);
    const float centerRadianY = 2.0f * DirectX::XM_PI / static_cast<float>(M * 2); // DIVIDE_Y は 2M と等しい

    // 上部の中心オフセット
    const float offsetHeight = halfHeight;

    // ----------------------------------------------------
    // 1. 天面の極 (Pole)
    // ----------------------------------------------------
    vertices.emplace_back(DirectX::XMFLOAT3(0.0f, offsetHeight + radius, 0.0f));

    // ----------------------------------------------------
    // 2. 上部半球 (vy = 0 to M-1)
    // ----------------------------------------------------
    for (int vy = 0; vy < M; vy++)
    {
        // 緯度角 (Y軸周りの角度)
        float yRadian = static_cast<float>(vy + 1) * centerRadianY / 2.0f;

        // XZ平面でのリングの半径
        float tempLen = std::abs(std::sin(yRadian) * radius);
        // Y座標 (単位球Y座標)
        float y = std::cos(yRadian) * radius;

        for (int vx = 0; vx < N; vx++)
        {
            float x = tempLen * std::sin(static_cast<float>(vx) * centerRadianX);
            float z = tempLen * std::cos(static_cast<float>(vx) * centerRadianX);

            // Y軸を円筒のオフセットで移動
            DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(x, y + offsetHeight, z);
            vertices.emplace_back(pos);
        }
    }

    // ----------------------------------------------------
    // 3. 中央の円筒部 (上下の半球の繋ぎ目に追加の頂点を作成)
    // ----------------------------------------------------
    // 頂点 1 〜 N が上部半球の赤道リング。これを下にずらしたリングを追加する

    // Note: 厳密には、CapsuleCollider::CreateVertices() のロジックでは、円筒の頂点は作成されず、
    // 半球の境界線がそのまま繋がっているように見えます ( vy = M の層がない)。
    // したがって、このロジックでは中央に円筒状のメッシュを生成せず、
    // 半球の赤道部分を直接接続することで円筒に見せるようにします。
    // *ただし、CapsuleCollider::CreateVerticesのロジックを忠実に再現すると以下のようになります。

    /*
    // CapsulCollider::CreateVertices() のロジックを忠実に再現すると...
    // -> 上部半球の頂点生成を vy < DIVIDE_Y / 2 = M で行っている。
    // -> 下部半球の頂点生成を vy < DIVIDE_Y / 2 で行い、offset = DIVIDE_Y / 2 を使用している。
    // つまり、vy = M/2 の層が円筒の開始であり、vy = M/2 の層が円筒の終了である。
    // vy = M の層を中央に 1層追加するロジックは無いため、ここでは追加しません。
    */

    // ----------------------------------------------------
    // 4. 下部半球 (vy = M to 2M-1)
    // ----------------------------------------------------
    int offset = M; // vy = M から開始
    for (int vy = 0; vy < M; vy++)
    {
        // 緯度角 (Y軸周りの角度)
        float yRadian = static_cast<float>(vy + offset) * centerRadianY / 2.0f;

        // XZ平面でのリングの半径
        float tempLen = std::abs(std::sin(yRadian) * radius);
        // Y座標 (単位球Y座標)
        float y = std::cos(yRadian) * radius;

        for (int vx = 0; vx < N; vx++)
        {
            float x = tempLen * std::sin(static_cast<float>(vx) * centerRadianX);
            float z = tempLen * std::cos(static_cast<float>(vx) * centerRadianX);

            // Y軸を円筒のオフセットで移動 (下側)
            DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(x, y - offsetHeight, z);
            vertices.emplace_back(pos);
        }
    }

    // ----------------------------------------------------
    // 5. 底面の極 (Bottom Pole)
    // ----------------------------------------------------
    vertices.emplace_back(DirectX::XMFLOAT3(0.0f, -(offsetHeight + radius), 0.0f));

    // ----------------------------------------------------
    // 6. インデックスの生成 (SphereColliderの複雑なロジックを流用)
    // ----------------------------------------------------
    // Note: このロジックは、以前のSphereColliderのものとほぼ同一のため、
    // 頂点数が異なるカプセルでもそのまま利用可能です。

    const int TotalRings = 2 * M; // 側面リングの総数

    // 6-1. 天面
    int offsetIndex = 0;
    for (int i = 0; i < N * 3; i++)
    {
        if (i % 3 == 0) { indices.emplace_back(0); }
        else if (i % 3 == 1) { indices.emplace_back(1 + offsetIndex); }
        else if (i % 3 == 2)
        {
            int index = 2 + offsetIndex++;
            index = index > N ? indices[1] : index;
            indices.emplace_back(index);
        }
    }

    // 6-2. 側面
    int startIndex = indices[1];
    // 総層数は DIVIDE_Y = 2M。両極を除くので TotalRings - 2 = 2M - 2 層の間のメッシュ。
    int sideIndexLen = N * (TotalRings - 1) * 2 * 3; // (2M-1)層の間の四角形
    int loop1stIndex = 0;
    int loop2ndtIndex = 0;
    int lapDiv = N * 2 * 3;
    int createSquareFaceCount = 0;

    for (int i = 0; i < sideIndexLen; i++)
    {
        // 一周の頂点数を超えたら更新(初回も含む).
        if (i % lapDiv == 0)
        {
            loop1stIndex = startIndex;
            loop2ndtIndex = startIndex + N; // Nは水平分割数 (DIVIDE_X).
            createSquareFaceCount++;
        }

        if (i % 6 == 0 || i % 6 == 3)
        {
            indices.emplace_back(startIndex);
        }
        else if (i % 6 == 1)
        {
            indices.emplace_back(startIndex + N); // Nは水平分割数 (DIVIDE_X).
        }
        else if (i % 6 == 2 || i % 6 == 4)
        {
            if (i > 0 &&
                (i % (lapDiv * createSquareFaceCount - 2) == 0 ||
                    i % (lapDiv * createSquareFaceCount - 4) == 0))
            {
                // 一周したときのループ処理.
                // 周回ポリゴンの最後から２番目のIndex.
                indices.emplace_back(loop2ndtIndex); // 💡 修正: loop2ndtIndex を使用
            }
            else
            {
                indices.emplace_back(startIndex + N + 1);
            }
        }
        else if (i % 6 == 5)
        {
            if (i > 0 && i % (lapDiv * createSquareFaceCount - 1) == 0)
            {
                indices.emplace_back(loop1stIndex);
            }
            else { indices.emplace_back(startIndex + 1); }
            startIndex++;
        }
    }

    // 6-3. 底面
    int bottom_pole_index = static_cast<int>(vertices.size()) - 1;
    int last_ring_start_index = bottom_pole_index - N;
    int loopIndex = last_ring_start_index;
    offsetIndex = last_ring_start_index;

    for (int i = N * 3 - 1; i >= 0; i--)
    {
        if (i % 3 == 0)
        {
            indices.emplace_back(bottom_pole_index);
            offsetIndex++;
        }
        else if (i % 3 == 1) 
        { 
            indices.emplace_back(offsetIndex);
        }
        else if (i % 3 == 2)
        {
            int value = 1 + offsetIndex;
            if (value >= bottom_pole_index) { value = loopIndex; }
            indices.emplace_back(value);
        }
    }

    out_data.Indices = indices;
    CreateD3D11Buffer(vertices, out_data.Indices, out_data.VertexBuffer, out_data.IndexBuffer);
}