
import wave
import math
import struct
import random
import os

SAMPLE_RATE = 44100

def write_wav(filename, duration, generator_func, volume=0.5):
    n_frames = int(SAMPLE_RATE * duration)
    data = bytearray()
    
    # State for the generator
    state = {'phase': 0.0, 't': 0.0}
    
    for i in range(n_frames):
        t = i / SAMPLE_RATE
        state['t'] = t
        
        val = generator_func(state)
        
        # Hard clip
        val = max(-1.0, min(1.0, val))
        
        # Scale to 16-bit signed integer
        sample = int(val * 32760 * volume)
        data.extend(struct.pack('<h', sample))
        
    with wave.open(filename, 'w') as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(SAMPLE_RATE)
        f.writeframes(data)
    print(f"Generated {filename}")

def shoot_sound(state):
    # Pew pew: Frequency sweep down
    # Freq goes from 800Hz to 100Hz linearly
    t = state['t']
    start_freq = 800
    end_freq = 100
    duration = 0.2
    
    if t > duration: t = duration
    
    # Linear ramp
    current_freq = start_freq + (end_freq - start_freq) * (t / duration)
    
    # Phase accumulation
    phase_delta = 2 * math.pi * current_freq / SAMPLE_RATE
    state['phase'] += phase_delta
    
    # Square/Saw hybrid for retro sound
    val = math.sin(state['phase'])
    return 1.0 if val > 0 else -1.0

def hit_sound(state):
    # Noise with exponential decay
    t = state['t']
    decay = math.exp(-20 * t)
    # White noise
    noise = random.uniform(-1, 1)
    return noise * decay

def die_sound(state):
    # Low frequency rumble/slide down
    t = state['t']
    start_freq = 200
    end_freq = 20
    duration = 1.0
    
    current_freq = start_freq + (end_freq - start_freq) * (t / duration)
    
    phase_delta = 2 * math.pi * current_freq / SAMPLE_RATE
    state['phase'] += phase_delta
    
    # Sawtoothish
    val = (state['phase'] % (2*math.pi)) / (2*math.pi)
    val = 2.0 * val - 1.0
    
    return val * (1.0 - t/duration)

def music_loop(state):
    # Simple Arpeggio C minor: C G Eb
    t = state['t']
    seconds_per_note = 0.2
    
    # Frequencies
    notes = [261.63, 392.00, 311.13, 523.25] # C4, G4, Eb4, C5
    idx = int(t / seconds_per_note) % len(notes)
    freq = notes[idx]
    
    phase_delta = 2 * math.pi * freq / SAMPLE_RATE
    state['phase'] += phase_delta
    
    # Square wave audio
    val = 1.0 if math.sin(state['phase']) > 0 else -1.0
    return val * 0.5 

if __name__ == "__main__":
    if not os.path.exists("hunter/assets"):
        os.makedirs("hunter/assets")
        
    write_wav("hunter/assets/shoot.wav", 0.2, shoot_sound, 0.6)
    write_wav("hunter/assets/hit.wav", 0.3, hit_sound, 0.6)
    write_wav("hunter/assets/die.wav", 1.0, die_sound, 0.6)
    write_wav("hunter/assets/music.wav", 6.4, music_loop, 0.4)
