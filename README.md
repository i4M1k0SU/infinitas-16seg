# INFINITAS 16Seg

beatmania IIDX INFINITASで16セグメントLEDを利用できるようにするDLL

ファイル読み込みベースのゲームステート取得により、表示文字列を生成する

## 対応デバイス

- [HTLAB.NET製16セグメントLED表示器 (16 Segment 9 Digit Display Controller Ver2.0)](https://htlab.net/products/16-segment-9-digit-display-controller-2/)

## 使い方

### DLL Proxy (オススメ)

- `infinitas-16seg.dll` を `dbghelp.dll` にリネームしてください
- `dbghelp.dll` をインストールディレクトリ (デフォルトであれば `C:\Games\beatmania IIDX INFINITAS\game\app` ) にコピーしてください

### DLL Injection

- 任意のDLL Injectorを使い、 `infinitas-16seg.dll` を `bm2dx.exe` にinjectしてください

## 注意

本DLLの使用は自己責任でお願いします

非公式ですので本DLLについて、ゲーム及び16セグメントLED表示器の開発・販売元に問い合わせることはご遠慮ください

本DLLは現在、概念実証レベルです

あくまで、何かが表示される程度であるという点をご理解ください
