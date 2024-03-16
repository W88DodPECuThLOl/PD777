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

# コマンドラインオプション

| オプション | 説明 |
|---|---|
| -code <コードファイル名> | コードファイルを読み込む |
| -ptn <パターンファイル名> | パターンファイルを読み込む |
| -disassemble | コードを逆アセンブルし終了 |

ファイルのフォーマットなどは ⇒ [PD777supplement Pages](https://github.com/W88DodPECuThLOl/PD777supplement)

# 動作イメージ
![Battle Vader](/wiki/image/BattleVader000.png)
![Battle Vader](/wiki/image/BattleVader001.png)
![Battle Vader](/wiki/image/BattleVader002.png)
![Battle Vader](/wiki/image/Galaxian000.png)
![Battle Vader](/wiki/image/Galaxian001.png)
![Battle Vader](/wiki/image/Galaxian002.png)
