import librosa
import librosa.display
import numpy as np
import matplotlib.pyplot as plt


def wav_to_spectrogram(wav_path, sr):
    # Function to convert .wav file to a spectrogram
    y, sr = librosa.load(wav_path, sr=sr)
    stft = librosa.stft(y)
    spectrogram = np.abs(stft)
    spectrogram_db = librosa.amplitude_to_db(spectrogram, ref=np.max)

    return spectrogram_db, sr


def plot_spectrogram(spectrogram, sr):
    # Function to plot a spectrogram
    plt.figure(figsize=(10, 6))
    librosa.display.specshow(spectrogram, sr=sr, x_axis='time', y_axis='log')
    plt.colorbar(format='%+2.0f dB')
    plt.title('Spectrogram (dB)')
    plt.xlabel('Time')
    plt.ylabel('Frequency (log scale)')

    plt.tight_layout()
    plt.show()
