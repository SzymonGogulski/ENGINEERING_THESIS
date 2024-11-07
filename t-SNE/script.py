from audio_preprocess import split_noise_to_samples, mix_key_word_and_noise_samples
from spectogram import plot_wav, wav_to_spectrogram, plot_spectrogram, plot_3d_spectrogram, plots_combined, plot_wav_and_windows
from pathlib import Path
print('some')
# Run these lines to prepare the data.
# split_noise_to_samples('noise', 'noise_samples', 'mp3', 'wav', 16000, 1)
# mix_key_word_and_noise_samples(
#     'key_words', 'noise_samples', 'key_words_with_noise', 'wav', 'wav')

# Turn .wav to spectogram
path = Path.cwd() / "key_words_with_noise" / "eight" / "0.wav"
spectrogram, sr = wav_to_spectrogram(str(path), 16000)

# print("spectrogram.shape", spectrogram.shape)
# wav = plot_wav(str(path), 16000)
# spec2d = plot_spectrogram(spectrogram, sr)
# spec3d = plot_3d_spectrogram(spectrogram, sr, 1)

# plots_combined(str(path), 16000, 1)

plot_wav_and_windows(str(path), 16000, 2048, 2048, 2048)
