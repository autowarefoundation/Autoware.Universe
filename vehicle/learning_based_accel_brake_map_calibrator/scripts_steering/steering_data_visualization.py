import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.linear_model import LinearRegression

# Read the data
columns = ["Velocity", "Throttling", "Acceleration_with_pitch_comp"]

# WRITE HERE THE NAME OF THE CSV FILE YOU WANT TO VISUALIZE

df = pd.read_csv("steering_01.csv", usecols=columns)

xdata = df.Velocity
ydata = df.Throttling
zdata = df.Acceleration_with_pitch_comp

# Fit a linear regression model
X = np.column_stack((xdata, ydata))
model = LinearRegression().fit(X, zdata)

x_grid, y_grid = np.meshgrid(
    np.linspace(xdata.min(), xdata.max(), 100),
    np.linspace(ydata.min(), ydata.max(), 100),
)

z_grid = model.predict(np.column_stack((x_grid.ravel(), y_grid.ravel())))
z_grid = z_grid.reshape(x_grid.shape)


fig = plt.figure()
ax = plt.axes(projection="3d")


cmap = plt.get_cmap("Greens")
normalize = plt.Normalize(zdata.min(), zdata.max())
sc = plt.cm.ScalarMappable(cmap=cmap, norm=normalize)
sc.set_array([])

scatter = ax.scatter3D(xdata, ydata, zdata, c=zdata, cmap=cmap, marker="o")

ax.plot_surface(x_grid, y_grid, z_grid, alpha=0.5, cmap=cmap)

ax.set_xlabel("Velocity")
ax.set_zlabel("Acceleration")
ax.set_ylabel("Throttling Output")

cbar = plt.colorbar(sc, ax=ax, label="Acceleration_with_pitch_comp")

plt.show()
