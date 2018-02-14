*****************************************************************************************************
Range Swap move for stereo imaging
*****************************************************************************************************
ステレオ対応問題に対して, 範囲移動アルゴリズムの性能を比較実験により検証するためのプログラム群
各アルゴリズムでの実行方法の詳細は各フォルダ内のReadme.txtを参照

ubuntu 17.10 + x86_64 Linux 4.8.0-54-generic + gcc version 7.2.0で動作確認.   
gccの最適化オプションは非推奨です(メモリ関係でエラーが出る可能性大).   
intel c compilerでは動作しませんでした.
-----------------------------------------------------------------------------------------
ファイル構成

フォルダ

    input
        実験に使用する画像
    ab-swp-str
        交換アルゴリズム
    alpha-expan-str
        α-拡張アルゴリズム
    GRSA_stereo
        範囲交換アルゴリズム

シェルスクリプト
    delResult.sh
        実験結果の削除
    runall.sh
        3種類の画像に対して,各アルゴリズムを実行する
        実験結果(エネルギー,実行時間,エラー率)の出力
            ab-swp-str/log (交換アルゴリズムの実験結果)
            alpha-expan-str/log (α-拡張アルゴリズムの実験結果)
            GRSA_stereo/log (範囲交換アルゴリズムの実験結果)
        画像の出力
            ab-swp-str/output (交換アルゴリズムの出力画像)
            alpha-expan-str/output (α-拡張アルゴリズムの出力画像)
            GRSA_stereo/output (範囲交換アルゴリズムの出力画像)
            
-----------------------------------------------------------------------------------------
実行方法

compile

$make  
実行

$./runAll.sh
結果の削除

$./delResult.sh
-----------------------------------------------------------------------------------------
パラメータ設定

打ち切り二乗関数

input image	λ	T
tsukuba	2	8
venus	2	8
teddy	2	9
区分線形関数

input image	λ	T
tsukuba	2	7
venus	3	7
teddy	2	9
-----------------------------------------------------------------------------------------
参考
Kangwei Liu et al. GRMA: Generalized Range Move Algorithms for the Efficient Optimization of MRFs International Journal of Computer Vision February 2017, Volume 121, Issue 3, pp 365–390