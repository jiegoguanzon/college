import numpy as np
import img_processing as ip
import rvm1_ik as ik
import matplotlib.pyplot as plt
import cv2

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
    
    #print(cost)

    return cost

def convex_hull():

    centers = []

    for i in range(len(world_circles)):

        if not i in present_circles:
            continue
        
        centers.append([world_circles[i][0], world_circles[i][1]])

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
        #print(hull.vertices[i])
        #print(centers[hull.vertices[i]][0], centers[hull.vertices[i]][1])

        cost = get_cost(present_circles[hull.vertices[i]])

        if(cost > max_cost):
            max_cost = cost
            max_cost_index = hull.vertices[i]
        
    #print(max_cost_index)

    return max_cost_index

def write_commands(joint_variables, prev_joint_variables):

    delta_q1 = joint_variables[0] - prev_joint_variables[0]
    delta_q2 = joint_variables[1] - prev_joint_variables[1]
    delta_q3 = joint_variables[2] - prev_joint_variables[2]
    delta_q4 = joint_variables[3] - prev_joint_variables[3]
    delta_q5 = joint_variables[4] - prev_joint_variables[4]

    command_q1 = -np.round(float(np.rad2deg(delta_q1)), 1)
    command_q2 = np.round(float(np.rad2deg(delta_q2)), 1)
    command_q3 = np.round(float(np.rad2deg(delta_q3)), 1)
    command_q4 = np.round(float(np.rad2deg(delta_q4)), 1)
    command_q5 = np.round(float(np.rad2deg(delta_q5)), 1)

    f.write("MJ {},{},{},{},{}\n".format(command_q1, command_q2, command_q3, command_q4, command_q5))
        
removed_circles = []
present_circles = [0, 1, 2, 3, 4, 5]
world_circles, image_circles, processed_snap = ip.img_process('calibrate.jpg', 'snap.jpg')

print("\nImage Circles:\n{}\n".format(image_circles))
print("World Circles:\n{}".format(world_circles))

pyramid_hgap = 5
pyramid_layer_height = 42
container_raidus = 17.5
pyramid_coordinates = [[380, -(pyramid_hgap + container_raidus), pyramid_layer_height],
                       [380, 0, pyramid_layer_height],
                       [380, pyramid_hgap + container_raidus, pyramid_layer_height],
                       [380, -(pyramid_hgap + container_raidus) / 2, pyramid_layer_height * 2],
                       [380, (pyramid_hgap + container_raidus) / 2, pyramid_layer_height * 2],
                       [380, 0, pyramid_layer_height * 3]]

for i, circle in enumerate(world_circles):

    index = convex_hull()
    removed_circles.append(present_circles.pop(index))

print("\nRemoved Circles: ", removed_circles)
print("Present Circles:", present_circles)

pickup_height = 42
pyramid_index = 0

f = open("rvm1_project3.txt", "w+")
f.write("NT\nOG\nMJ 0,0,0,0,-8\nTI 10\n")

prev_joint_variables = [0.0, 0.0, 0.0, 0.0, 0.0]

for i in removed_circles:

    # Place gripper above object
    joint_variables, phi = ik.inverse_kinematics(world_circles[i][0], world_circles[i][1], 1.5 * pickup_height)
    phi = np.rad2deg(phi)

    print("\nPlace gripper above object.")
    print("Joint Variables: \n{}".format(np.rad2deg(joint_variables)))
    print("phi: {}".format(phi))
    ik.compute_coordinates(joint_variables)

    write_commands(joint_variables, prev_joint_variables)
    prev_joint_variables = joint_variables.copy()

    # Pickup object
    joint_variables, phi = ik.inverse_kinematics(world_circles[i][0], world_circles[i][1], pickup_height)
    phi = np.rad2deg(phi)

    if phi == -90.0:
        nearest_circle_index = get_nearest_circle(i)
        joint_variables[4] = ik.gripper_orientation([world_circles[i][0], world_circles[i][1]], [world_circles[nearest_circle_index][0], world_circles[nearest_circle_index][1]])

    print("\nPick-up object.")
    print("Joint Variables: \n{}".format(np.rad2deg(joint_variables)))
    print("phi: {}".format(phi))
    ik.compute_coordinates(joint_variables)

    processed_snap = ip.draw_arrowed_line(processed_snap, (image_circles[i][0], image_circles[i][1]), image_circles[i][2], joint_variables[4])

    write_commands(joint_variables, prev_joint_variables)
    prev_joint_variables = joint_variables.copy()
    f.write("GC\nTI 10\n")

    # Raise gripper
    joint_variables, phi = ik.inverse_kinematics(world_circles[i][0], world_circles[i][1], 1.5 * pickup_height)
    phi = np.rad2deg(phi)

    print("\nRaise gripper with object.")
    print("Joint Variables: \n{}".format(np.rad2deg(joint_variables)))
    print("phi: {}".format(phi))
    ik.compute_coordinates(joint_variables)

    write_commands(joint_variables, prev_joint_variables)
    prev_joint_variables = joint_variables.copy()

    # Place gripper above pyramid position
    joint_variables, phi = ik.inverse_kinematics(pyramid_coordinates[pyramid_index][0], pyramid_coordinates[pyramid_index][1], 1.5 * pyramid_coordinates[pyramid_index][2])
    phi = np.rad2deg(phi)
    joint_variables[4] = np.deg2rad(90)

    print("\nPlace gripper above pyramid position.")
    print("Joint Variables: \n{}".format(np.rad2deg(joint_variables)))
    print("phi: {}".format(phi))
    ik.compute_coordinates(joint_variables)

    write_commands(joint_variables, prev_joint_variables)
    prev_joint_variables = joint_variables.copy()

    # Place object
    joint_variables, phi = ik.inverse_kinematics(pyramid_coordinates[pyramid_index][0], pyramid_coordinates[pyramid_index][1], pyramid_coordinates[pyramid_index][2])
    phi = np.rad2deg(phi)
    joint_variables[4] = np.deg2rad(90)

    print("\nPlace object on pyramid.")
    print("Joint Variables: \n{}".format(np.rad2deg(joint_variables)))
    print("phi: {}".format(phi))
    ik.compute_coordinates(joint_variables)

    write_commands(joint_variables, prev_joint_variables)
    prev_joint_variables = joint_variables.copy()
    f.write("TI 10\nGO\nTI 10\n")

    # Place gripper above pyramid position
    joint_variables, phi = ik.inverse_kinematics(pyramid_coordinates[pyramid_index][0], pyramid_coordinates[pyramid_index][1], 1.5 * pyramid_coordinates[pyramid_index][2])
    phi = np.rad2deg(phi)
    joint_variables[4] = np.deg2rad(90)

    print("\nPlace gripper above pyramid position.")
    print("Joint Variables: \n{}".format(np.rad2deg(joint_variables)))
    print("phi: {}".format(phi))
    ik.compute_coordinates(joint_variables)

    write_commands(joint_variables, prev_joint_variables)
    prev_joint_variables = joint_variables.copy()

    pyramid_index += 1

cv2.namedWindow('Processed Image', cv2.WINDOW_NORMAL)
cv2.resizeWindow('Processed Image', 885, 420)
cv2.imshow('Processed Image', processed_snap)
cv2.waitKey(0)
cv2.destroyAllWindows()
