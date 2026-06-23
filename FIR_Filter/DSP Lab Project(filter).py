from scipy.signal import firwin

Fs = 20
Fc = 2
N = 10

coeff = firwin(N+1, Fc/(Fs/2))

print(coeff)