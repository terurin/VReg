# VReg

ソフトウェア的にレジスタの読み書きとして
表現できる機能を実現するライブラリです。
利用例は次のとおりです。

* CANのidとデータを紐づけるとき
  + 部分的に実装完了 
* UARTでレジスタ情報を確認したいとき
  + 未実装
* 仮想レジスタ情報を一覧作成したいとき
  + 未実装(データ構造は用意してあるのでフォーマッターを実装すれば良い)

なお、このライブラリは割り込み中の操作に原則として対応していません。

## 対象

C++20が動作する環境

## 開発環境

* ubuntu
```sh
sudo apt update && sudo apt upgrade
sudo apt install build-essential cmake git clang-format
```

## テスト方法

```sh
mkdir -p build && cd build
cmake .. && make -j && ctest
```

## 依存しているライブラリ

* google test(開発時のみ)
