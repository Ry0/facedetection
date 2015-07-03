# -*- coding:utf-8 -*-
import numpy
import cv2
cascade_path = '/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml'
image_path = 'Girls-Generation.jpg'
color = (255, 255, 255) #白
#ファイル読み込み
image = cv2.imread(image_path)
#グレースケール変換
image_gray = cv2.cvtColor(image, cv2.cv.CV_BGR2GRAY)
#カスケード分類器の特徴量を取得する
cascade = cv2.CascadeClassifier(cascade_path)
#物体認識（顔認識）の実行
facerect = cascade.detectMultiScale(
                image_gray, scaleFactor=1.1, minNeighbors=1, minSize=(1, 1))
if len(facerect) <= 0:
    exit()
#検出した顔を囲む矩形の作成
# for rect in facerect:
#     cv2.rectangle(image, tuple(rect[0:2]),tuple(rect[0:2]+rect[2:4]), color, thickness=2)
#認識結果の保存
#cv2.imwrite("detected.jpg", image)
i = 1
for rect in facerect:
    #cv2.imwrite('demo.jpg', image[rect])
    print rect
    x = rect[0]
    y = rect[1]
    w = rect[2]
    h = rect[3]
    
    # img[y: y + h, x: x + w]
    img_name = './output/face_' + str(i) + '.jpg'
    cv2.imwrite(img_name, image[y-h*0.3:y+h*1.3, x-w*0.3:x+w*1.3])
    i+=1
