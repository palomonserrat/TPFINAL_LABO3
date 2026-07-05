import numpy as np
import matplotlib.pyplot as plt


def compute_frequency_response_2d(kernel: np.ndarray, fft_size: int = 512):
    """
    Calcula la respuesta en frecuencia 2D de un kernel usando FFT.

    El kernel se zero-paddea hasta fft_size x fft_size.
    """

    H = np.fft.fft2(kernel, s=(fft_size, fft_size))
    H_shifted = np.fft.fftshift(H)

    magnitude = np.abs(H_shifted)
    magnitude_db = 20 * np.log10(magnitude + 1e-12)

    phase = np.angle(H_shifted)

    return magnitude, magnitude_db, phase


def plot_kernel(kernel: np.ndarray, title: str = "Kernel espacial"):
    """
    Grafica el kernel en el dominio espacial.
    """

    plt.figure()
    plt.imshow(kernel, cmap="gray")
    plt.colorbar(label="Coeficiente")
    plt.title(title)
    plt.xlabel("n")
    plt.ylabel("m")
    plt.tight_layout()


def plot_frequency_response_2d(kernel: np.ndarray,
                               title: str = "Respuesta en frecuencia 2D",
                               fft_size: int = 512):
    """
    Grafica la magnitud de la respuesta en frecuencia 2D en dB.
    """

    _, magnitude_db, _ = compute_frequency_response_2d(kernel, fft_size)

    plt.figure()
    plt.imshow(
        magnitude_db,
        cmap="jet",
        extent=[-np.pi, np.pi, -np.pi, np.pi],
        origin="lower"
    )
    plt.colorbar(label="Magnitud [dB]")
    plt.title(title)
    plt.xlabel(r"$\omega_x$ [rad/muestra]")
    plt.ylabel(r"$\omega_y$ [rad/muestra]")
    plt.tight_layout()


def plot_frequency_cut(kernel: np.ndarray,
                       title: str = "Corte central de la respuesta en frecuencia",
                       fft_size: int = 512):
    """
    Grafica un corte horizontal de la respuesta en frecuencia.

    Es decir, muestra H(wx, wy=0).
    """

    _, magnitude_db, _ = compute_frequency_response_2d(kernel, fft_size)

    center = fft_size // 2
    cut = magnitude_db[center, :]

    w = np.linspace(-np.pi, np.pi, fft_size)

    plt.figure()
    plt.plot(w, cut)
    plt.grid(True)
    plt.title(title)
    plt.xlabel(r"$\omega_x$ [rad/muestra]")
    plt.ylabel("Magnitud [dB]")
    plt.tight_layout()