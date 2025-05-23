# OpenSiv3DSpineTest

[OpsnSiv3D](https://github.com/Siv3D/OpenSiv3D) v0.6.15を使った[Spine](http://esotericsoftware.com/) v4.1の描画。

### 実行例

https://github.com/user-attachments/assets/cd06e923-3438-4ba5-8eb0-44322e3ef4bd

## ファイル構成

### Spineに関係するもの

| ファイル | 機能 |
| --- | --- |
| [siv3d_spine.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_spine.cpp) | Siv3Dの機能を使ったSpineのテクスチャ生成・破棄、描画処理。 |
| [siv3d_spine_blendmode.h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_spine_blendmode.h) | Siv3Dの定数に基づくSpine混色法定義。 |
| [siv3d_spine_player.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_spine_player.cpp) | Siv3Dの機能を使ったSpine描画時の視点・拡縮補正。 |
| [spine_loader.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/spine_loader.cpp) | Spine出力ファイルの取り込み処理。 |
| [spine_player.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/spine_player.cpp) | Spineの機能命令を閲覧向けにまとめたもの。 |

- これらのファイルを公式の汎用ランタイムと一緒に使うことでSiv3DでのSpine描画が行えます。
- `spine_player.cpp/h`は他の描画ライブラリでも使えるような構成にしてあるため、Siv3D向けに最適化されていません。
具体的には`<string>`や`<vector>`を直に使用しています。
  - 最下限の統合は`siv3d_spine.cpp/h`と`siv3d_spine_blendmode.h`で行えるので、用途に応じてSpineの機能命令群はまとめ直して下さい。
- 混色法に関して、定義済みの各種`s3d::BlendState`は基本的に`dstA = dstA`の計算式になっていて`srcA`を寄与させる表現ができないため、Spineに関係する混色法は全てユーザ定義を行っています。

### Spineに関係しないもの

動作確認用のファイルです。

| ファイル | 機能 |
| --- | --- |
| [Main.cpp](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/Main.cpp) | エントリ関数記載。 |
| [siv3d_main_window.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_main_window.cpp) | Siv3Dのウィンドウ表示。 |
| [siv3d_window_menu.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_window_menu.cpp) | メニュー操作結果通知。 |
| [siv3d_recorder.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_recorder.cpp) | 描画結果の動画出力。 |

## プロジェクト構成

- 環境変数は使わず、Siv3DのSDKは`project/deps`階層下に置く構成になっています。
  - [project/deps/CMakeLists](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/project/deps/CMakeLists.txt)を実行することでSpine含め依存ライブラリの取得と配置が行えます。

## 補足

Spine `3.8`, `4.0`, `4.1`で動作確認を行っています。  
- `3.6`より古い版で動かしたい場合、`spine-cpp`ではなく`spine-c`を使うことになりますので[別ライブラリの作り](https://github.com/BithreenGirlen/DxLibSpineViewer/blob/main/DxLibSpineC)を参考にしてください。
