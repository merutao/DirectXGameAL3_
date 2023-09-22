//クリアのヘッダファイル
#pragma once

// ヘッダファイル読み込み
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

// class
class Clear {
public:
	/// <summary>
	/// delete
	/// </summary>
	~Clear();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	Sprite* clear_ = nullptr;

	uint32_t textureHandle2_ = 0;
	uint32_t textureHandle3_ = 0;
};
