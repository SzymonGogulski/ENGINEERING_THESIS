import librosa
import librosa.display
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from scipy.signal import get_window


def plot_wav(wav_path, sr):
    # Load the audio file
    y, sr = librosa.load(wav_path, sr=sr)

    # Create a time axis in seconds based on the sample rate and number of samples
    time = np.linspace(0, len(y) / sr, len(y))

    # Plot the waveform
    plt.figure(figsize=(10, 4))
    plt.plot(time, y, label="Amplitude")
    plt.xlabel("Time [s]")
    plt.ylabel("Amplitude")
    plt.title("Waveform")
    plt.grid()
    plt.tight_layout()

    return plt


def plot_wav_and_windows(wav_path, sr, n_fft, hop_length, win_length):
    y, sr = librosa.load(wav_path, sr=sr)
    # Create a time axis in seconds based on the sample rate and number of samples
    time = np.linspace(0, len(y) / sr, len(y))
    num_frames = 4
    y_len = len(y)

    plt.subplot(1+num_frames, 1, 1)
    plt.plot(y, label="Amplitude")
    plt.grid()

    plt.title("Waveform")

    window = get_window("hann", win_length)
    for i in range(0, num_frames-1):
        padded_window = np.pad(window, (i*hop_length, 0), 'constant')
        if len(padded_window) < y_len:
            padded_window = np.pad(
                padded_window, (0, y_len - len(padded_window)), 'constant')

        signal_window = np.multiply(y, padded_window)
        plt.subplot(num_frames, 1, 2+i)
        plt.plot(time, signal_window, time, padded_window)
        plt.grid()

    plt.xlabel("Time [s]")
    plt.show()


def wav_to_spectrogram(wav_path, sr):
    # Function to convert .wav file to a spectrogram
    # sr - sample rate
    y, sr = librosa.load(wav_path, sr=sr)
    print("y", y)
    print("y.shape", y.shape)
    stft = librosa.stft(y)
    spectrogram = np.abs(stft)
    spectrogram_db = librosa.amplitude_to_db(spectrogram, ref=np.max)

    return spectrogram_db, sr


def plot_spectrogram(spectrogram, sr):
    # Function to plot a spectrogram with viridis colormap
    plt.figure(figsize=(10, 6))
    librosa.display.specshow(
        spectrogram, sr=sr, x_axis='time', y_axis='log', cmap='viridis')
    plt.colorbar(format='%+2.0f dB', cmap='viridis')
    plt.title('Spectrogram (dB)')
    plt.xlabel('Time [s]')
    plt.ylabel('Frequency (log scale) [Hz]')
    plt.tight_layout()

    return plt


def plot_3d_spectrogram(spectrogram, sr, duration):
    # Function to plot a 3D spectrogram with correct time range
    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')
    time = np.linspace(0, duration, spectrogram.shape[1])
    freq = np.linspace(0, sr / 2, spectrogram.shape[0])
    time_mesh, freq_mesh = np.meshgrid(time, freq)
    ax.plot_surface(time_mesh, freq_mesh, spectrogram, cmap='viridis')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Frequency (Hz)')
    ax.set_zlabel('Amplitude (dB)')
    ax.set_title('3D Spectrogram')

    return fig


def plots_combined(wav_path, sr, duration):
    fig = plt.figure(figsize=(8, 6))

    # Define a 2x2 grid
    gs = gridspec.GridSpec(2, 2, figure=fig)

    # Create the first plot that spans the first two columns
    ax1 = fig.add_subplot(gs[0, 0:2])
    y, sr = librosa.load(wav_path, sr=sr)
    time = np.linspace(0, len(y) / sr, len(y))
    ax1.plot(time, y, label="Plot 1")
    ax1.set_xlabel("Time [s]")
    ax1.set_ylabel("Amplitude")
    ax1.set_title("Waveform")
    ax1.grid()

    # Second plot in the bottom-left position
    stft = librosa.stft(y)
    spectrogram = np.abs(stft)
    spectrogram = librosa.amplitude_to_db(spectrogram, ref=np.max)

    ax2 = fig.add_subplot(gs[1, 0])
    librosa_display = librosa.display.specshow(
        spectrogram, sr=sr, x_axis='time', y_axis='log', cmap='viridis', ax=ax2)
    ax2.set_title('Spectrogram')
    fig.colorbar(librosa_display, ax=ax2, format='%+2.0f dB', cmap='viridis')
    ax2.set_xlabel('Time [s]')
    ax2.set_ylabel('Frequency (log scale) [Hz]')

    # Third plot in the bottom-right position
    time = np.linspace(0, duration, spectrogram.shape[1])
    freq = np.linspace(0, sr / 2, spectrogram.shape[0])
    time_mesh, freq_mesh = np.meshgrid(time, freq)

    ax3 = fig.add_subplot(gs[1, 1], projection='3d')
    ax3.plot_surface(time_mesh, freq_mesh, spectrogram, cmap='viridis')
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Frequency (Hz)')
    ax3.set_zlabel('Amplitude (dB)')
    ax3.set_title('3D Spectrogram')

    plt.tight_layout()
    plt.show()
