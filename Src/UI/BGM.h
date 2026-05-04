#pragma once
#include <unordered_map>
#include <string>
#include<vector>
#include<functional>


class BGM
{
public:

    enum class BgmType {
        TITLE = 0,
        GAME,
        CLEAR,
        GAMEOVER,
        NONE
    };

    BGM();
    ~BGM();

    // BGM種類を指定してセット
    void Set(BgmType type);

    // 今セットされているファイルパスを取得
    std::string GetPath() const { return bgmPath_; }

    // 今セットされているBGM名
    std::string GetName() const { return bgmName_; }
    static int ToSoundID(BgmType type);
private:
    std::string bgmPath_;   // 再生に使うファイルパス
    std::string bgmName_;   // 表示用の曲名
};
