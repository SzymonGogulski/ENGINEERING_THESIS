from audio_preprocess import split_noise_to_samples, mix_key_word_and_noise_samples
from spectogram import plot_wav, wav_to_spectrogram, plot_spectrogram, plot_3d_spectrogram, plots_combined, plot_wav_and_windows
from pathlib import Path
import os
import sys
import librosa

print('some')

os.system("cls")

import numpy as np
from pydub import AudioSegment
from pydub.playback import play
# path = Path.cwd() / "key_words" / "eight" / "11860c84_nohash_2.wav"
# # path = Path.cwd() / "key_words" / "eight" / "0a2b400e_nohash_0.wav"
# segment = AudioSegment.from_file(path)

# if segment.frame_rate != 16000:
#     segment.set_frame_rate(16000)
# play(segment)

# print(segment.frame_rate)
# print(segment.sample_width)
# print(segment.frame_count())
# print(segment.frame_count()/segment.frame_rate)
# print(segment.duration_seconds)
# print(len(segment))
# print("====")


# def adjust_frame_count(segment: AudioSegment, target_frame_count: int = 16000) -> AudioSegment:
#     current_frame_count = segment.frame_count()
    
#     # Calculate the duration of one frame in milliseconds
#     frame_duration_ms = 1000 / segment.frame_rate
    
    
#     if current_frame_count < target_frame_count:
#         padding_duration_ms = (target_frame_count - current_frame_count) * frame_duration_ms
#         silence = AudioSegment.silent(duration=padding_duration_ms, frame_rate=segment.frame_rate)
#         segment = segment + silence
#     elif current_frame_count > target_frame_count:
#         max_duration_ms = target_frame_count * frame_duration_ms
#         segment = segment[:max_duration_ms]
    
#     return segment


# segment = adjust_frame_count(segment)

# play(segment)
# print(segment.frame_rate)
# print(segment.sample_width)
# print(segment.frame_count())
# print(segment.frame_count()/segment.frame_rate)
# print(segment.duration_seconds)
# print(len(segment))

# import matplotlib
# import matplotlib.pyplot as plt


# Run these lines to prepare the data.
# split_noise_to_samples('noise', 'noise_samples', 'mp3', 'wav', 16000, 1)
# print("done")
mix_key_word_and_noise_samples(
    'key_words', 'noise_samples', 'key_words_with_noise', 'wav', 'wav')
print("done")

# Turn .wav to spectogram
# path = Path.cwd() / "key_words" / "zero" / "0a2b400e_nohash_0.wav"
# y, sr = librosa.load(path, sr=16000)

# from pydub import AudioSegment
# from pydub.playback import play
# sample = AudioSegment.from_wav(path)
# play(sample)

#spectrogram, sr = wav_to_spectrogram(str(path), 16000)


# print("spectrogram.shape", spectrogram.shape)
# wav = plot_wav(str(path), 16000)
# spec2d = plot_spectrogram(spectrogram, sr)
# spec3d = plot_3d_spectrogram(spectrogram, sr, 1)

# plots_combined(str(path), 16000, 1)

# plot_wav_and_windows(str(path), 16000, 2048, 2048, 2048)
