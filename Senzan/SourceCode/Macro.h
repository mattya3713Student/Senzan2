#pragma once
// 解放.
#define SAFE_RELEASE(p)do{if(p!=nullptr){(p)->Release();(p)=nullptr;}}while(0)

// 破棄.
#define SAFE_DELETE(p)do{if(p!=nullptr){delete (p);(p)=nullptr;}}while(0)

// 配列破棄.
#define SAFE_DELETE_ARRAY(p)do{if(p!=nullptr){delete[](p);(p) = nullptr;}}while(0)

// enumにビット演算を定義.

// トレイト構造体: EnumType がビットフラグとして有効化されているかを示すマーカー.
template <typename T>
struct is_scoped_enum_flag : std::false_type {};

// 基底型の別名定義.
template <typename T>
using underlying_type = std::underlying_type_t<T>;

// テンプレート関数を有効化するためのエイリアス.
template <typename T>
using enable_if_flag = std::enable_if_t<is_scoped_enum_flag<T>::value, T>;
template <typename T>
using enable_if_flag_ref = std::enable_if_t<is_scoped_enum_flag<T>::value, T&>;
template <typename T>
struct is_scoped_enum_flag : std::false_type {};

// | 演算子 (OR).
template <typename T> enable_if_flag<T> operator|(T a, T b) noexcept { return static_cast<T>(static_cast<underlying_type<T>>(a) | static_cast<underlying_type<T>>(b)); }
// & 演算子 (AND).
template <typename T> enable_if_flag<T> operator&(T a, T b) noexcept { return static_cast<T>(static_cast<underlying_type<T>>(a) & static_cast<underlying_type<T>>(b)); }
// ^ 演算子 (XOR).
template <typename T> enable_if_flag<T> operator^(T a, T b) noexcept { return static_cast<T>(static_cast<underlying_type<T>>(a) ^ static_cast<underlying_type<T>>(b)); }
// ~ 演算子 (NOT/反転).
template <typename T> enable_if_flag<T> operator~(T a) noexcept { return static_cast<T>(~static_cast<underlying_type<T>>(a)); }
// |= 演算子 (OR 代入).
template <typename T> enable_if_flag_ref<T> operator|=(T& a, T b) noexcept { a = a | b; return a; } 
// &= 演算子 (AND 代入).
template <typename T> enable_if_flag_ref<T> operator&=(T& a, T b) noexcept { a = a & b; return a; }
// ^= 演算子 (XOR 代入).
template <typename T> enable_if_flag_ref<T> operator^=(T& a, T b) noexcept { a = a ^ b; return a; }

#define ENABLE_ENUM_FLAG_OPERATORS(EnumType) \
    template <> struct is_scoped_enum_flag<EnumType> : std::true_type {};

// D3DXMATRIXをDirectX::XMMATRIXに変換.
inline DirectX::XMMATRIX D3DXMatrixToXMMatrix(const D3DXMATRIX & d3d9Matrix)
{
	// D3DXMATRIXの各行のfloat要素をDirectX::XMVECTORとしてロードする
	// XMMatrixは行優先でデータを格納する

	DirectX::XMVECTOR Row0 = DirectX::XMVectorSet(d3d9Matrix._11, d3d9Matrix._12, d3d9Matrix._13, d3d9Matrix._14);
	DirectX::XMVECTOR Row1 = DirectX::XMVectorSet(d3d9Matrix._21, d3d9Matrix._22, d3d9Matrix._23, d3d9Matrix._24);
	DirectX::XMVECTOR Row2 = DirectX::XMVectorSet(d3d9Matrix._31, d3d9Matrix._32, d3d9Matrix._33, d3d9Matrix._34);
	DirectX::XMVECTOR Row3 = DirectX::XMVectorSet(d3d9Matrix._41, d3d9Matrix._42, d3d9Matrix._43, d3d9Matrix._44);

	// 4つのXMVECTOR行ベクトルからXMMATRIXを構築する
	return DirectX::XMMATRIX(Row0, Row1, Row2, Row3);
}

// D3DXMATRIXをDirectX::XMMATRIXに変換.
inline void XMMatrixToD3DXMatrix(D3DXMATRIX & outD3d9Matrix, const DirectX::XMMATRIX & xmMatrix)
{
	// XMMATRIXの各行ベクトルを取得.
	DirectX::XMVECTOR Row0 = xmMatrix.r[0];
	DirectX::XMVECTOR Row1 = xmMatrix.r[1];
	DirectX::XMVECTOR Row2 = xmMatrix.r[2];
	DirectX::XMVECTOR Row3 = xmMatrix.r[3];

	// 各行ベクトルをD3DXMATRIXのメモリ領域にストアする.
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&outD3d9Matrix.m[0]), Row0);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&outD3d9Matrix.m[1]), Row1);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&outD3d9Matrix.m[2]), Row2);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&outD3d9Matrix.m[3]), Row3);
}