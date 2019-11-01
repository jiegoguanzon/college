import numpy as np 

pa_x = float(input("Point Ax: "))
pa_y = float(input("Point Ay: "))
pb_x = float(input("Point Bx: "))
pb_y = float(input("Point By: "))

fault = 0
run = 0

while (1):

    if (fault == 1):
        temp = pb_x
        pb_x = pa_x
        pa_x = temp
        temp = pb_y
        pb_y = pa_y
        pa_y = temp

    fault = 0

    z_write_height = 280.0
    z_retract_height = 290.0

    e_left = [[0.0, 0.0, z_write_height], [40.0, 0.0, z_write_height],
            [0.0, -25.0, z_write_height], [20.0, -25.0, z_write_height],
            [0.0, -50.0, z_write_height], [40.0, -50.0, z_write_height]]

    e_mid = [[50.0, 0.0, z_write_height], [90.0, 0.0, z_write_height],
            [50.0, -25.0, z_write_height], [70.0, -25.0, z_write_height],
            [50.0, -50.0, z_write_height], [90.0, -50.0, z_write_height]]

    e_right = [[100.0, 0.0, z_write_height], [140.0, 0.0, z_write_height],
            [100.0, -25.0, z_write_height], [120.0, -25.0, z_write_height],
            [100.0, -50.0, z_write_height], [140.0, -50.0, z_write_height]]

    d1 = 300
    d5 = 72
    a2 = 250
    a3 = 160
    a4 = 117
    phi = 0

    if (pb_x != pa_x):
        rot_theta = np.arctan2(pb_y - pa_y, pb_x - pa_x)
    else: 
        if (pb_y > pa_y):
            rot_theta = np.radians(90)
        else:
            rot_theta = np.radians(-90)

    print("rot_theta: ", np.degrees(rot_theta))

    rot_matrix = np.array([[np.cos(rot_theta), -np.sin(rot_theta), 0], [np.sin(rot_theta), np.cos(rot_theta), 0], [0, 0, 1]])
    #rot_matrix = np.array([[np.sin(rot_theta), np.cos(rot_theta), 0], [np.cos(rot_theta), -np.sin(rot_theta), 0], [0, 0, 1]])
    trans_matrix = np.array([pa_x, pa_y, 0])

    for i in range(len(e_left)):
        e_left[i] = rot_matrix.dot(e_left[i]) + trans_matrix
        e_mid[i] = rot_matrix.dot(e_mid[i]) + trans_matrix
        e_right[i] = rot_matrix.dot(e_right[i]) + trans_matrix

    retract_points = [[e_right[1][0], e_right[1][1], z_retract_height],
                    [e_right[5][0], e_right[5][1], z_retract_height], 
                    [e_right[2][0], e_right[2][1], z_retract_height],
                    [e_right[3][0], e_right[3][1], z_retract_height],
                    [e_mid[1][0], e_mid[1][1], z_retract_height],
                    [e_mid[5][0], e_mid[5][1], z_retract_height], 
                    [e_mid[2][0], e_mid[2][1], z_retract_height],
                    [e_mid[3][0], e_mid[3][1], z_retract_height],
                    [e_left[1][0], e_left[1][1], z_retract_height],
                    [e_left[5][0], e_left[5][1], z_retract_height], 
                    [e_left[2][0], e_left[2][1], z_retract_height],
                    [e_left[3][0], e_left[3][1], z_retract_height]]

    path = [retract_points[0], e_right[1], e_right[0], e_right[4], e_right[5], retract_points[1], 
            retract_points[3], e_right[3], e_right[2], retract_points[2], 
            retract_points[4], e_mid[1], e_mid[0], e_mid[4], e_mid[5], retract_points[5], 
            retract_points[7], e_mid[3], e_mid[2], retract_points[6], 
            retract_points[8], e_left[1], e_left[0], e_left[4], e_left[5], retract_points[9], 
            retract_points[11], e_left[3], e_left[2], retract_points[10]]

    prev_q1 = 0
    prev_q2 = 0
    prev_q3 = 0
    prev_q4 = 0

    f = open("rvm1_project2.txt", "w+")
    f.write("NT\nOG\nMJ 0,0,0,0,-8\nTI 50\nGC\nTI 10\n")
    f.write("MJ 0,2.3,-2.3,0,0\n")

    prev_q2 = np.radians(2.3)
    prev_q3 = np.radians(-2.3)

    q1 = 0
    q2 = prev_q2
    q3 = prev_q3
    q4 = 0

    x = ((a2 * np.cos(q2)) + (a3 * np.cos(q2 + q3)) + ((d5 + a4) * np.cos(q2 + q3 + q4))) * np.sin(q1)
    y = ((a2 * np.cos(q2)) + (a3 * np.cos(q2 + q3)) + ((d5 + a4) * np.cos(q2 + q3 + q4))) * np.cos(q1)
    z = ((a2 * np.sin(q2)) + (a3 * np.sin(q2 + q3)) + ((d5 + a4) * np.sin(q2 + q3 + q4)))

    print("")
    print("x: ", x)
    print("y: ", y)
    print("z: ", z)

    for point in path:

        q1 = np.arctan2(point[0], point[1])

        pw_xy = np.sqrt(point[0]**2 + point[1]**2) - ((d5 + a4)* np.cos(phi))
        pw_z = point[2] - d1

        c3 = (pw_xy**2 + pw_z**2 - a2**2 - a3**2) / (2 * a2 * a3)
        s3 = np.sqrt(1 - c3**2)
        q3 = -np.arctan2(s3, c3)

        print("")
        print("c3: ", c3)
        print("s3: ", s3)

        q3 = -np.arccos(c3)

        k = (pw_xy**2 + pw_z**2 + a2**2 - a3**2) / (2 * a2)
        r = np.sqrt(pw_xy**2 + pw_z**2)
        p = np.arctan(pw_z / pw_xy)
        q2 = np.arccos (k / r) + p

        q4 = phi - q2 - q3

        if (np.degrees(q2) < -30 or np.degrees(q2) > 100 or np.degrees(q3) > 0 or np.degrees(q3) < -110 or np.degrees(q4) > 90 or np.degrees(q4) < -90):
            fault += 1
            break

        x = ((a2 * np.cos(q2)) + (a3 * np.cos(q2 + q3)) + ((d5 + a4) * np.cos(q2 + q3 + q4))) * np.sin(q1)
        y = ((a2 * np.cos(q2)) + (a3 * np.cos(q2 + q3)) + ((d5 + a4) * np.cos(q2 + q3 + q4))) * np.cos(q1)
        z = ((a2 * np.sin(q2)) + (a3 * np.sin(q2 + q3)) + ((d5 + a4) * np.sin(q2 + q3 + q4)))

        delta_q1 = -q1 - prev_q1
        delta_q2 = q2 - prev_q2
        delta_q3 = q3 - prev_q3
        delta_q4 = q4 - prev_q4

        command_q1 = -np.round(np.degrees(delta_q1), 1)
        command_q2 = np.round(np.degrees(delta_q2), 1)
        command_q3 = np.round(np.degrees(delta_q3), 1)
        command_q4 = np.round(np.degrees(delta_q4), 1)

        f.write("MJ {},{},{},{},0.0\n".format(command_q1, command_q2, command_q3, command_q4))

        print("")
        print(point)
        print("q1: ", np.round(np.degrees(q1), 1))
        print("q2: ", np.round(np.degrees(q2), 1))
        print("q3: ", np.round(np.degrees(q3), 1))
        print("q4: ", np.round(np.degrees(q4), 1))
        print("x: ", x)
        print("y: ", y)
        print("z: ", z)

        prev_q1 = -q1
        prev_q2 = q2
        prev_q3 = q3
        prev_q4 = q4

    if (fault == 0):
        print("Done.")
        break

    run += 1
    
    print("Run: ", run )

    if (run == 2):
        print("Error.")
        break
