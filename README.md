# OpenCVを使った顔検出と切り出し

## C++版
c++版はopencvのサンプルから
切り出しはまだできていない

## Python版
ここのブログのコードにちょっと手を加えている  
[http://symfoware.blog68.fc2.com/blog-entry-1524.html](http://symfoware.blog68.fc2.com/blog-entry-1524.html)

## How to run(共通)

```bash
$ mkdir output
```

### Python
ディレクトリを指定したらそのディレクトリに入っている画像ファイルをすべて顔認識して切り取る  
指定したディレクトリには画像ファイルのみを配置しておかないとだめ（エラー処理してない）

```bash
$ python facedetect.py src
```

### for C++

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./facedetect
```
