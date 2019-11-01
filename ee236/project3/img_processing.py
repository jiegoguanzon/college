import numpy as np
import cv2
from matplotlib import pyplot as plt

def img_process(calibrate_img, snap_img):
    container_height = 42
    container_diameter = 35

    calibrate_img = cv2.imread(calibrate_img, cv2.IMREAD_GRAYSCALE)
    snap_img = cv2.imread(snap_img, cv2.IMREAD_GRAYSCALE)

    block_size = 11

    #filtered_calibrate = calibrate_img
    #filtered_calibrate = cv2.GaussianBlur(calibrate_img, (9,9), 0)
    #filtered_calibrate = cv2.bilateralFilter(calibrate_img, block_size, 200, 200)
    filtered_calibrate = cv2.medianBlur(calibrate_img, block_size)

    #filtered_snap = snap_img
    #filtered_snap = cv2.GaussianBlur(snap_img, (9,9), 0)
    #filtered_snap = cv2.bilateralFilter(snap_img, block_size, 200, 200)
    filtered_snap = cv2.medianBlur(snap_img, block_size)

    processed_calibrate = cv2.cvtColor(filtered_calibrate, cv2.COLOR_GRAY2BGR)
    calibrate_circle = cv2.HoughCircles(filtered_calibrate, cv2.HOUGH_GRADIENT, 1, 20, param1=50, param2=30, minRadius=0, maxRadius=0)
    #calibrate_circle = np.uint16(np.around(calibrate_circle))

    img_x = calibrate_circle[0][0][0]
    img_y = calibrate_circle[0][0][1]
    img_diameter = calibrate_circle[0][0][2]

    img2world_ratio = container_diameter / (img_diameter * 2)
    #img2world_ratio = 380 / img_y
    #print(img2world_ratio)

    cv2.circle(processed_calibrate, (img_x, img_y), img_diameter, (0, 255, 0), 2)
    cv2.circle(processed_calibrate, (img_x, img_y), 2, (0, 0, 255), 3)

    #print(calibrate_circle)

    cv2.imshow('Calibration Image', processed_calibrate)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    processed_snap = cv2.cvtColor(filtered_snap, cv2.COLOR_GRAY2BGR)
    snap_circles = cv2.HoughCircles(filtered_snap, cv2.HOUGH_GRADIENT, 1, 20, param1=50, param2=30, minRadius=0, maxRadius=0)
    #calibrate_circle = np.uint16(np.around(calibrate_circle))

    for i in snap_circles[0, :]:
        cv2.circle(processed_snap, (i[0], i[1]), i[2], (0, 255, 0), 2)
        cv2.circle(processed_snap, (i[0], i[1]), 2, (0, 0, 255), 3)
        cv2.imshow('Calibration Image', processed_snap)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    #print(snap_circles)

    cv2.imshow('Calibration Image', processed_snap)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    world_circles = snap_circles
    #print(world_circles[0])

    for i in range(len(world_circles[0])):
        img2world_ratio = container_diameter / (world_circles[0][i][2] * 2)
        world_circles[0][i][0] = (img_x - world_circles[0][i][0]) * img2world_ratio
        world_circles[0][i][1] = (world_circles[0][i][1] - img_y) * img2world_ratio + 380

    #print(world_circles[0])

    return world_circles[0]
