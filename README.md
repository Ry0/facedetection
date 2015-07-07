# OpenCVを使った顔検出と切り出し

## C++版
c++版はopencvのサンプルから
切り出しはまだできていない

## Python版
ここのブログのコードにちょっと手を加えている．  
[http://symfoware.blog68.fc2.com/blog-entry-1524.html](http://symfoware.blog68.fc2.com/blog-entry-1524.html)

## How to run(共通)
Pythonのコードはエラー処理で出力先フォルダが存在しなかった場合，作成するようにした．

```bash
$ mkdir output
```

### Python
* ディレクトリを指定したらそのディレクトリに入っている画像ファイルをすべて顔認識して切り取る．  
* ~~指定したディレクトリには画像ファイルのみを配置しておかないとだめ（エラー処理してない）．~~
* [2015.7.6 更新] フォルダの中のフォルダにある画像とかも拾えるようにした．
* [2015.7.6 更新] プログレスバー設置(要 `pip install`)
```bash
sudo pip install progressbar2
```
* [2015.7.6 更新] 壊れた画像ファイルを読み込んでも，処理が止まらないようにした


####実行

```bash
$ python facedetect.py src output
```

### for C++

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./facedetect
```
