# -*- coding: utf-8 -*-
import cv2
import time
import os
import sys

def input_arg(argvs, argc):
    if (argc != 3):   # 引数が足りない場合は、その旨を表示
        print 'Usage: # python %s srcdirectory outputdirectory' % argvs[0]
        quit()        # プログラムの終了

    print 'Input file = %s' % argvs[1]
    print 'Output directoryname = %s' % argvs[2]
    # 引数でとったディレクトリの文字列をリターン
    return argvs


def create_directory(output_directory):
    if os.path.isdir(output_directory) == 0:
        print "Not exist \"%s\" folder. So create it." % output_directory
        os.makedirs(output_directory)
    else:
        print "Exist \"%s\" folder." % output_directory


#イメージのパスと名前付けに使う数字のはじめの数字
def facedetect(image_path, output_directory, num):
    #ファイル読み込み
    image = image_path
    #グレースケール変換
    image_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    #カスケード分類器の特徴量を取得する
    cascade = cv2.CascadeClassifier(cascade_path)
    #物体認識（顔認識）の実行
    facerect = cascade.detectMultiScale(image_gray, scaleFactor=1.1, minNeighbors=1, minSize=(250, 250))
    if len(facerect) > 0:
        for rect in facerect:
            # print rect
            x = rect[0]
            y = rect[1]
            w = rect[2]
            h = rect[3]

            # ファイルネームの決定
            img_name = "./" + output_directory + "/test_" + str(num) + '.jpg'
            # 出力窓を調整
            cut_img = image[y-h*0.2:y+h*1.2, x-w*0.2:x+h*1.2]
            cv2.imwrite(img_name, cut_img,(100,100))
            # 出力窓が画像エリアからはみ出て不正な画像ファイルができたときは削除する
            if os.path.getsize(img_name) == 0:
                os.remove(img_name)
                # print "Delete broken file!"
            num += 1
    return num


if __name__ == "__main__":
    # 分類器へのパス
    cascade_path = "/usr/local/share/OpenCV/haarcascades/lbpcascade_animeface.xml"

    argvs = sys.argv   # コマンドライン引数を格納したリストの取得
    argc = len(argvs)  # 引数の個数

    directory = input_arg(argvs, argc)
    video_path = directory[1]
    create_directory(directory[2])

    # 動画ファイル読み込み
    cap = cv2.VideoCapture(video_path)

    frame_num = 0
    save_num = 0
    # フレームごとの処理
    while(cap.isOpened()):
        ret, frame = cap.read()
        if frame_num%10 == 0:
            if (ret == False):
                break
            save_num =facedetect(frame, directory[2], save_num)

        frame_num += 1

    cap.release()