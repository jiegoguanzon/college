import numpy as np
import img_processing as ip
import rvm1_ik as ik
import matplotlib.pyplot as plt

from scipy.spatial import ConvexHull, convex_hull_plot_2d

def get_nearest_circle(circle_index):

    cost = 9999999999
    nearest_circle_index = circle_index

    x = world_circles[circle_index][0]
    y = world_circles[circle_index][1]

    for i in range(len(world_circles)):

        if (i == circle_index):
            continue

        test_x = world_circles[i][0]
        test_y = world_circles[i][1]

        temp_cost = np.sqrt((x - test_x) ** 2 + (y - test_y) ** 2)

        if(temp_cost <= cost):
            cost = temp_cost
            nearest_circle_index = i

    return nearest_circle_index

def get_cost(index):

    cost = 0

    for i in range(len(world_circles)):
        cost += (world_circles[index][0] - world_circles[i][0]) ** 2 + (world_circles[index][1] - world_circles[i][1]) ** 2
    
    print(cost)

    return cost

def convex_hull():

    centers = []

    for i in range(len(world_circles)):

        if not i in present_circles:
            continue
        
        centers.append([world_circles[i][0], world_circles[i][1]])

    print(centers)

    max_cost = 0
    max_cost_index = 0

    if (len(present_circles) <= 1):
        return 0
    elif (len(present_circles) <= 2):
        if get_cost(present_circles[0]) > get_cost(present_circles[1]):
            return 0
        else:
            return 1

    hull = ConvexHull(centers)

    for i in range(len(hull.vertices)):
        print(hull.vertices[i])
        print(centers[hull.vertices[i]][0], centers[hull.vertices[i]][1])

        cost = get_cost(present_circles[hull.vertices[i]])

        if(cost > max_cost):
            max_cost = cost
            max_cost_index = hull.vertices[i]
        
    print(max_cost_index)

    return max_cost_index
        
removed_circles = []
present_circles = [0, 1, 2, 3, 4, 5]
world_circles = ip.img_process('calibrate.jpg', 'snap.jpg')
print(world_circles)

joint_variables = ik.inverse_kinematics(395.6, 282.6, 61.5)

if (joint_variables == -1):
    print("Inverse Kinematics Error.")
else:
    print(np.rad2deg(joint_variables))

for i in range(len(world_circles)):
    nearest_circle_index = get_nearest_circle(i)
    joint_variables[4] = ik.gripper_orientation([world_circles[i][0], world_circles[i][1]], [world_circles[nearest_circle_index][0], world_circles[nearest_circle_index][1]])

for i in range(len(world_circles)):
    index = convex_hull()
    removed_circles.append(present_circles.pop(index))
    print("Removed: ", removed_circles)
    print("Present:", present_circles)




