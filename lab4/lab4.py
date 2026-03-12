import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

A = 1.0
freq  = 0.5
phase = 0.0
n_mean = 0.0
n_cov  = 0.1
cutoff = 5.0

t = np.linspace(0, 10, 1000)

noise = np.random.normal(n_mean, np.sqrt(n_cov), len(t))
def harmonic_with_noise(amplitude, frequency, phase,noise_mean, noise_covariance,show_noise, regenerate_noise=False):
    global noise

    y = amplitude * np.sin(2 * np.pi * frequency * t + phase)

    if regenerate_noise:
        noise = np.random.normal(noise_mean, np.sqrt(noise_covariance), len(t))

    if show_noise:
        return y + noise
    else:
        return y

def apply_filter(signal, cutoff_freq):
    fs = 100
    nyq = 0.5 * fs
    normal_cutoff = np.clip(cutoff_freq / nyq, 0.001, 0.999)
    b, a = butter(4, normal_cutoff, btype='low')
    return filtfilt(b, a, signal)
y_noisy    = harmonic_with_noise(A, freq, phase, n_mean, n_cov, True)
y_clean    = A * np.sin(2 * np.pi * freq * t + phase)
y_filtered = apply_filter(y_noisy, cutoff)

fig, ax = plt.subplots(figsize=(10, 5))
plt.subplots_adjust(bottom=0.55)

line_noisy,    = ax.plot(t, y_noisy,    color='orange', alpha=0.6, label='Зашумлена')
line_clean,    = ax.plot(t, y_clean,    color='blue',   lw=2,      label='Чиста')
line_filtered, = ax.plot(t, y_filtered, color='purple', lw=2, linestyle='--', label='Відфільтрована')

ax.set_title('Гармоніка з шумом')
ax.set_xlabel('t')
ax.set_ylabel('y(t)')
ax.legend()
ax.grid(True)

ax_amp  = plt.axes([0.15, 0.42, 0.65, 0.025])
ax_freq = plt.axes([0.15, 0.37, 0.65, 0.025])
ax_ph   = plt.axes([0.15, 0.32, 0.65, 0.025])
ax_nm   = plt.axes([0.15, 0.27, 0.65, 0.025])
ax_nc   = plt.axes([0.15, 0.22, 0.65, 0.025])
ax_cut  = plt.axes([0.15, 0.17, 0.65, 0.025])

s_amp  = Slider(ax_amp,  'Amplitude',         0.1, 3.0,        valinit=A)
s_freq = Slider(ax_freq, 'Frequency',         0.1, 2.0,        valinit=freq)
s_ph   = Slider(ax_ph,   'Phase',             0.0, 2*np.pi,    valinit=phase)
s_nm   = Slider(ax_nm,   'Noise Mean',       -1.0, 1.0,        valinit=n_mean)
s_nc   = Slider(ax_nc,   'Noise Covariance',  0.0, 1.0,        valinit=n_cov)
s_cut  = Slider(ax_cut,  'Cutoff Freq',       0.5, 20.0,       valinit=cutoff)

ax_check = plt.axes([0.75, 0.08, 0.15, 0.06])
check = CheckButtons(ax_check, ['Show Noise'], [True])
show_noise = [True]

ax_btn = plt.axes([0.15, 0.08, 0.1, 0.05])
btn = Button(ax_btn, 'Reset')

def redraw(regen=False):
    global noise
    if regen:
        noise = np.random.normal(s_nm.val, np.sqrt(s_nc.val), len(t))
    y = harmonic_with_noise(s_amp.val, s_freq.val, s_ph.val,
                            s_nm.val, s_nc.val, show_noise[0])
    yc = s_amp.val * np.sin(2 * np.pi * s_freq.val * t + s_ph.val)
    yf = apply_filter(y, s_cut.val)
    line_noisy.set_ydata(y)
    line_clean.set_ydata(yc)
    line_filtered.set_ydata(yf)
    fig.canvas.draw_idle()

def toggle(label):
    show_noise[0] = not show_noise[0]
    redraw()

def reset(event):
    for s in (s_amp, s_freq, s_ph, s_nm, s_nc, s_cut):
        s.reset()
    redraw(regen=True)

for s in (s_amp, s_freq, s_ph, s_cut):
    s.on_changed(lambda v: redraw())
for s in (s_nm, s_nc):
    s.on_changed(lambda v: redraw(regen=True))

check.on_clicked(toggle)
btn.on_clicked(reset)

fig.text(0.28, 0.035,
    "Amplitude/Frequency/Phase — параметри гармоніки\n"
    "Noise Mean/Covariance — параметри шуму (шум перегенерується при зміні)\n"
    "Cutoff Freq — частота зрізу фільтра  |  Show Noise — показати/сховати шум  |  Reset — скинути параметри",
    fontsize=8, color='dimgray', verticalalignment='bottom')

plt.show()