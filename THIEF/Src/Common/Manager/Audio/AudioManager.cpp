#include "AudioManager.h"

#include <DxLib.h>
#include <algorithm>
#include "SoundTable.h"

AudioManager* AudioManager::instance_ = nullptr;

AudioManager::AudioManager(void)
{
	currentBgm_ = SoundID::NON;
	bgmVolume_ = 127;
	seVolume_ = 127;
	masterVolume_ = 127;
}

AudioManager::~AudioManager()
{
	// 念のため削除関数
	DeleteAll();
}

void AudioManager::Init(void)
{
	// 現在再生されているBGM
	currentBgm_ = SoundID::NON;

	bgmVolume_ = 127;		// bgm音量
	seVolume_ = 127;		// se音量
	masterVolume_ = 127;	// master音量
}

void AudioManager::LoadSceneSound(LoadScene scene)
{
	// 空のテーブルを用意
	const std::unordered_map<SoundID, SoundData>* table = nullptr;

	// シーンで読み込みたいテーブルを切り替える
	switch (scene)
	{
	case LoadScene::SYSTEM: table = &SoundTable_System::Table; break;
	case LoadScene::TITLE: table = &SoundTable_Title::Table; break;
	case LoadScene::MAIN_MENU:  table = &SoundTable_MainMenu::Table; break;
	case LoadScene::GAME:  table = &SoundTable_Game::Table; break;
	case LoadScene::SHOP:  table = &SoundTable_Shop::Table; break;
	}

	// 存在しないテーブルを選んでいたら処理しない
	if (!table) return;

	// 指定したテーブルのサウンドを全て読み込む
	for (auto& [id, data] : *table)
	{
		// 既に読み込まれていたら削除
		auto it = handles_.find(id);
		if (it != handles_.end())
		{
			DeleteSoundMem(it->second.handle);
			handles_.erase(it);
		}

		// サウンドを読み込む
		SoundResource res;

		res.handle = LoadSoundMem(data.path.c_str());
		res.volume = data.volume;

		handles_[id] = res;
	}
}

void AudioManager::DeleteSceneSound(LoadScene scene)
{
	// 空のテーブルを用意
	const std::unordered_map<SoundID, SoundData>* table = nullptr;

	// シーンで削除したいテーブルを切り替える
	switch (scene)
	{
	case LoadScene::SYSTEM: table = &SoundTable_System::Table; break;
	case LoadScene::TITLE: table = &SoundTable_Title::Table; break;
	case LoadScene::MAIN_MENU:  table = &SoundTable_MainMenu::Table; break;
	case LoadScene::GAME:  table = &SoundTable_Game::Table; break;
	case LoadScene::SHOP:  table = &SoundTable_Shop::Table; break;
	}

	// 存在しないテーブルを選んでいたら処理しない
	if (!table) return;

	// 指定したテーブルのサウンドを削除する
	for (auto& [id, _] : *table)
	{
		// IDからサウンドハンドルを抽出
		auto it = handles_.find(id);

		// サウンドが読み込まれているか？
		if (it != handles_.end())
		{
			// 削除する
			DeleteSoundMem(it->second.handle);

			// 配列からも削除
			handles_.erase(it);
		}
	}
}

void AudioManager::PlayBGM(SoundID id)
{
	// IDからサウンドハンドルを抽出
	auto it = handles_.find(id);

	// サウンドが読み込まれているか？
	if (it == handles_.end())
		// 読み込まれていないので終了
		return;

	// 現在のBGMが同じならスキップ
	if (currentBgm_ == id && CheckSoundMem(it->second.handle))
		return;

	// 別のBGMが再生中なら停止
	if (currentBgm_ != SoundID::NON)
		StopBGM();

	// BGMを更新
	currentBgm_ = id;

	// 実音量を計算
	auto& sound = it->second;
	int volume = static_cast<int>(sound.volume * (bgmVolume_ / 255.0f) * (masterVolume_ / 255.0f));

	// 音量を変更
	ChangeVolumeSoundMem(volume, sound.handle);

	// BGMなのでループ再生
	PlaySoundMem(sound.handle, DX_PLAYTYPE_LOOP, true);
}

void AudioManager::StopBGM()
{
	// 何も再生されていないなら何もしない
	if (currentBgm_ == SoundID::NON)
		return;

	// IDからサウンドハンドルを抽出
	auto it = handles_.find(currentBgm_);

	// サウンドが読み込まれているか？
	if (it != handles_.end())
		// 読み込まれているのでサウンドを止める
		StopSoundMem(it->second.handle);

	// 現在のBGMを再生していない状態に更新
	currentBgm_ = SoundID::NON;
}

void AudioManager::PlaySE(SoundID id)
{
	// IDからサウンドハンドルを抽出
	auto it = handles_.find(id);

	// サウンドが読み込まれているか？
	if (it == handles_.end())
		// 読み込まれていないので終了
		return;

	// 実音量を計算
	auto sound = it->second;
	int volume = static_cast<int>(sound.volume * (seVolume_ / 255.0f) * (masterVolume_ / 255.0f));

	// 音量を変更
	ChangeVolumeSoundMem(volume, sound.handle);

	// SEは複数同時再生を許可
	PlaySoundMem(sound.handle, DX_PLAYTYPE_BACK, true);
}

void AudioManager::DeleteAll(void)
{
	// サウンドが1つも読み込まれてないなら処理しない
	if (handles_.empty())
		return;

	// 全てのサウンドを検索
	for (auto& [id, data] : handles_)
	{
		// エラーサウンドかをチェック
		if (CheckSoundMem(data.handle) != -1)
		{
			// 削除する
			DeleteSoundMem(data.handle);
		}
	}

	// 配列をクリア
	handles_.clear();
}

void AudioManager::SetBgmVolume(int volume)
{
	// 0 ～ 255 までに収める
	bgmVolume_ = std::clamp(volume, 0, 255);

	// 別のBGMが再生中
	if (currentBgm_ != SoundID::NON)
	{
		// IDからサウンドハンドルを抽出
		auto it = handles_.find(currentBgm_);

		// サウンドが読み込まれているか？
		if (it != handles_.end())
		{
			// 実音量を計算
			auto& sound = it->second;
			int volume = static_cast<int>(sound.volume * (bgmVolume_ / 255.0f) * (masterVolume_ / 255.0f));

			// 音量を変更
			ChangeVolumeSoundMem(volume, sound.handle);
		}
	}
}

void AudioManager::SetSeVolume(int volume)
{
	// 0 ～ 255 までに収める
	seVolume_ = std::clamp(volume, 0, 255);
}

void AudioManager::SetMasterVolume(int volume)
{
	// 0 ～ 255 までに収める
	masterVolume_ = std::clamp(volume, 0, 255);

	// 再生中のBGMに即時反映
	if (currentBgm_ != SoundID::NON)
	{
		// IDからサウンドハンドルを抽出
		auto it = handles_.find(currentBgm_);

		// サウンドが読み込まれているか？
		if (it != handles_.end())
		{
			// 実音量を計算
			auto& sound = it->second;
			int volume = static_cast<int>(sound.volume * (bgmVolume_ / 255.0f) * (masterVolume_ / 255.0f));

			// 音量を変更
			ChangeVolumeSoundMem(volume, sound.handle);
		}
	}
}
