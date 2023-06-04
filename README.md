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

## 仕様

| ゲームステート           | 表示文字列                            | スクロール           |
| ------------------------ | ------------------------------------- | -------------------- |
| 起動直後                 | `NOW LOADING...`                      | YES                  |
| タイトル画面             | `WELCOME TO BEATMANIA IIDX INFINITAS` | YES (9 文字 padding) |
| エントリー               | `ENTRY`                               | NO                   |
| エントリー後モード選択前 | `DECIDE!`                             | NO                   |
| モード選択               | `MODE?`                               | NO                   |
| STANDARD 選択            | `STAY COOL`                           | NO                   |
| 選曲画面                 | `MUSIC SELECT!!`                      | YES                  |
| 楽曲にフォーカス         | 曲名                                  | YES                  |
| プレイ画面               | 曲名                                  | YES                  |
| リザルト                 | `[曲名] RESULT`                       | YES                  |
| 段位選択                 | `CLASS`                               | YES                  |
| 段位リザルト             | `CLASS RESULT`                        | YES                  |
| 保存画面                 | `THANK YOU FOR PLAYING!!`             | YES                  |

## 実装参考動画
- https://youtu.be/FS8T7abHsuQ
- https://youtu.be/xHBT0c2PS0c

## 注意

本DLLの使用は自己責任でお願いします

非公式ですので本DLLについて、ゲーム及び16セグメントLED表示器の開発・販売元に問い合わせることはご遠慮ください
