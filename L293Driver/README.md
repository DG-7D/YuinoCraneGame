# L293Dシリアルコントロール

UARTでL293Dの4セット8出力を制御する基板用のATtinyX4 (24, 44, 84) のプログラムです。

## 使い方

1バイト(8bit)のデータを受信すると、それぞれのビットが出力に対応します。

## 参考情報

[L293Dのデータシート](https://www.ti.com/document-viewer/ja-jp/l293d/datasheet)

ATtinyx4のピン配置

![](https://github.com/SpenceKonde/ATTinyCore/blob/v2.0.0-devThis-is-the-head-submit-PRs-against-this/avr/extras/ATtiny_x4.png)

Arduino Uno使用時(動作確認用)の出力ピン: 2-9