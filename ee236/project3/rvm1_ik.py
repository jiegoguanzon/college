import numpy as np
from sympy import *

d1 = 300
d5 = 72
a2 = 250
a3 = 160
a4 = 117
phi = np.deg2rad(-90)

def get_max_reach(p_z, phi):
    pw_z = p_z - d1 - ((d5 + a4) * np.sin(phi))
    theta = np.arcsin(pw_z / (a2 + a3))
    phi = np.deg2rad(phi)
    print(np.rad2deg(theta))
    print(np.rad2deg(phi - theta))
    return ((a2 + a3) * np.cos(theta)) + ((d5 + a4) * np.cos(phi - theta))


def compute_coordinates(joint_variables):

    q1 = joint_variables[0]
    q2 = joint_variables[1]
    q3 = joint_variables[2]
    q4 = joint_variables[3]
    
    x = ((a2 * np.cos(q2)) + (a3 * np.cos(q2 + q3)) + ((d5 + a4) * np.cos(q2 + q3 + q4))) * np.sin(q1)
    y = ((a2 * np.cos(q2)) + (a3 * np.cos(q2 + q3)) + ((d5 + a4) * np.cos(q2 + q3 + q4))) * np.cos(q1)
    z = ((a2 * np.sin(q2)) + (a3 * np.sin(q2 + q3)) + ((d5 + a4) * np.sin(q2 + q3 + q4))) + d1

    print("x: {}\ty: {}\tz: {}".format(x, y, z))


def inverse_kinematics(p_x, p_y, p_z):

    phi = np.deg2rad(-90)

    while (1):
        q1 = np.arctan2(p_x, p_y)

        pw_xy = np.sqrt(p_x ** 2 + p_y ** 2) - ((d5 + a4) * np.cos(phi))
        pw_z = p_z - d1 - ((d5 + a4) * np.sin(phi))

        k = (pw_xy ** 2 + pw_z ** 2 + a2 ** 2 - a3 ** 2) / (2 * a2)
        r = np.sqrt(pw_xy ** 2 + pw_z ** 2)
        p = np.arctan(pw_z / pw_xy)
        q2 = np.arccos (k / r) + p

        c3 = (pw_xy ** 2 + pw_z ** 2 - a2 ** 2 - a3 ** 2) / (2 * a2 * a3)
        s3 = np.sqrt(1 - c3 ** 2)
        #q3 = -np.arctan2(s3, c3)
        q3 = -np.arccos(c3)

        q4 = phi - q2 - q3

        q5 = 0

        if (np.rad2deg(q2) < -30 or np.rad2deg(q2) > 100 or np.rad2deg(q3) > 0 or np.rad2deg(q3) < -110 or np.rad2deg(q4) > 90 or np.rad2deg(q4) < -90 or np.isnan(q2) or np.isnan(q3) or np.isnan(q4)):
            joint_variables = -1
            phi += np.deg2rad(1)
            if phi > 0:
                break
        else:
            joint_variables = [q1, q2, q3, q4, q5]
            break

    if (joint_variables == -1):
        print("Inverse Kinematics Error.")
        exit()

    return joint_variables, phi

def gripper_orientation(circle, nearest_circle):

    prev_theta = np.deg2rad(90)
    learning_rate = 0.30
    epoch = 100

    theta_gd = []
    cost_gd = []

    r1 = 25
    x = Symbol('x')

    gripper_cost_fcn = (1 / ((r1 * cos(x) - (nearest_circle[0] - circle[0])) ** 2 + (r1 * sin(x) - (nearest_circle[1] - circle[1])) ** 2))
    gripper_cost_fcn_der = gripper_cost_fcn.diff(x)

    gripper_cost_fcn = lambdify(x, gripper_cost_fcn)
    gripper_cost_fcn_der = lambdify(x, gripper_cost_fcn_der)

    theta_gd.append(prev_theta)
    cost_gd.append(gripper_cost_fcn(prev_theta))

    for i in range(epoch):
        #theta = prev_theta - learning_rate * gripper_cost_fcn_der(prev_theta, circle[0], circle[1], nearest_circle[0], nearest_circle[1])
        theta = prev_theta - learning_rate * gripper_cost_fcn_der(prev_theta)
        theta_gd.append(theta)
        #cost_gd.append(gripper_cost_fcn(theta, circle[0], circle[1], nearest_circle[0], nearest_circle[1]))
        cost_gd.append(gripper_cost_fcn(theta))

        prev_theta = theta

    #theta = np.arctan2(nearest_circle[1] - circle[1], nearest_circle[0] - circle[0])
    theta = np.arctan((nearest_circle[1] - circle[1]) / (nearest_circle[0] - circle[0]))
    
    q5 = theta_gd[epoch - 1] + theta

    return q5
