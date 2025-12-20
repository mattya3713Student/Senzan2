#pragma once
#include "Global.h"

namespace ColorPreset
{
	// Alphaアニメーション用.
	const DirectX::XMFLOAT3 Selected = DirectX::XMFLOAT3(0.07, 0.66, 1.0);

	// 固定.
	const DirectX::XMFLOAT4 UnActive = DirectX::XMFLOAT4(0,0,0,0.5);


}

namespace ColorUtil {
	inline DirectX::XMFLOAT4 RGBA(
		const DirectX::XMFLOAT3& rgb,
		float a)
	{
		return DirectX::XMFLOAT4(rgb.x, rgb.y, rgb.z, a);
	}
}