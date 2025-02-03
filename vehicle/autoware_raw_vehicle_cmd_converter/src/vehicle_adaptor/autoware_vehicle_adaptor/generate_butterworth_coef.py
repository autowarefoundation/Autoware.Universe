import numpy as np
from scipy.signal import butter
import yaml

order = 2
sampling_rate = int(1.0 / 0.033)  # Hz
cutoff = 14.5  # Hz

nyquist = 0.5 * sampling_rate
normalized_cutoff = cutoff / nyquist

b, a = butter(order, normalized_cutoff, btype="low", analog=False)

data = {"Butterworth": {"order": order, "cutoff": cutoff, "b": b.tolist(), "a": a.tolist()}}
with open("autoware_vehicle_adaptor/param/butterworth_coef.yaml", "w") as f:
    yaml.dump(data, f)
