# μPD777エミュレータ

# これは何？
μPD777というののエミュレータです。

# ビルド環境
* Windowsのみ
* C++20
* CMake

## ビルド方法
1. rom.cppにROMイメージ、パターンを書きます
2. CMakeにてプロジェクトを作成し
3. ビルドします

# todo
* 命令の実装があっているのかどうか

# 動作イメージ
![Battle Vader](/wiki/image/BattleVader000.png)
![Battle Vader](/wiki/image/BattleVader001.png)
![Battle Vader](/wiki/image/BattleVader002.png)
![Battle Vader](/wiki/image/Galaxian000.png)
![Battle Vader](/wiki/image/Galaxian001.png)
![Battle Vader](/wiki/image/Galaxian002.png)

# 更新履歴
グラフィック
* スプライトの優先順位の調整
* スプライトの色の調整
* 表示範囲の調整

サウンド
* サイン波を矩形波に変更

入力
* PUSH1、PUSH2の２つのボタンを認識するように修正  
　@todo ボタン４個つかっているのは不明
