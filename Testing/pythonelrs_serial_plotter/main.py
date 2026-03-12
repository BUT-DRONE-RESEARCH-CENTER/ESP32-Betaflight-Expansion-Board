import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import re

# --- Configuration ---
COM_PORT = '/dev/ttyUSB0'  # Change this to your ESP32 port
BAUD_RATE = 115200
NUM_CHANNELS = 16

# Setup Serial
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1)

# Setup Plot
fig, ax = plt.subplots()
x_data = list(range(1, NUM_CHANNELS + 1))
y_data = [1500] * NUM_CHANNELS # Default midpoint
bars = ax.bar(x_data, y_data, color='skyblue')

ax.set_ylim(800, 2200) # ELRS range is roughly 172-1811, adjust as needed
ax.set_xticks(x_data)
ax.set_xlabel('Channels')
ax.set_ylabel('Value')
ax.set_title('ELRS RC Channel Monitor')

def update(frame):
    if ser.in_waiting > 0:
        try:
            line = ser.readline().decode('utf-8').strip()
            # Extract numbers using regex (looks for digits following 'CHx: ')
            values = re.findall(r'CH\d+:\s*(\d+)', line)
            
            if len(values) >= NUM_CHANNELS:
                for i in range(NUM_CHANNELS):
                    val = int(values[i])
                    bars[i].set_height(val)
                    
                    # Optional: Color coding for visual feedback
                    if val > 1600: bars[i].set_color('green')
                    elif val < 1400: bars[i].set_color('red')
                    else: bars[i].set_color('skyblue')
        except Exception as e:
            print(f"Error parsing: {e}")
            
    return bars

# Animate the plot
ani = FuncAnimation(fig, update, blit=False, interval=20, cache_frame_data=False)
plt.show()

ser.close()
