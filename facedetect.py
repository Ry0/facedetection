# -*- coding:utf-8 -*-
import numpy
import cv2
import sys
import os


def input_arg(argvs, argc):
    if (argc != 2):   # 引数が足りない場合は、その旨を表示
        print 'Usage: # python %s filename' % argvs[0]
        quit()         # プログラムの終了

    print 'Input filename = %s' % argvs[1]
    return argvs[1]


def facedetect(image_path, num):
    #ファイル読み込み
    image = cv2.imread(image_path)
    #グレースケール変換
    image_gray = cv2.cvtColor(image, cv2.cv.CV_BGR2GRAY)
    #カスケード分類器の特徴量を取得する
    cascade = cv2.CascadeClassifier(cascade_path)
    #物体認識（顔認識）の実行
    facerect = cascade.detectMultiScale(image_gray, scaleFactor=1.1, minNeighbors=1, minSize=(1, 1))
    if len(facerect) <= 0:
        exit()

    for rect in facerect:
        print rect
        x = rect[0]
        y = rect[1]
        w = rect[2]
        h = rect[3]
        # 拡張子の削除
        image_path = image_path.strip("src/")
        image_path = image_path.rstrip(".jpg")
        image_path = image_path.rstrip(".png")
        # ファイルネームの決定
        img_name = "./output/" + image_path + "_" + str(num) + '.jpg'
        # 出力窓を調整
        cv2.imwrite(img_name, image[y-h*0.4:y+h*1.4, x-w*0.4:x+h*1.4])
        num += 1
    return num


if __name__ == "__main__":
    cascade_path = '/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml'

    argvs = sys.argv   # コマンドライン引数を格納したリストの取得
    argc = len(argvs)  # 引数の個数

    image_path = input_arg(argvs, argc)
    facedetect(image_path, 1)
