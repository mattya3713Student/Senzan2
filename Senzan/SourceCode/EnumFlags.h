// EnumFlags.h

#pragma once
#include <type_traits> // std::underlying_type_t, std::enable_if_t, std::true_type, std::false_type
#include <cstdint>     // 標準の整数型

// 1. トレイト構造体: EnumType がビットフラグとして有効化されているかを示すマーカー
template <typename T>
struct is_scoped_enum_flag : std::false_type {};

// 2. テンプレート演算子のグローバル定義 (SFINAEにより、有効化された型でのみ動作)

// 基底型の別名定義
template <typename T>
using underlying_type = std::underlying_type_t<T>;

// テンプレート関数を有効化するためのエイリアス
template <typename T>
using enable_if_flag = std::enable_if_t<is_scoped_enum_flag<T>::value, T>;

template <typename T>
using enable_if_flag_ref = std::enable_if_t<is_scoped_enum_flag<T>::value, T&>;

// ------------------------------------
// a) 2項演算子 (|, &, ^)
// ------------------------------------

// | 演算子 (OR)
template <typename T>
enable_if_flag<T> operator|(T a, T b) noexcept
{
    return static_cast<T>(static_cast<underlying_type<T>>(a) | static_cast<underlying_type<T>>(b));
}

// & 演算子 (AND)
template <typename T>
enable_if_flag<T> operator&(T a, T b) noexcept
{
    return static_cast<T>(static_cast<underlying_type<T>>(a) & static_cast<underlying_type<T>>(b));
}

// ^ 演算子 (XOR)
template <typename T>
enable_if_flag<T> operator^(T a, T b) noexcept
{
    return static_cast<T>(static_cast<underlying_type<T>>(a) ^ static_cast<underlying_type<T>>(b));
}

// ------------------------------------
// b) 単項演算子 (~)
// ------------------------------------

// ~ 演算子 (NOT/反転)
template <typename T>
enable_if_flag<T> operator~(T a) noexcept
{
    return static_cast<T>(~static_cast<underlying_type<T>>(a));
}

// ------------------------------------
// c) 複合代入演算子 (|=, &=, ^=)
// ------------------------------------

// |= 演算子 (OR 代入)
template <typename T>
enable_if_flag_ref<T> operator|=(T& a, T b) noexcept
{
    a = a | b;
    return a;
}

// &= 演算子 (AND 代入)
template <typename T>
enable_if_flag_ref<T> operator&=(T& a, T b) noexcept
{
    a = a & b;
    return a;
}

// ^= 演算子 (XOR 代入)
template <typename T>
enable_if_flag_ref<T> operator^=(T& a, T b) noexcept
{
    a = a ^ b;
    return a;
}


// ------------------------------------
// 3. 有効化マクロ
// ------------------------------------

// ユーザーが対象の Enum Class をビットフラグとしてマークするためのマクロ
#define ENABLE_ENUM_FLAG_OPERATORS(EnumType) \
    template <> struct is_scoped_enum_flag<EnumType> : std::true_type {};