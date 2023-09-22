#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "DebugCamera.h"
#include "Enemy.h"
#include "skydome.h"
#include "RailCamera.h"
#include <sstream>
#include "Title.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 敵弾の追加する
	/// </summary>
	/// <param name="enemyBullet"></param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommand();


	void EnemyOccurrence(Vector3 position, Vector3 velocity);

	void Clear();

	void Timer();

	void Over();

	void TitleProc();

	void ClearProc();

	void OverProc();

	void TitleDraw();
	enum class NowGameScene {
		Title, // タイトル
		Play,  // プレイシーン
		Clear, // クリアシーン
		Over,  // ゲームオーバーシーン
	};

	enum NowGameScene NowGameScene;


private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	Title* title_ = nullptr;
	uint32_t textureHandle_ = 0;
	
	uint32_t textureHandleModel_ = 0;
	float inputFloat3[3] = {0, 0, 0};
	void CheckAllCollision();
	int Count = 5;

	
	//Sprite* sprite_ = nullptr;
	Model* model_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Player* player_ = nullptr;
	ViewProjection viewProjection_;
	Enemy* enemy_ = nullptr;
	Skydome* skydome_ = nullptr;
	RailCamera* railCamera_ = nullptr; 
	EnemyBullet* enemyBullet_ = nullptr;

	

	


	// 弾
	std::list<EnemyBullet*> enemyBullets_;
	//敵
	std::list<Enemy*> enemys_;
	//敵発生コマンド
	std::stringstream enemyPopCommands;

	int standFlag = false;
	int standTime = 0;

	int Time = 1000;
	int TFlag = 0;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	//デバックカメラ有効
	bool isDebugCameraActive_ = false;

	//デバックカメラ
	DebugCamera* debugCamera_ = nullptr;
	

	
};