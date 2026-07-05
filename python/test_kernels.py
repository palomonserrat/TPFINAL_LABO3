import numpy as np
import matplotlib.pyplot as plt

from kernels import box_blur_kernel, quantize_q15
from frequency_response import (
    plot_kernel,
    plot_frequency_response_2d,
    plot_frequency_cut,
)


def main():
    kernel_size = 5

    kernel_float = box_blur_kernel(kernel_size)
    kernel_q15, kernel_fixed = quantize_q15(kernel_float)

    print("Kernel box blur float:")
    print(kernel_float)

    print()
    print("Suma float:", np.sum(kernel_float))

    print()
    print("Kernel box blur Q1.15 entero:")
    print(kernel_q15)

    print()
    print("Kernel box blur reconstruido desde Q1.15:")
    print(kernel_fixed)

    print()
    print("Suma reconstruida desde Q1.15:", np.sum(kernel_fixed))

    print()
    print("Error de cuantización:")
    print(kernel_fixed - kernel_float)

    plot_kernel(
        kernel_float,
        title=f"Kernel promedio float {kernel_size}x{kernel_size}"
    )

    plot_kernel(
        kernel_fixed,
        title=f"Kernel promedio Q1.15 reconstruido {kernel_size}x{kernel_size}"
    )

    plot_frequency_response_2d(
        kernel_float,
        title=f"Respuesta en frecuencia float - Box Blur {kernel_size}x{kernel_size}"
    )

    plot_frequency_response_2d(
        kernel_fixed,
        title=f"Respuesta en frecuencia Q1.15 - Box Blur {kernel_size}x{kernel_size}"
    )

    plot_frequency_cut(
        kernel_float,
        title=f"Corte central float - Box Blur {kernel_size}x{kernel_size}"
    )

    plot_frequency_cut(
        kernel_fixed,
        title=f"Corte central Q1.15 - Box Blur {kernel_size}x{kernel_size}"
    )

    plt.show()


if __name__ == "__main__":
    main()