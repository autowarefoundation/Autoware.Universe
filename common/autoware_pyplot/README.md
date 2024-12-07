# autoware_pyplot

This package provides C++ interface for the notable `matplotlib` using `pybind11` backend for

- creating scientific plots and images illustrating the function inputs/outputs
- debugging the output and internal data of a function before unit testing in a more lightweight manner than planning_simulator

## usage

In your main function, setup the python context and import `matplotlib`

```cpp
#include <autoware/pyplot/pyplot.hpp>
#include <pybind11/embed.h>

// in main...
  py::scoped_interpreter guard{};
  auto plt = autoware::pyplot::import();
```

Then you can use major functionalities of `matplotlib` almost in the same way as native python code.

```cpp
{
    plt.plot(Args(std::vector<int>({1, 3, 2, 4})), Kwargs("color"_a = "blue", "linewidth"_a = 1.0));
    plt.xlabel(Args("x-title"));
    plt.ylabel(Args("y-title"));
    plt.title(Args("title"));
    plt.xlim(Args(0, 5));
    plt.ylim(Args(0, 5));
    plt.grid(Args(true));
    plt.savefig(Args("test_single_plot.png"));
}

{
    auto [fig, axes] = plt.subplots(1, 2);
    auto & ax1 = axes[0];
    auto & ax2 = axes[1];

    ax1.set_aspect(Args("equal"));
    ax2.set_aspect(Args("equal"));
}
```
