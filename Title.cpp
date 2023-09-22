﻿//タイトルのソースファイル

//ヘッダファイル読み込み
#include "Title.h"

//delete
Title::~Title()
{ delete title_; }

/// 初期化
void Title::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle1_ = TextureManager::Load("Title.png");
	
	title_ = Sprite::Create(textureHandle1_, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1}, {0.5f, 0.5f});

}

/// 更新
void Title::Update() 
{

}

///描画
void Title::Draw() 
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	title_->Draw();
	
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

}