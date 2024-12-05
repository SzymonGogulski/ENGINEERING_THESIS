from pathlib import Path
from pydub import AudioSegment
from pydub.playback import play
import numpy as np
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import random
import os



def print_audio_params(audio: AudioSegment):
    """
    Print parameters of given audio segment.
    """
    print("Frame rate: ", audio.frame_rate, "Hz")
    print("Audio channels: ", audio.channels)
    print("Audio duration: ", audio.duration_seconds, "s")
    print("Total number of samples: ", len(
        audio.get_array_of_samples()))
    print("\n")
    play(audio)

    # Turn AudioSegment to numpy array and a plot.
    arr = np.array(audio.get_array_of_samples())
    plt.plot(arr)
    plt.xlabel('Sample')
    plt.ylabel('Amplitude')
    plt.title('Audio Waveform')
    plt.show()


def split_noise_to_samples(input_dir: str, output_dir: str, input_format: str, export_format: str, freq: int, channels: int):
    """
        Splits downloaded .mp3 files in /noise directory to 1 second long .wav files in the / n directory.
    """
    input_path = Path.cwd() / input_dir
    output_path = Path.cwd() / output_dir

    # Check input dir.
    if not input_path.exists():
        print(input_dir, "not found. ")
        return

    # Check or create output dir.
    if output_path.exists():
        if os.listdir(output_path) != []:
            print(output_dir, "already created. ")
            return
    else:
        os.mkdir(output_path)

    # Check format.
    if input_format not in ['mp3', 'wav'] or export_format not in ['mp3', 'wav']:
        print("Incorrect formats. ")
        return

    counter = 0     # count number of files created.

    # For every file in input dir split audio format files into 1 second long fragments.
    for file in input_path.glob('*.' + input_format):
        # Read file and set parameters.
        audio = AudioSegment.from_file(file, format=input_format)
        audio = audio.set_frame_rate(16000)
        audio = audio.set_channels(channels)

        # Split files.
        if len(audio)/float(1000) > 1:
            for i in range(1000, len(audio), 1000):
                fragment = audio[i-1000:i]
                fragment.export(
                    f"{str(output_path)}\{str(counter)}.{export_format}", format=export_format)
                counter = counter + 1


def get_random_noise_sample(noise_path: Path) -> Path:

    if not noise_path.exists():
        # Check noise dir.
        print(str(noise_path), "not found. ")

    wav_files = list(noise_path.glob("*.wav"))

    if not wav_files:
        raise FileNotFoundError(f"No .wav files found in {noise_path}")

    return random.choice(wav_files)


def normalize_loudness(audio: AudioSegment, target_dBFS: float) -> AudioSegment:
    """Normalize audio segment to a target loudness in dBFS."""
    change_in_dBFS = target_dBFS - audio.dBFS
    return audio.apply_gain(change_in_dBFS)


def compare_two_samples(first: AudioSegment, first_label: str, second: AudioSegment, second_label: str):
    first = np.array(first.get_array_of_samples())
    second = np.array(second.get_array_of_samples())
    plt.plot(first)
    plt.plot(second)
    plt.grid()
    plt.legend(first_label, second_label)
    plt.xlabel('Sample')
    plt.ylabel('Amplitude')
    plt.title('Audio Waveform')
    plt.show()


def check_samples(first: AudioSegment, second: AudioSegment, channels: int, frame_rate: int):
    if (first.channels == second.channels and second.channels == channels and first.frame_rate == second.frame_rate and second.frame_rate == frame_rate):
        return True
    else:
        return False


def mix_key_word_and_noise_samples(key_words_dir: str, noise_dir: str, output_dir: str, input_format: str, export_format: str):
    key_words_path = Path.cwd() / key_words_dir
    noise_path = Path.cwd() / noise_dir
    output_path = Path.cwd() / output_dir

    if not key_words_path.exists():
        # Check key words dir.
        print(key_words_dir, "not found. ")

    if not noise_path.exists():
        # Check noise dir.
        print(noise_dir, "not found. ")

    if output_path.exists():
        # Check or create output dir.
        if os.listdir(output_path) != []:
            print(output_dir, "already created. ")
            return
    else:
        os.mkdir(output_path)

    # Check format.
    if input_format not in ['mp3', 'wav']:
        print("Incorrect file format. ")
        return

    # Iterate over subdirectoires and files in key words. Create mixed key words with noise.
    skip = 0
    for dirpath, dirnames, filenames in os.walk(key_words_path):

        if skip == 0:
            skip = 1
        else:
            top_dir = os.path.basename(dirpath)
            os.mkdir(f"{str(output_path)}\{top_dir}")

        counter = 0
        # Print each filename in the current directory
        for file in filenames:
            key_word = f"{dirpath}\{file}"
            key_word_audio = AudioSegment.from_file(key_word, input_format)
            noise_sample = AudioSegment.from_file(
                get_random_noise_sample(noise_path), input_format)

            # Normalize the audio loudness.
            key_word_audio_normalized = normalize_loudness(
                key_word_audio, -20)
            noise_sample_normalized = normalize_loudness(
                noise_sample, -20)

            overlayed_audio_normalized = key_word_audio_normalized.overlay(
                noise_sample_normalized)

            overlayed_audio_normalized.export(
                f"{str(output_path)}\{top_dir}\{str(counter)}.{export_format}", format=export_format)
            counter = counter + 1



