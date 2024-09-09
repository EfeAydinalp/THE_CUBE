# THE CUBE



https://github.com/user-attachments/assets/b771ae5d-3425-476c-b9aa-d5a4e26e588f



This project showcases a real-time 3D simulation of a rotating cube, driven by data from an MPU6050 gyroscope sensor. The sensor is interfaced with an STM32F407G-DISC1 microcontroller, and the simulation is rendered in Python using OpenGL.

## Table of Contents

- [Project Overview](#project-overview)
- [Hardware Setup](#hardware-setup)
- [Software Components](#software-components)
- [Code Structure](#code-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Calibration and Filtering](#calibration-and-filtering)
- [Customization](#customization)
- [Known Issues](#known-issues)
- [Future Improvements](#future-improvements)


## Project Overview

This project involves the integration of an MPU6050 sensor with an STM32F407G-DISC1 microcontroller to gather real-time motion data (gyroscope and accelerometer readings). The data is processed and transmitted via UART to a Python script, which then updates a 3D cube's orientation in real time. The primary goal is to visualize the sensor's orientation and motion in a clear and interactive manner.

## Hardware Setup

### Components:

1. **STM32F407G-DISC1 Microcontroller**
2. **MPU6050 Gyroscope/Accelerometer Sensor**
3. **Connection Wires**
4. **Computer with Python Environment**

### Connections:

- **MPU6050 to STM32F407G-DISC1:**
  - `SDA` (Data) -> `PB9`
  - `SCL` (Clock) -> `PB6`
  - `VCC` -> `3.3V`
  - `GND` -> `GND`

- **STM32F407G-DISC1 to Computer:**
  - UART Communication via `USART2` (PA2, PA3)

## Software Components

### 1. **Embedded C Code**:
   - **File**: `mpu6050.c`
   - **Functionality**:
     - Initializes the MPU6050 sensor.
     - Reads raw data from the sensor.
     - Converts raw data into human-readable units.
     - Applies filtering to reduce noise.
     - Transmits processed data via UART.

### 2. **Python Script**:
   - **File**: `THE_CUBE.py`
   - **Functionality**:
     - Receives UART data from the STM32.
     - Parses the data to extract pitch, roll, and yaw.
     - Updates a 3D cube’s orientation in real-time.
     - Uses OpenGL for rendering the 3D simulation.

## Code Structure

### C Code (`mpu6050.c`)

- **`mpu6050_init()`**: Initializes the sensor.
- **`mpu6050_read_raw()`**: Reads raw sensor data.
- **`mpu6050_read_converted()`**: Converts raw data to meaningful units.
- **`mpu6050_read_filtered()`**: Applies a low-pass filter to the data.
- **`mpu6050_read_and_send_filtered()`**: Combines reading, filtering, and transmitting data via UART.

### Python Code (`THE_CUBE.py`)

- **`parse_sensor_data(line)`**: Parses the incoming sensor data.
- **`draw_cube()`**: Renders the 3D cube using OpenGL.
- **`apply_threshold(value)`**: Filters out minor noise.
- **`update_scene(pitch_delta, roll_delta, yaw_delta)`**: Updates the cube's orientation based on sensor data.
- **`setup_viewport(width, height)`**: Configures the OpenGL viewport.
- **`main()`**: Main loop that handles data reception and visualization.

## Getting Started

### Prerequisites:

1. **Python 3.10.4** (Ensure compatibility with `glfw` library).
2. **STM32CubeIDE** for flashing the microcontroller.
3. **Required Python Libraries**:
   - `pyserial`
   - `PyOpenGL`
   - `GLFW`
   - `Pygame`

### Installation:

1. Clone the repository:
   ```bash
   git clone https://github.com/EfeAydinalp/THE_CUBE.git
   cd THE_CUBE
   ```
2. Install Python dependencies:
   ```bash
   pip install pyserial PyOpenGL glfw pygame
   ```
3. Flash the mpu6050.c code to your STM32 microcontroller using STM32CubeIDE.

### Usage:

1. Connect the hardware as described in the [References](#Hardware Setup) section.

2. Run the Python script:
   ```bash
   python THE_CUBE.py
   ```
3. The simulation window will open, displaying a cube whose orientation corresponds to the real-time data from the MPU6050 sensor.

### Calibration and Filtering:

- **`Calibration`**: Ensure that the MPU6050 is correctly calibrated by setting appropriate offsets for the accelerometer and gyroscope. This can be fine-tuned in the mpu6050.c file.
- **`Filtering`**: A simple low-pass filter is applied to smooth the data and reduce noise. The filter constant ALPHA can be adjusted in mpu6050.h to meet your needs.

### Customization:

- **`Adjust Rotation Sensitivity`**: Modify the RADYAN_TO_DEG constant in the Python script to change the sensitivity of the cube’s rotation.
- **`Thresholding`**: Adjust the THRESHOLD value to filter out small, insignificant movements.

### Customization:

- **`GLFW Compatibility`**: Ensure you are using Python 3.10.4 to avoid issues with the glfw library.
- **`UART Configuration`**: Double-check the UART settings in mpu6050.c to match your specific microcontroller setup.

### Future Improvements:

- **`Enhanced Filtering`**: Implement advanced filtering techniques like Kalman filters for more accurate data representation.
- **`Multi-Sensor Support`**: Extend the project to support multiple IMUs for more complex simulations.
- **`Additional Visualizations`**: Include more shapes or environments to represent different sensor data.
