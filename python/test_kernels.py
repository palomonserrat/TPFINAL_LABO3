import numpy as np
import matplotlib.pyplot as plt

from kernels import (
    box_blur_kernel,
    quantize_q15_dc_corrected,
    laplacian_kernel,
    sharpening_kernel,
)

from frequency_response import (
    plot_kernel,
    plot_frequency_response_2d,
    plot_frequency_cut,
)


def main():
    # Cambiar entre:
    # "box_blur"
    # "laplacian"
    # "sharpening"
    filter_name = "box_blur"

    if filter_name == "box_blur":
        kernel_float = box_blur_kernel(size=5)
        kernel_q15, kernel_fixed = quantize_q15_dc_corrected(kernel_float)

        title = "Box Blur 5x5 Q1.15"

        print("Kernel float:")
        print(kernel_float)

        print()
        print("Kernel Q1.15 entero:")
        print(kernel_q15)

        print()
        print("Suma Q1.15:", np.sum(kernel_q15))
        print("Suma reconstruida:", np.sum(kernel_fixed))

    elif filter_name == "laplacian":
        kernel_float = laplacian_kernel()
        kernel_fixed = kernel_float.copy()

        title = "Laplaciano 3x3"

        print("Kernel Laplaciano:")
        print(kernel_fixed)

        print()
        print("Suma de coeficientes:", np.sum(kernel_fixed))

    elif filter_name == "sharpening":
        kernel_float = sharpening_kernel()
        kernel_fixed = kernel_float.copy()

        title = "Sharpening 3x3"

        print("Kernel sharpening:")
        print(kernel_fixed)

        print()
        print("Suma de coeficientes:", np.sum(kernel_fixed))

    else:
        raise ValueError("Filtro no reconocido.")

    plot_kernel(
        kernel_fixed,
        title=f"Kernel espacial - {title}"
    )

    plot_frequency_response_2d(
        kernel_fixed,
        title=f"Respuesta en frecuencia 2D - {title}"
    )

    plot_frequency_cut(
        kernel_fixed,
        title=f"Corte central - {title}"
    )

    plt.show()


if __name__ == "__main__":
    main()