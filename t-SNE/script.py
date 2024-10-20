
from audio_preprocess import split_noise_to_samples, mix_key_word_and_noise_samples
from spectogram import wav_to_spectrogram, plot_spectrogram
from pathlib import Path
print('some')
# Run these lines to prepare the data.
# split_noise_to_samples('noise', 'noise_samples', 'mp3', 'wav', 16000, 1)
# mix_key_word_and_noise_samples(
#     'key_words', 'noise_samples', 'key_words_with_noise', 'wav', 'wav')

# Turn .wav to spectogram
path = Path.cwd() / "key_words_with_noise" / "eight" / "0.wav"
spectrogram_db, sr = wav_to_spectrogram(str(path), 16000)
plot_spectrogram(spectrogram_db, sr)
