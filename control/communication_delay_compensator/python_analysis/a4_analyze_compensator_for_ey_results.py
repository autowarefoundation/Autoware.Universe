import os
import matplotlib.pyplot as plt
import numpy as np

data_path = "/mnt/KinsgtonExternal/ROS2_universe/logs"
# Press the green button in the gutter to run the script.

files = sorted(os.listdir(data_path))
file_list = []
file_names = []

for f in files:
    if f.endswith(".txt"):
        print(f)
        file_list.append(data_path + f)
        file_names.append(f)

    file_list.sort()
    file_names.sort()


def load_numpy(k):
    f0 = data_path + "/" + file_names[k]

    return np.loadtxt(f0)


if __name__ == '__main__':

    for (f, k) in zip(file_names, range(len(file_names))):
        print(f" k: {k}, file_name: {f}")

    time_vec = np.loadtxt(data_path + "/" + "time_vec.txt")

    # Inputs of the delay compensator.
    steering_scale = 0.1
    vel_trg_vec_input = np.loadtxt(data_path + "/" + "vel_trg_vec_input.txt")
    vel_sqr_vec_input = np.loadtxt(data_path + "/" + "vel_sqr_vec_input.txt")
    steer_sin_vec_input = np.loadtxt(data_path + "/" + "steer_sin_vec_input.txt")

    # Outputs of the delay compensator
    ycompensator = np.loadtxt(data_path + "/" + "sim_results_dist_compensator_ey.txt")
    ynonlin_vehicle = np.loadtxt(data_path + "/" + "sim_results_nonlin_vehicle.txt")

    # ## Figure inputs
    # fig, ax = plt.subplots(2, 1)
    # ax[0].plot(time_vec, steer_sin_vec_input * steering_scale, label="steering sent")
    # ax[1].set_title(" Steering State")
    #
    # ax[1].plot(time_vec, vel_trg_vec_input)
    # ax[1].set_title(" Velocity Triangle State")
    #
    # # ax[2].plot(time_vec, vel_sqr_vec_input)
    # # ax[2].set_title(" Velocity Square State")
    # plt.tight_layout()
    # plt.show()

    # ## PLOT VEHICLE
    # fig, ax = plt.subplots(4, 1, figsize=(16, 12))
    # ax[0].plot(time_vec, ynonlin_vehicle[:, 0], label="Lateral error")
    # ax[0].set_title("Vehicle Lateral Error")
    # ax[0].legend()
    #
    # ax[1].plot(time_vec, ynonlin_vehicle[:, 1])
    # ax[1].set_title("Vehicle Heading Error")
    #
    # ax[2].plot(time_vec, ynonlin_vehicle[:, 2], label="vehicle steering state")
    # ax[2].plot(time_vec, ycompensator[:, 0], label="steering filtered")
    # ax[2].set_title("Vehicle Steering State")
    # ax[2].legend()
    #
    # ax[3].plot(time_vec, ynonlin_vehicle[:, 3])
    # ax[3].set_title(" Vehicle Speed")
    #
    # for axx in ax:
    #     axx.grid()
    #
    # plt.tight_layout()
    # # plt.legend()
    # plt.show()

    # Figure OUTPUTS of delay compensator
    fig, ax = plt.subplots(5, 1, figsize=(16, 16))
    ax[0].plot(time_vec, steer_sin_vec_input * steering_scale, label="steering sent")
    ax[0].plot(time_vec, ycompensator[:, 0], label="steering filtered")
    ax[0].set_title("Filtered Input")
    ax[0].legend()

    ax[1].plot(time_vec, ycompensator[:, 1])
    ax[1].set_title(" u-du")

    ax[2].plot(time_vec, ycompensator[:, 2], 'r', label="du")
    ax[2].plot(time_vec, ycompensator[:, 1], label="u-du")
    ax[2].plot(time_vec, ycompensator[:, 0], label="steering filtered")
    ax[2].plot(time_vec, steer_sin_vec_input * steering_scale, label="steering original")
    ax[2].plot(time_vec, ycompensator[:, 1] + ycompensator[:, 2], 'k-.', label="u-du + du = u original")
    ax[2].legend()

    ax[2].set_title("du")

    ax[3].plot(time_vec, -ycompensator[:, 3], label = "-1 *ey from du")
    ax[3].plot(time_vec, ynonlin_vehicle[:, 0], label="vehicle lateral error")
    ax[3].set_title("ey from du")
    ax[3].legend()

    ax[4].plot(time_vec, ycompensator[:, 4], label = "ey_du + ey = ey_without_delay")
    # ax[4].plot(time_vec, ynonlin_vehicle[:, 0], label="Lateral error")
    ax[4].set_title(" Pure ey without delay affect")
    ax[4].legend()

    for axx in ax:
        axx.grid()


    plt.tight_layout()
    # plt.legend()
    plt.show()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
