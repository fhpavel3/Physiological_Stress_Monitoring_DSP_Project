import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import windows, freqz

# Parameters
N = 10          # Filter Order
sigma = 2.5     # Standard deviation

# Generate Gaussian window (filter coefficients)
gaussian_taps = windows.gaussian(N + 1, std=sigma)

# Normalize coefficients (unity gain)
coeff_normalized = gaussian_taps / np.sum(gaussian_taps)

# Print coefficients for Arduino
print("  " + ", ".join([f"{x:.4f}" for x in coeff_normalized]))

# Plot 1: Filter Coefficients (Impulse Response)
plt.figure(figsize=(8,4))
plt.stem(range(len(coeff_normalized)), coeff_normalized)
plt.title("Gaussian FIR Filter Coefficients")
plt.xlabel("Tap Index")
plt.ylabel("Amplitude")
plt.grid(True)
plt.tight_layout()
plt.show()

# Plot 2: Frequency Response

w, h = freqz(coeff_normalized, worN=1024)

plt.figure(figsize=(8,4))
plt.plot(w/np.pi, np.abs(h), linewidth=2)
plt.title("Frequency Response of Gaussian FIR Filter")
plt.xlabel("Normalized Frequency (×π rad/sample)")
plt.ylabel("Magnitude")
plt.grid(True)
plt.tight_layout()
plt.show()