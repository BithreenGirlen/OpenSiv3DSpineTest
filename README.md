# OpenSiv3DSpineTest

[OpsnSiv3D](https://github.com/Siv3D/OpenSiv3D) v0.6.15を使った[Spine](http://esotericsoftware.com/) v4.1の描画。

### 実行例

https://github.com/user-attachments/assets/cd06e923-3438-4ba5-8eb0-44322e3ef4bd

## ファイル構成
`src`階層下にコードを置いてあります。

| ファイル | 機能  |
| --- | --- |
| [Main.cpp](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/Main.cpp) | エントリ関数記載。 |
| [siv3d_main_window.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_main_window.cpp) | Siv3Dのウィンドウ表示。 |
| [siv3d_spine.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_spine.cpp) | Siv3Dの機能を使ったSpineのテクスチャ生成・破棄、描画処理。 |
| [siv3d_spine_player.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_spine_player.cpp) | Siv3Dの機能を使ったSpine描画時の視点・拡縮補正。 |
| [spine_loader.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/spine_loader.cpp) | Spine出力ファイルの取り込み処理。 |
| [spine_player.cpp/h](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/spine_player.cpp) | Spineの機能命令を閲覧向けにまとめたもの。 |

- `spine_loader.cpp/h`と`spine_player.cpp/h`は他の描画ライブラリでも使えるような構成にしてあるため、Siv3D向けに最適化されていません。
具体的には`<string>`や<`vector>`を直に使用しています。
  - 最下限の統合は`siv3d_spine.cpp/h`で行えますので、用途に応じてSpineの機能命令・ファイルデータをまとめるクラスは別途作成してください。

## プロジェクト構成
- 環境変数をいじりたくなかったので、Siv3DのSDKは`project/deps`階層下に置く構成になっています。
  - [project/deps/CMakeLists](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/project/deps/CMakeLists.txt)を実行することでSpine含め依存ライブラリの取得と配置が行えます。
- プリコンパイルヘッダ名は`stdafx.h`ではなく`pch.h`にしてあります。

## 補足

Spine v4.1で動作確認を行ってますが、v3.8でも問題なく動くかと思います。  
- もっと古い版で動かしたい場合、`spine-cpp`ではなく`spine-c`を使うことになりますので[別ライブラリの作り](https://github.com/BithreenGirlen/DxLibSpineViewer/blob/main/DxLibSpineC)を参考にしてください。
- より新しい版で動かしたい場合、本プロジェクトファイルではマクロによる分岐を行ってますが、
v4.0->v4.1->v4.2の変遷に於ける汎用ランタイムの破壊的変更は多いので、版毎に`siv3d_spine.cpp/h`を作成した方が無難かもしれません。

### 混色法に関して

定義済みの各種`s3d::BlendState`は基本的に`dstA = dstA`の計算式になっていて、透明度を表現することができません。  
そのため、Spineに関わる混色法は全て[siv3d_spine.cpp](https://github.com/BithreenGirlen/OpenSiv3DSpineTest/blob/main/src/siv3d_spine.cpp)にてユーザ定義を行っています。
