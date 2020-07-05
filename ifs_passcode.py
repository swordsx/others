# -*- coding: utf-8 -*-

import cv2
import numpy as np
from numpy import dot
from numpy.linalg import norm
import os
import ssl
import csv
ssl._create_default_https_context = ssl._create_unverified_context
from urllib.request import urlretrieve
import requests

IFS_NAME = "ifsbj202007"           #<--------need to modify
DEFAULT_BG_PX = np.array([50, 50, 50])
BG_DIS_THRES = 300 ** 0.5

def L2dis(a, b):
    return norm(a - b)

def px_is_bg(px):
    return L2dis(px, DEFAULT_BG_PX) < BG_DIS_THRES

def download_portal_image(csv_filename, download_dir):
    csv_file = open(csv_filename, "r+", encoding="gbk")
    csv_reader = csv.reader(csv_file)
    ssl._create_default_https_context = ssl._create_unverified_context
    print("downloading portal image files...")
    os.system('rm -rf %s' %(download_dir))
    os.system("mkdir %s" %(download_dir))
    cnt = 0
    for row in csv_reader:
        img_url = row[3]
        img_id = img_url.split("/")[-1]
        if img_url == " Image" or img_url == "":
            continue
        proxies = {
            #图片在googleusercontent上，需要代理。此处对应trojan的http本地端口和代理
            "http": "http://127.0.0.1:1081",
        }
        r = requests.get(img_url, proxies=proxies)
        with open("%s/%s.jpg" %(download_dir, img_id), "wb") as f:
            f.write(r.content)
        portal_name = row[0].replace("%u", "\\u").encode('utf-8').decode('unicode_escape')
        cnt += 1
        print("downloading image of %s (#%d)" % (portal_name, cnt))
    print("downloading done!")
    csv_file.close()
    csv_filename_new = "%s_%s.csv" %(csv_filename.split(".")[0], IFS_NAME)
    os.system("cp %s %s/%s" %(csv_filename, download_dir, csv_filename_new))


def read_dump_data(csv_filename, download_dir):
    csv_file = open("%s/%s" %(download_dir, csv_filename), "r+", encoding="gbk")
    dump_data = {}
    csv_reader = csv.reader(csv_file)
    print("reading dump data...")
    for row in csv_reader:
        portal_name = row[0].replace("%u", "\\u").encode('utf-8').decode('unicode_escape')
        lat = row[1]
        lng = row[2]
        img_url = row[3]
        if img_url == " Image" or img_url == "":
            continue
        img_id = img_url.split("/")[-1]
        img = cv2.imread("%s/%s.jpg" %(download_dir, img_id))
        tup = (portal_name, lat, lng, img)
        dump_data[img_id] = tup
    print("reading done!")
    return dump_data

def whole_row_bg(img, row):
    n_col = len(img[0])
    for j in range(n_col):
        px = img[row][j]
        if not px_is_bg(px):
            return False
    return True

def whole_col_bg(img, col):
    n_row = len(img)
    for i in range(n_row):
        px = img[i][col]
        if not px_is_bg(px):
            return False
    return True

def split_img_into_columns(ifs_img_filename, sub_img_dir):
    img = cv2.imread(ifs_img_filename)

    px_rows = len(img)
    px_cols = len(img[0])

    os.system('rm -rf %s' %(sub_img_dir))
    os.system("mkdir %s" %(sub_img_dir))

    col_split_points = []   #默认间隔全部为500
    
    print("finding col split points...")


    for i in range(px_rows):
        px = img[i][0]
        if not px_is_bg(px):
            #找到最左上第一张图，剪掉第一行图上方的标题(默认高度为100)
            img = img[i:, :]
            px_rows -= i
            break
    last_col_is_bg = True
    
    for j in range(px_cols):
        if j % 100 == 0:
            print("progress: %d/%d" %(j, px_cols))
        col_is_bg = True
        
        
        fg_cnt_thres = 0.005 * px_rows
        fg_px_cnt = 0
        for i in range(px_rows):
            px = img[i][j]
            if not px_is_bg(px):
                fg_px_cnt += 1
            if fg_px_cnt > fg_cnt_thres:
                col_is_bg = False
                break
        
        #col_is_bg = whole_col_bg(img, j)
        if last_col_is_bg == True and col_is_bg == False:
            col_split_points.append(j)
        last_col_is_bg = col_is_bg
    col_split_points.append(px_cols)
    print("splitting points:", col_split_points)
    
    sub_img_cols = len(col_split_points) - 1
    for i in range(1, len(col_split_points)):
        row_ending = px_rows
        while px_is_bg(img[row_ending - 1][col_split_points[i-1]]):
            row_ending -= 1    #去掉每列最下面多余的背景
        sub_img_col = img[:row_ending, col_split_points[i-1] : col_split_points[i]]
        cv2.imwrite("./%s/%s_%d.jpg" %(sub_img_dir, ifs_img_filename, i), sub_img_col)

    return sub_img_cols

def split_img_into_grid(ifs_img_filename, sub_img_dir, sub_img_cols):
    sub_imgs = [[] for i in range(sub_img_cols)]
    for i in range(sub_img_cols):
        img = cv2.imread("./%s/%s_%d.jpg" %(sub_img_dir, ifs_img_filename, i + 1))
        up = 0
        print("splitting ./%s/%s_%d.jpg" %(sub_img_dir, ifs_img_filename, i + 1))
        px_rows = len(img)
        while True:
            left = right = 0
            down = up
            while down < px_rows and not whole_row_bg(img, down):
                down += 1   #下边界
            sub_img = img[up:down, :]
            while not whole_col_bg(sub_img, right):
                right += 1   #右边界

            sub_img = sub_img[:, left:right]
            #print(up, down, left, right)
            sub_imgs[i].append(sub_img)
            cv2.imwrite("./%s/%s_%d_%d.jpg" %(sub_img_dir, ifs_img_filename, i + 1, len(sub_imgs[i])), sub_img)

        
            up = down + 10    #magic number，好多图片上面有两像素的红色谜之区域导致难以处理
            if up >= px_rows:
                break

    return sub_imgs
    

def calculate(image1, image2):
    # 灰度直方图算法
    # 计算单通道的直方图的相似值
    hist1 = cv2.calcHist([image1], [0], None, [256], [0.0, 255.0])
    hist2 = cv2.calcHist([image2], [0], None, [256], [0.0, 255.0])
    # 计算直方图的重合度
    degree = 0
    for i in range(len(hist1)):
        if hist1[i] != hist2[i]:
            degree = degree + \
                (1 - abs(hist1[i] - hist2[i]) / max(hist1[i], hist2[i]))
        else:
            degree = degree + 1
    degree = degree / len(hist1)
    return degree


def classify_hist_with_split(image1, image2, size=(256, 256)):
    # RGB每个通道的直方图相似度
    # 将图像resize后，分离为RGB三个通道，再计算每个通道的相似值
    image1 = cv2.resize(image1, size)
    image2 = cv2.resize(image2, size)
    sub_image1 = cv2.split(image1)
    sub_image2 = cv2.split(image2)
    sub_data = 0
    for im1, im2 in zip(sub_image1, sub_image2):
        sub_data += calculate(im1, im2)
    sub_data = sub_data / 3
    return sub_data

'''
def calculate_1(image1, image2_id, i, hists_dump_img):
    # 灰度直方图算法
    # 计算单通道的直方图的相似值
    hist1 = cv2.calcHist([image1], [0], None, [256], [0.0, 255.0])
    hist2 = hists_dump_img[image2_id][i]
    # 计算直方图的重合度
    degree = 0
    for i in range(len(hist1)):
        if hist1[i] != hist2[i]:
            degree = degree + \
                (1 - abs(hist1[i] - hist2[i]) / max(hist1[i], hist2[i]))
        else:
            degree = degree + 1
    degree = degree / len(hist1)
    return degree


def classify_hist_with_split_1(image1, image2_id, hists_dump_img, size=(256, 256)):
    # RGB每个通道的直方图相似度
    # 将图像resize后，分离为RGB三个通道，再计算每个通道的相似值
    image1 = cv2.resize(image1, size)
    sub_image1 = cv2.split(image1)
    sub_data = 0
    for i in range(3):
        sub_data += calculate_1(sub_image1[i], image2_id, i, hists_dump_img)
    sub_data = sub_data / 3
    return sub_data
'''
def calculate_hist_dump_img(dump_data):
    #提前算好，避免重复计算
    #……好像并没有变快
    size = (256,256)
    hists_dump_img = {}
    for img_id in dump_data:
        img = dump_data[img_id][3]
        img = cv2.resize(img, size)
        hists_dump_img[img_id] = []
        for channel in cv2.split(img):
            hist = cv2.calcHist([img], [0], None, [256], [0.0, 255.0])
            hists_dump_img[img_id].append(hist)
    return hists_dump_img


def search_in_dump(ifs_img_filename, sub_img_dir, sub_imgs, sub_img_cols, dump_data):
    search_results = [[] for i in range(sub_img_cols)]

    #hists_dump_img = calculate_hist_dump_img(dump_data)
    
    for i in range(sub_img_cols):
        for j in range(len(sub_imgs[i])):
            sub_img = sub_imgs[i][j]
            similarities = {}
            for dump_img_id in dump_data:
                
                dump_img = dump_data[dump_img_id][3]
                sim = classify_hist_with_split(sub_img, dump_img)
                
                #sim = classify_hist_with_split_1(sub_img, dump_img_id, hists_dump_img)
                
                similarities[dump_img_id] = sim
            TOPN = 5
            print("Top 5 search result for %s_%d_%d.jpg:" %(ifs_img_filename, i + 1, j + 1))
            topn_sims = sorted(similarities.items(), key=lambda x:x[1], reverse=True)[:TOPN]
            for (dump_img_id, sim) in topn_sims:
                #存一下前五是因为万一有算错的...
                print("portal name:%s, sim:%f" %(dump_data[dump_img_id][0], sim))
            search_results[i].append(topn_sims[0][0])
    return search_results

def display_search_results(search_results, dump_data):
    '''
    https://intel.ingress.com/intel?ll=45.717403,126.636646&z=17&pll=45.717403,126.636646
    '''
    for i in range(len(search_results)):
        print("search result for col %d" %(i + 1))
        for j in range(len(search_results[i])):
            img_id = search_results[i][j]
            portal_name = dump_data[img_id][0]
            lat = dump_data[img_id][1]
            lng = dump_data[img_id][2]
            print("https://intel.ingress.com/intel?ll=%s,%s&z=17&pll=%s,%s\t%s"%(lat, lng, lat, lng, portal_name))
        print("")

def generate_draw_items(search_results, dump_data):
    for i in range(len(search_results)):
        draw_item = "["
        for j in range(1, len(search_results[i])):
            last = search_results[i][j - 1]
            cur = search_results[i][j]

            
            lat_last = dump_data[last][1]
            lng_last = dump_data[last][2]

            lat_cur = dump_data[cur][1]
            lng_cur = dump_data[cur][2]
            polyline = '{"type":"polyline","latLngs":[{"lat":%s,"lng":%s},''{"lat":%s,"lng":%s}],"color":"#a24ac3"},' \
                       %(lat_last, lng_last, lat_cur, lng_cur)
            draw_item += polyline
        draw_item += "{}]"
        print("draw item for col %d:\n%s\n" %(i, draw_item))



if __name__ == "__main__":
    csv_filename = "Portal_Export.csv"
    download_dir = "dump_img_%s" %(IFS_NAME)
    download_portal_image(csv_filename, download_dir)       #<----下载很慢，加注释跳过
    
    csv_filename_new = "%s_%s.csv" %(csv_filename.split(".")[0], IFS_NAME)
    csv_filename = csv_filename_new

    dump_data = read_dump_data(csv_filename, download_dir)

    ifs_img_filename = "%s.jpg"%(IFS_NAME)
    sub_img_dir = "sub_img_%s" %(IFS_NAME)
    sub_img_cols = split_img_into_columns(ifs_img_filename, sub_img_dir)
    #sub_img_cols = 1     #for debug
    sub_imgs = split_img_into_grid(ifs_img_filename, sub_img_dir, sub_img_cols)
    search_results = search_in_dump(ifs_img_filename, sub_img_dir, sub_imgs, sub_img_cols, dump_data)
    display_search_results(search_results, dump_data)
    generate_draw_items(search_results, dump_data)




def find_background_px():
    import collections
    counter = collections.defaultdict(int)
    for i in range(px_rows):
        if i % 100 == 0:
            print(i)
        for j in range(px_cols):
            px = img[i][j]
            px_str = (px[0] << 16) + (px[1] << 8) + px[2]
            counter[px_str] += 1
    for tup in sorted(counter.items(), key=lambda x:x[1], reverse=True)[:100]:
        print(tup[0] >> 16, (tup[0] >> 8) & 255, tup[0] & 255, tup[1])
