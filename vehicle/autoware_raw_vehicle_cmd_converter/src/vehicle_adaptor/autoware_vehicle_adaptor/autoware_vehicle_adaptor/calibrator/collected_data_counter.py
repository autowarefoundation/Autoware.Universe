import numpy as np
from sklearn.linear_model import HuberRegressor


def get_check_list(i, j):
    check_list = []
    check_list.append((i - 1, j))
    check_list.append((i - 1, j - 1))
    check_list.append((i - 1, j + 1))
    check_list.append((i, j - 1))
    check_list.append((i, j + 1))
    check_list.append((i + 1, j))
    check_list.append((i + 1, j - 1))
    check_list.append((i + 1, j + 1))
    return check_list


class CollectedDataCounter:
    def __init__(self):
        self.alpha_for_huber = 1e-7
        self.min_samples_for_outlier_removal = 5
        self.outlier_threshold = 2.0
        self.num_bins = 15
        self.v_min, self.v_max = 0.0, 13.0
        self.a_min, self.a_max = -1.3, 1.3
        self.collected_data_counts = np.zeros((self.num_bins, self.num_bins))
        self.data_point_indices = []
        for i in range(self.num_bins):
            tmp_list = []
            for j in range(self.num_bins):
                tmp_list.append([])
            self.data_point_indices.append(tmp_list)
        self.v_bins = np.linspace(self.v_min, self.v_max, self.num_bins + 1)
        self.a_bins = np.linspace(self.a_min, self.a_max, self.num_bins + 1)

        self.v_bin_centers = (self.v_bins[:-1] + self.v_bins[1:]) / 2
        self.a_bin_centers = (self.a_bins[:-1] + self.a_bins[1:]) / 2
        self.collected_data_bins = []

    def clear(self):
        self.collected_data_counts = np.zeros((self.num_bins, self.num_bins))
        self.data_point_indices = []
        for i in range(self.num_bins):
            tmp_list = []
            for j in range(self.num_bins):
                tmp_list.append([])
            self.data_point_indices.append(tmp_list)
        self.collected_data_bins = []

    def get_data_point(self, v, a):
        v_bin = np.digitize(v, self.v_bins) - 1
        a_bin = np.digitize(a, self.a_bins) - 1
        return v_bin, a_bin

    def add_data_point(self, v, a, index):
        v_bin, a_bin = self.get_data_point(v, a)
        if v_bin < 0:
            v_bin = 0
        if a_bin < 0:
            a_bin = 0
        if v_bin >= self.num_bins:
            v_bin = self.num_bins - 1
        if a_bin >= self.num_bins:
            a_bin = self.num_bins - 1
        self.collected_data_counts[v_bin][a_bin] += 1
        self.data_point_indices[v_bin][a_bin].append(index)
        self.collected_data_bins.append((v_bin, a_bin))

    def calc_weights(self, maximum_weight=0.02):
        weights = []
        for i in range(len(self.collected_data_bins)):
            v_bin, a_bin = self.collected_data_bins[i]
            weights.append(1 / (self.collected_data_counts[v_bin][a_bin] + 1 / maximum_weight))
        return weights

    def outlier_exclusion_by_linear_regression(self, data_input, data_output):
        inlier_indices = []
        for i in range(self.num_bins):
            for j in range(self.num_bins):
                if 0 < i < self.num_bins - 1 and 0 < j < self.num_bins - 1:
                    if len(self.data_point_indices[i][j]) > self.min_samples_for_outlier_removal:
                        check_list = get_check_list(i, j)
                        test_indices = self.data_point_indices[i][j].copy()
                        for i_0, j_0 in check_list:
                            test_indices += self.data_point_indices[i_0][j_0].copy()
                        small_test_indices = self.data_point_indices[i][j].copy()
                        huber = HuberRegressor(alpha=1e-7)
                        huber.fit(
                            np.array(data_input)[test_indices], np.array(data_output)[test_indices]
                        )
                        accel_output_pred = huber.predict(np.array(data_input)[small_test_indices])
                        residual = accel_output_pred - np.array(data_output)[small_test_indices]
                        small_inlier_indices = np.array(small_test_indices)[
                            np.where(np.abs(residual) < self.outlier_threshold * np.std(residual))
                        ].tolist()
                        inlier_indices += small_inlier_indices
                    else:
                        inlier_indices += self.data_point_indices[i][j]
                else:
                    inlier_indices += self.data_point_indices[i][j]
        print("outliers are removed")
        print(f"The sample size has decreased from {len(data_input)} to {len(inlier_indices)}.")
        return inlier_indices

    def get_extracted_indices(self, max_data_num=None):
        if max_data_num is None:
            max_data_num = int(np.median(self.collected_data_counts))
        extracted_indices = []
        for i in range(self.num_bins):
            for j in range(self.num_bins):
                if self.collected_data_counts[i][j] <= max_data_num:
                    extracted_indices.extend(self.data_point_indices[i][j])
                else:
                    random_indices = np.random.choice(
                        self.data_point_indices[i][j], max_data_num, replace=False
                    )
                    extracted_indices.extend(random_indices)
        return extracted_indices
