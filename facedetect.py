# -*- coding:utf-8 -*-
import numpy
import cv2
import sys
import os
import os.path
import re
from progressbar import ProgressBar
import time


def input_arg(argvs, argc):
    if (argc != 3):   # 引数が足りない場合は、その旨を表示
        print 'Usage: # python %s srcdirectory outputdirectory' % argvs[0]
        quit()        # プログラムの終了

    print 'Input directoryname = %s' % argvs[1]
    print 'Output directoryname = %s' % argvs[2]
    # 引数でとったディレクトリの文字列をリターン
    return argvs


def input_filename(directory_path):
    # コマンド引数でとったディレクトリに含まれているファイル（文字列を取得）
    # サブディレクトリは想定してない
    files = os.listdir(directory_path)
    file_array = []

    for file in files:
        # 引数でとったディレクトリの文字列とそのディレクトリに入っているファイル名を結合
        file_directory = directory_path + "/" + file
        # 再びリストに格納
        file_array.append(file_directory)

    return file_array


def fild_all_files(directory):
    for root, dirs, files in os.walk(directory):
        yield root
        for file in files:
            yield os.path.join(root, file)


def filenamelist(src_directory):
    # サブディレクトリを想定
    file_array = []
    for file in fild_all_files(src_directory):
        # .pngか.jpgのときリストに格納
        if file.endswith(".jpg") or file.endswith(".png"):
            file_array.append(file)

    return file_array

def create_directory(output_directory):
    if os.path.isdir(output_directory) == 0:
        print "Not exist \"%s\" folder. So create it." % output_directory
        os.makedirs(output_directory)
    else:
        print "Exist \"%s\" folder." % output_directory


#イメージのパスと名前付けに使う数字のはじめの数字
def facedetect(image_path, output_directory, num):
    #ファイル読み込み
    image = cv2.imread(image_path)
    #グレースケール変換
    image_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    #カスケード分類器の特徴量を取得する
    cascade = cv2.CascadeClassifier(cascade_path)
    #物体認識（顔認識）の実行
    facerect = cascade.detectMultiScale(image_gray, scaleFactor=1.1, minNeighbors=1, minSize=(50, 50))
    if len(facerect) <= 0:
        return 0

    for rect in facerect:
        # print rect
        x = rect[0]
        y = rect[1]
        w = rect[2]
        h = rect[3]
        # 正規表現でファイルネームだけに
        # http://docs.python.jp/2/library/re.html
        image_path = re.sub('^.*/', "", image_path)
        # 拡張子の削除
        image_path = image_path.rstrip(".jpg")
        image_path = image_path.rstrip(".png")
        # ファイルネームの決定
        img_name = "./" + output_directory + "/" + image_path + "_" + str(num) + '.jpg'
        # 出力窓を調整
        cut_img = image[y-h*0.2:y+h*1.2, x-w*0.2:x+h*1.2]
        cv2.imwrite(img_name, cut_img,(100,100))
        num += 1
    return num


if __name__ == "__main__":
    cascade_path = '/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml'

    argvs = sys.argv   # コマンドライン引数を格納したリストの取得
    argc = len(argvs)  # 引数の個数

    directory = input_arg(argvs, argc)
    # まず元画像があるディレクトリを持ってくる
    image_path = directory[1]
    # file_namesにディレクトリ込みのファイルネームを格納
    file_names = filenamelist(image_path)
    #引数にとった出力先ディレクトリの存在確認 & 作成
    create_directory(directory[2])

    i = 0

    if len(file_names) == 0:
        print "Not exist jpg or png file !"
    else :
        p = ProgressBar(len(file_names))
        print "File num = ",len(file_names)

    # 元画像のファイルの数だけ顔認識を続ける
    for file_name in file_names:
        # print file_name
        try:
            facedetect(file_name, directory[2], 1)
        except:
            print "Oops! Illegal image... Skip!!"
        p.update(i+1)
        i += 1

    print "Complete !"
