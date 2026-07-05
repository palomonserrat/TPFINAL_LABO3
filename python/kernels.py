import numpy as np


def box_blur_kernel(size: int = 5) -> np.ndarray:
    """
    Crea un kernel promedio size x size en punto flotante.
    """

    if size <= 0:
        raise ValueError("El tamaño del kernel debe ser positivo.")

    if size % 2 == 0:
        raise ValueError("El tamaño del kernel debe ser impar.")

    kernel = np.ones((size, size), dtype=np.float32)
    kernel = kernel / np.sum(kernel)

    return kernel


def quantize_q15(kernel: np.ndarray):
    """
    Cuantiza un kernel float a formato Q1.15.
    """

    scale = 2**15

    kernel_q15 = np.round(kernel * scale).astype(np.int16)
    kernel_fixed = kernel_q15.astype(np.float32) / scale

    return kernel_q15, kernel_fixed


def quantize_q15_dc_corrected(kernel: np.ndarray):
    """
    Cuantiza a Q1.15 y corrige la suma para que la ganancia DC sea exactamente 1.
    """

    scale = 2**15

    kernel_q15 = np.round(kernel * scale).astype(np.int32)

    error = scale - np.sum(kernel_q15)

    center_row = kernel_q15.shape[0] // 2
    center_col = kernel_q15.shape[1] // 2

    kernel_q15[center_row, center_col] += error

    kernel_fixed = kernel_q15.astype(np.float32) / scale

    return kernel_q15.astype(np.int16), kernel_fixed


def laplacian_kernel() -> np.ndarray:
    """
    Crea un kernel Laplaciano 3x3.

    Este filtro es pasa-altos espacial.
    Resalta bordes y cambios bruscos de intensidad.
    """

    kernel = np.array([
        [0, -1, 0],
        [-1, 4, -1],
        [0, -1, 0]
    ], dtype=np.float32)

    return kernel