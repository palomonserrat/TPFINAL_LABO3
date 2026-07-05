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

    Valor real aproximado:
        h_fixed = h_q15 / 2^15

    Devuelve:
        kernel_q15: coeficientes enteros int16
        kernel_fixed: coeficientes reconstruidos como float
    """

    scale = 2**15

    kernel_q15 = np.round(kernel * scale).astype(np.int16)
    kernel_fixed = kernel_q15.astype(np.float32) / scale

    return kernel_q15, kernel_fixed