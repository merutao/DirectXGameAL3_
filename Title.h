#pragma once
#include "EnemyBullet.h"
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include <list>



class Title {
public:

	void Initialize(Model* model, uint32_t textureHandle, Vector3 position);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	void UpDate();

	WorldTransform worldTransform_;
	private:
	Model* model_ = nullptr;
	uint32_t texturehandle_ = 0u;
};
