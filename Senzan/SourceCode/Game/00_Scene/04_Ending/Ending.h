#pragma once
#include "../00_Base/SceneBase.h"

/*********************************************
*	エンディングクラス.
**/

class Ending
	: public SceneBase
{
public:
	Ending();
	~Ending() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
private:

};