# L293D制御基板コントローラ

UARTでL923Dを制御する基板に指示を出すための試験用コントローラです。

## 使い方

シリアル接続して`1`から`8`までのキーを押下すると、対応するピンの出力が切り替わります。その他のキーを押下すると、全てのピンの出力がLOWになります。シリアルモニタは改行コード等が送信されない直接入力モード (pio device monitorでは既定) で使用してください。

## ピン配置

- 11: UART TX - ATtinyX4のRX (11) に接続
- 12: UART RX - ATtinyX4のTX (12) に接続

![](https://github.com/SpenceKonde/ATTinyCore/blob/v2.0.0-devThis-is-the-head-submit-PRs-against-this/avr/extras/ATtiny_x4.png)