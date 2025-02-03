import casadi
import matplotlib.pyplot as plt
import numpy as np
import scipy.interpolate


def create_straight_line_course(
    start_vel=0.0,
    terminal_vel=0.0,
    max_vel=11.1,
    trajectory_len=700.0,
    show_flag=True,
    save_path="mpc_straight_line_course_data.csv",
):
    min_acc = -1.0
    max_acc = 1.0
    min_jerk = -0.5
    max_jerk = 0.5
    dpos = 0.01

    # generate total path
    x = 0.0
    y = 0.0
    yaw = 0.0
    steer = 0.0
    total_path = []
    total_path.append(np.array([x, y, yaw, steer]))
    while x < trajectory_len:
        x += dpos
        total_path.append(np.array([x, y, yaw, steer]))

    velocity_limit = np.array([max_vel] * len(total_path))
    total_path = np.array(total_path)

    # start and terminal velocity
    speed_transition_step = min(500, len(total_path) // 2)
    applied_velocity_limit = []

    for i in range(len(total_path)):
        if i <= speed_transition_step:
            applied_velocity_limit.append(max_vel * i / speed_transition_step)
        elif i >= len(total_path) - speed_transition_step - 1:
            applied_velocity_limit.append(
                max_vel * (len(total_path) - i - 1) / speed_transition_step
            )
        else:
            applied_velocity_limit.append(np.inf)
    applied_velocity_limit = np.array(applied_velocity_limit)
    velocity_limit = np.stack([velocity_limit, applied_velocity_limit]).min(axis=0)

    lambda_1 = 1e-4
    lambda_2 = 1e-8
    n = len(velocity_limit)
    v = casadi.SX.sym("v", n)
    w = [v]
    lbw = [0] * n
    ubw = velocity_limit.tolist()

    g = []
    lbg = []
    ubg = []
    J = -(v.T @ v)
    print("check 0")
    for i in range(n - 1):
        a = (
            0.5 * (v[i + 1] * v[i + 1] - v[i] * v[i]) / dpos
        )  # (v[i+1] - v[i]) / (dpos/ (0.5 * (v[i+1] + v[i])))
        J += lambda_1 * (v[i + 1] - v[i]) * (v[i + 1] - v[i]) / (dpos * dpos)
        g.append(a)
        lbg.append(min_acc)
        ubg.append(max_acc)
    for i in range(n - 2):
        jerk = (
            0.25
            * (v[i + 2] * v[i + 2] - 2 * v[i + 1] * v[i + 1] + v[i] * v[i])
            * (v[i] + v[i + 1])
            / (dpos * dpos)
        )
        J += (
            lambda_2
            * (v[i + 2] - 2 * v[i + 1] + v[i])
            * (v[i + 2] - 2 * v[i + 1] + v[i])
            / (dpos * dpos * dpos * dpos)
        )
        g.append(jerk)
        lbg.append(min_jerk)
        ubg.append(max_jerk)

    print("check 1")
    nlp = {"f": J, "x": casadi.vertcat(*w), "g": casadi.vertcat(*g)}
    print("check 2")
    solver = casadi.nlpsol("S", "ipopt", nlp)
    print("check 3")
    sol = solver(x0=([0] * n), lbx=lbw, ubx=ubw, lbg=lbg, ubg=ubg)

    v_sol = sol["x"].toarray().flatten()
    v_sol[0] = start_vel
    v_sol[-1] = terminal_vel
    for i in range(min(int(1.0 / dpos), v_sol.shape[0] // 2)):
        v_sol[i] = min(
            max(v_sol[i], np.sqrt(start_vel * start_vel + 2 * i * dpos * 0.1 * max_acc)),
            v_sol[int(1.0 / dpos)],
        )
        v_sol[-i - 1] = min(
            max(v_sol[-i - 1], np.sqrt(terminal_vel * terminal_vel + 2 * i * dpos * 0.1 * max_acc)),
            v_sol[-int(1.0 / dpos) - 1],
        )
    fig, axes = plt.subplots(nrows=1, ncols=3, figsize=(18, 5), tight_layout=True)
    fig.suptitle("smoothed velocity plots")
    axes[0].plot(
        dpos * np.arange(total_path.shape[0]), velocity_limit, label="final velocity limit"
    )
    axes[0].plot(dpos * np.arange(total_path.shape[0]), v_sol, label="smoothing result")
    axes[0].set_ylim([-1, max_vel + 1])
    axes[0].set_xlabel("travel distance [m]")
    axes[0].set_ylabel("longitudinal velocity [m/s]")
    axes[1].set_title("total velocity path")
    axes[1].plot(dpos * np.arange(total_path.shape[0]), v_sol, label="smoothing result")
    axes[1].set_xlim([0.0, 0.1])
    axes[1].set_ylim([-0.2, 0.2])
    axes[1].set_xlabel("travel distance [m]")
    axes[1].set_ylabel("longitudinal velocity [m/s]")
    axes[1].set_title("velocity path around start point")
    axes[2].plot(dpos * np.arange(total_path.shape[0]), v_sol, label="smoothing result")
    axes[2].set_xlim([dpos * (total_path.shape[0] - 1) - 0.1, dpos * (total_path.shape[0] - 1)])
    axes[2].set_ylim([-0.2, 0.2])
    axes[2].set_xlabel("travel distance [m]")
    axes[2].set_ylabel("longitudinal velocity [m/s]")
    axes[2].set_title("velocity path around end point")
    plt.show()

    # total path before interpolation

    dt_path = 2 * dpos / (v_sol[1:] + v_sol[:-1])
    timestamp = []
    xy_path = []
    v_path = []
    yaw_path = []
    steer_path = []
    t = 0
    for i in range(len(dt_path)):
        timestamp.append(1.0 * t)
        xy_path.append(total_path[i, [0, 1]])
        v_path.append(v_sol[i])
        yaw_path.append(total_path[i, 2])
        steer_path.append(total_path[i, -1])
        if i == len(dt_path) - 1:
            t += 10.0
            timestamp.append(1.0 * t)
            xy_path.append(total_path[i, [0, 1]])
            v_path.append(v_sol[i])
            yaw_path.append(total_path[i, 2])
            steer_path.append(total_path[i, -1])
        t += dt_path[i]
    timestamp = np.array(timestamp)
    xy_path = np.array(xy_path)
    v_path = np.array(v_path)
    yaw_path = np.array(yaw_path)
    steer_path = np.array(steer_path)

    a_path = (v_path[1:] - v_path[:-1]) / (timestamp[1:] - timestamp[:-1])
    a_path = np.hstack([a_path, a_path[-1]])
    jerk_path = (a_path[1:] - a_path[:-1]) / (timestamp[1:] - timestamp[:-1])
    jerk_path = np.hstack([jerk_path, jerk_path[-1]])
    dsteer_path = (steer_path[1:] - steer_path[:-1]) / (timestamp[1:] - timestamp[:-1])
    dsteer_path = np.hstack([dsteer_path, dsteer_path[-1]])

    trajectory_data = np.zeros((timestamp.shape[0], 9))
    trajectory_data[:, 0] = timestamp
    trajectory_data[:, [1, 2]] = xy_path
    trajectory_data[:, 3] = v_path
    trajectory_data[:, 4] = yaw_path
    trajectory_data[:, 5] = a_path
    trajectory_data[:, 6] = steer_path
    trajectory_data[:, 7] = jerk_path
    trajectory_data[:, 8] = dsteer_path
    fig, axes = plt.subplots(nrows=2, ncols=4, figsize=(18, 5), tight_layout=True)
    fig.suptitle("total path before interpolation")
    title_name_list = ["x", "y", "vel", "yaw", "acc", "steer", "jerk", "dsteer"]
    unit_name_list = ["[m]", "[m]", "[m/s]", "[rad]", "[m/s^2]", "[rad]", "[m/s^3]", "[rad/s]"]
    for i in range(8):
        axes[int(i / 4), i % 4].plot(trajectory_data[:, 0], trajectory_data[:, i + 1])
        axes[int(i / 4), i % 4].set_title(title_name_list[i] + "_path")
        axes[int(i / 4), i % 4].set_xlabel("Time [s]")
        axes[int(i / 4), i % 4].set_ylabel(title_name_list[i] + " " + unit_name_list[i])
    plt.show()

    # interpolation
    timestamp_interp = []
    diff_step_for_jerk = 20
    min_dt = 0.05
    for i in range(timestamp.shape[0]):
        timestamp_interp.append(timestamp[i])
        if i == timestamp.shape[0] - 1:
            continue
        else:
            while timestamp_interp[-1] + min_dt < timestamp[i + 1]:
                timestamp_interp.append(timestamp_interp[-1] + min_dt)
    timestamp_interp = np.array(timestamp_interp)
    trajectory_interpolator_list = [
        scipy.interpolate.interp1d(trajectory_data[:, 0], trajectory_data[:, 1 + i])
        for i in range(trajectory_data.shape[1] - 1)
    ]
    trajectory_interp = np.zeros((timestamp_interp.shape[0], 9))
    trajectory_interp[:, 0] = timestamp_interp
    for i in range(8):
        trajectory_interp[:, i + 1] = trajectory_interpolator_list[i](timestamp_interp)
    trajectory_interp[:-1, 5] = (trajectory_interp[1:, 3] - trajectory_interp[:-1, 3]) / (
        timestamp_interp[1:] - timestamp_interp[:-1]
    )
    trajectory_interp[-1, 5] = trajectory_interp[-2, 5]
    trajectory_interp[diff_step_for_jerk:-diff_step_for_jerk, 7] = (
        trajectory_interp[2 * diff_step_for_jerk :, 5]
        - trajectory_interp[: -2 * diff_step_for_jerk, 5]
    ) / (timestamp_interp[2 * diff_step_for_jerk :] - timestamp_interp[: -2 * diff_step_for_jerk])
    trajectory_interp[:-1, 8] = (trajectory_interp[1:, 6] - trajectory_interp[:-1, 6]) / (
        timestamp_interp[1:] - timestamp_interp[:-1]
    )
    trajectory_interp[-1, 8] = trajectory_interp[-2, 6]

    fig, axes = plt.subplots(nrows=2, ncols=4, figsize=(18, 5), tight_layout=True)
    fig.suptitle("total path after interpolation")
    for i in range(8):
        axes[int(i / 4), i % 4].plot(trajectory_interp[:, 0], trajectory_interp[:, i + 1])
        axes[int(i / 4), i % 4].set_title(title_name_list[i] + "_path")
        axes[int(i / 4), i % 4].set_xlabel("Time [s]")
        axes[int(i / 4), i % 4].set_ylabel(title_name_list[i] + " " + unit_name_list[i])
    plt.show()
    np.savetxt(save_path, trajectory_interp, delimiter=",")
