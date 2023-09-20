#include "Title.h"

void Title::Initialize(Model* model, uint32_t textureHandle, Vector3 position) {

	assert(model);

	model_ = model;

	texturehandle_ = textureHandle;

	worldTransform_.translation_ = position;


}



void Title::Draw(ViewProjection& viewProjection){

	model_->Draw(worldTransform_, viewProjection, texturehandle_);

};

void Title::UpDate(){};
