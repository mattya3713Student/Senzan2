#pragma once

#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <DirectXMath.h>
#include "System/Singleton/SingletonTemplate.h"

// ログレベル.
enum class LogLevel
{
    Info,
    Warning,
    Error
};

class Log final
    : public Singleton<Log>
{
    friend class Singleton<Log>;
    Log() {}

public:
    inline void LogInfo(const std::string& message) const { OutputLog(LogLevel::Info, message); }
    inline void LogWarning(const std::string& message) const { OutputLog(LogLevel::Warning, message); }
    inline void LogError(const std::string& message) const { OutputLog(LogLevel::Error, message); }

    template<typename T>
    inline void Info(const std::string& prefix, const T& value) const
    {
        LogInternal(LogLevel::Info, prefix, value);
    }

    template<typename T>
    inline void Warning(const std::string& prefix, const T& value) const
    {
        LogInternal(LogLevel::Warning, prefix, value);
    }

    template<typename T>
    inline void Error(const std::string& prefix, const T& value) const
    {
        LogInternal(LogLevel::Error, prefix, value);
    }
private:
    // ログレベルに応じたプレフィックスを生成.
    inline std::string GetPrefix(LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::Error:   return "[ERROR] ";
            case LogLevel::Warning: return "[WARNING] ";
            case LogLevel::Info:    return "[INFO] ";
            default:                return "[LOG] ";
        }
    }

    // 最終的なデバッグ出力を行うコア関数.
    inline void OutputLog(LogLevel level, const std::string& message) const
    {
        std::stringstream ss;
        ss << GetPrefix(level) << message << "\n";
        std::string full_message = ss.str();

        // コンソール出力に統一
        if (level == LogLevel::Error) {
            std::cerr << full_message;
        }
        else {
            std::cout << full_message;
        }
    }

    // 標準型.
    template<typename T>
    inline std::string Format(const T& value) const
    {
        std::stringstream ss;
        // 浮動小数点数にのみ適用される.
        ss << std::fixed << std::setprecision(6) << value;
        return ss.str();
    }

    // DirectX::XMFLOATx.
    inline std::string Format(DirectX::XMFLOAT2 value) const
    {
        return "(X: " + Format(value.x) + ", Y: " + Format(value.y) + ")";
    }
    inline std::string Format(DirectX::XMFLOAT3 value) const
    {
        return "(X: " + Format(value.x) + ", Y: " + Format(value.y) + ", Z: " + Format(value.z) + ")";
    }
    inline std::string Format(DirectX::XMFLOAT4 value) const
    {
        return "(X: " + Format(value.x) + ", Y: " + Format(value.y) + ", Z: " + Format(value.z) + ", W: " + Format(value.w) + ")";
    }

    // DirectX::XMFLOAT4X4.
    inline std::string Format(const DirectX::XMFLOAT4X4& value) const
    {
        std::stringstream ss;
        ss << "{\n"
            << " [0]: " << Format(value._11) << ", " << Format(value._12) << ", " << Format(value._13) << ", " << Format(value._14) << "\n"
            << " [1]: " << Format(value._21) << ", " << Format(value._22) << ", " << Format(value._23) << ", " << Format(value._24) << "\n"
            << " [2]: " << Format(value._31) << ", " << Format(value._32) << ", " << Format(value._33) << ", " << Format(value._34) << "\n"
            << " [3]: " << Format(value._41) << ", " << Format(value._42) << ", " << Format(value._43) << ", " << Format(value._44) << "\n"
            << "}";
        return ss.str();
    }

    // コンテナ.
    template<typename T>
    inline std::string FormatContainer(const T& container) const
    {
        std::stringstream ss;
        ss << "[ ";
        for (const auto& item : container) {
            ss << Format(item) << ", ";
        }
        std::string result = ss.str();
        if (result.size() > 2) {
            result.resize(result.size() - 2);
        }
        result += " ]";
        return result;
    }

    // std::vector.
    template<typename T, typename Allocator>
    inline std::string Format(const std::vector<T, Allocator>& vec) const
    {
        return "Vector" + FormatContainer(vec);
    }

    // std::array.
    template<typename T, std::size_t N>
    inline std::string Format(const std::array<T, N>& arr) const
    {
        return "Array" + FormatContainer(arr);
    }

    // テンプレート.
    template<typename T>
    inline void LogInternal(LogLevel level, const std::string& prefix, const T& value) const
    {
        OutputLog(level, prefix + ": " + Format(value));
    }
};