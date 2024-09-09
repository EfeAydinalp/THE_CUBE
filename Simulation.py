
"""                         https://github.com/EfeAydinalp                          """                         

#----------------------------------------------------------------------------------
#Used Python interpreter 3.10.4 other interpreters may cause error for 'import glfw' line.
#----------------------------------------------------------------------------------
import serial
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import glfw
import numpy as np
import pygame
#----------------------------------------------------------------------------------
#Initialize accumulated angles.
#These variables store the cumulative rotation angles for the cube.
#----------------------------------------------------------------------------------
accumulated_pitch = 0.0
accumulated_roll = 0.0
accumulated_yaw = 0.0
#----------------------------------------------------------------------------------
#A small value used to ignore minor noise and drift from the sensor data. If a detected movement is smaller than this threshold, it will be ignored.
#You can change it as you wish.
#----------------------------------------------------------------------------------
THRESHOLD = 3.98
#----------------------------------------------------------------------------------
#1 Radian equals to 57,2958 degree our simulation has 6x const. at first so 1/6 x 57,2958 equals to 9.5493
#If you want to speed up the simulations's rotation speed you can change the constant RADYAN_TO_DEG
#To read more about RADYAN_TO_DEG please check the pdf from: https://github.com/EfeAydinalp/THE_CUBE
#If you change that value to half of it (which is 4.77465) and rotate your sensor 360 degree, the cube will rotate 720 degree in same direction as your sensor.
#----------------------------------------------------------------------------------
RADYAN_TO_DEG = 9.5493 
#----------------------------------------------------------------------------------
#This function reads the raw IMU data line, splits it into individual components, and converts these into pitch, roll, and yaw values based on the corresponding keys (’GX’, ’GY’, ’GZ’).
#If there is any error during parsing, it catches the exception and returns None.
#----------------------------------------------------------------------------------
def parse_sensor_data(line):
    try:
        data_parts = line.split()
        data_dict = {}
        for part in data_parts:
            key, value = part.split('=')
            data_dict[key] = float(value)
        pitch = data_dict.get('GX', 0.0)
        roll = data_dict.get('GY', 0.0)
        yaw = data_dict.get('GZ', 0.0)        
        return pitch, roll, yaw
    except Exception as e:
        print(f"Error parsing line: {line} -> {e}")
        return None
#----------------------------------------------------------------------------------
# This function uses OpenGL commands to draw a colored cube. Each face of the cube is drawn with a specific color using the glColor3f function.
#----------------------------------------------------------------------------------
def draw_cube():
    glBegin(GL_QUADS)    
    # Front face
    glColor3f(1.0, 0.0, 0.0)  # Red
    glVertex3f(1.0, 1.0, -1.0)
    glVertex3f(-1.0, 1.0, -1.0)
    glVertex3f(-1.0, -1.0, -1.0)
    glVertex3f(1.0, -1.0, -1.0)    
    # Back face
    glColor3f(0.0, 1.0, 0.0)  # Green
    glVertex3f(1.0, 1.0, 1.0)
    glVertex3f(-1.0, 1.0, 1.0)
    glVertex3f(-1.0, -1.0, 1.0)
    glVertex3f(1.0, -1.0, 1.0)    
    # Left face
    glColor3f(0.0, 0.0, 1.0)  # Blue
    glVertex3f(-1.0, 1.0, 1.0)
    glVertex3f(-1.0, 1.0, -1.0)
    glVertex3f(-1.0, -1.0, -1.0)
    glVertex3f(-1.0, -1.0, 1.0) 
    # Right face
    glColor3f(1.0, 1.0, 0.0)  # Yellow
    glVertex3f(1.0, 1.0, 1.0)
    glVertex3f(1.0, 1.0, -1.0)
    glVertex3f(1.0, -1.0, -1.0)
    glVertex3f(1.0, -1.0, 1.0)  
    # Top face
    glColor3f(1.0, 0.0, 1.0)  # Magenta
    glVertex3f(1.0, 1.0, 1.0)
    glVertex3f(-1.0, 1.0, 1.0)
    glVertex3f(-1.0, 1.0, -1.0)
    glVertex3f(1.0, 1.0, -1.0) 
    # Bottom face
    glColor3f(0.0, 1.0, 1.0)  # Cyan
    glVertex3f(1.0, -1.0, 1.0)
    glVertex3f(-1.0, -1.0, 1.0)
    glVertex3f(-1.0, -1.0, -1.0)
    glVertex3f(1.0, -1.0, -1.0)
    glEnd()
#----------------------------------------------------------------------------------
#This function ensures that small changes in sensor data (below the threshold) are ignored, preventing the cube from moving due to minor noise or drift.
# If the absolute value of the input is smaller than the threshold, the function returns 0.0; otherwise, it returns the original value.
#----------------------------------------------------------------------------------
def apply_threshold(value):
    if abs(value) < THRESHOLD:
        return 0.0
    return value
#----------------------------------------------------------------------------------
#This block initializes Pygame and sets up the font system to render text in the OpenGL window.
pygame.init()
pygame.font.init()
font = pygame.font.SysFont("Arial", 18)
#----------------------------------------------------------------------------------
#Renders text on the screen using Pygame and OpenGL.
def render_text(x, y, text):
    text_surface = font.render(text, True, (255, 255, 255), (0, 0, 0))  # White text, black background
    text_data = pygame.image.tostring(text_surface, "RGBA", True)
    glWindowPos2f(x, y)  # Set the position on the window for 2D text
    glDrawPixels(text_surface.get_width(), text_surface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, text_data)
#----------------------------------------------------------------------------------
#This function handles the logic for up dating the cube’s orientation based on the latest IMU data:
#   •The deltas (pitch, roll, yaw) are first filtered using the apply-threshold function.
#   •The filtered deltas are then added to the accumulated rotation angles.
#   •The OpenGL scene is cleared and reset, and the cube is redrawn with the new rotation values applied
#----------------------------------------------------------------------------------
def update_scene(pitch_delta, roll_delta, yaw_delta):
    global accumulated_pitch, accumulated_roll, accumulated_yaw
    # Apply the threshold to the deltas
    pitch_delta = apply_threshold(pitch_delta)
    roll_delta = apply_threshold(roll_delta)
    yaw_delta = apply_threshold(yaw_delta)
    # Update the accumulated angles
    accumulated_pitch += (pitch_delta/RADYAN_TO_DEG)
    accumulated_roll += (roll_delta/RADYAN_TO_DEG)
    accumulated_yaw += (yaw_delta/RADYAN_TO_DEG)
    #If accumulated values are higher than 360 degree or lower than -360 degree we set them as zero to reduce some wrong calculations in simulation.
    if(accumulated_yaw > 360 or accumulated_yaw < -360):
        accumulated_yaw = 0
    if(accumulated_roll > 360 or accumulated_roll < -360):
        accumulated_roll = 0
    if(accumulated_pitch > 360 or accumulated_pitch < -360):
        accumulated_pitch = 0
    print(f"Updating scene with Pitch: {pitch_delta}, Roll: {roll_delta}, Yaw: {yaw_delta}")
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    # Set up perspective projection for 3D drawing
    glPushMatrix()  # Push for 3D transformations
    glTranslatef(0.0, 0.0, -5)
    glRotatef(accumulated_roll, 1.0, 0.0, 0.0)
    glRotatef(accumulated_pitch, 0.0, 1.0, 0.0)
    glRotatef(accumulated_yaw, 0.0, 0.0, 1.0)
    draw_cube()
    # Restore the projection matrix for 2D drawing
    glPopMatrix()
    #Switch to orthographic projection for 2D text rendering
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluOrtho2D(0, 800, 600, 0)  # Set up orthographic projection with window size
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    # Render text in the top-left corner (screen coordinates)
    render_text(10, 20, f"Pitch: {accumulated_pitch:.2f}")
    render_text(10, 40, f"Roll: {accumulated_roll:.2f}")
    render_text(10, 60, f"Yaw: {accumulated_yaw:.2f}")
    # Restore perspective projection
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)
    # Swap the buffers to show the new frame
    glfw.swap_buffers(window)
#----------------------------------------------------------------------------------
#This function sets up the OpenGL viewport and perspective projection matrix, defining how the scene will be viewed in the window.
#----------------------------------------------------------------------------------
def setup_viewport(width, height):
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, width / height, 0.1, 50.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
#----------------------------------------------------------------------------------
# The core function of the program, which initializes the GLFW window, sets up the OpenGL context, and enters the main event loop. In each iteration: 
#   • It checks if new IMU data is available on the serial port
#   • If data is available, it reads and parses the data.
#   • The parsed orientation data is then used to update the scene.
#   • GLFW’s event handling is continuously polled to respond to window events.
#----------------------------------------------------------------------------------
def main():
    global window
    if not glfw.init():
        raise Exception("GLFW cannot be initialized")
    width, height = 800, 600
    window = glfw.create_window(width, height, "IMU Visualization", None, None)
    if not window:
        glfw.terminate()
        raise Exception("GLFW window cannot be created")
    glfw.make_context_current(window)
    glEnable(GL_DEPTH_TEST)
    setup_viewport(width, height)  # Set up viewport and perspective
    while not glfw.window_should_close(window):
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            print(f"Received data: {line}")
            orientation = parse_sensor_data(line)
            if orientation:
                update_scene(*orientation) 
        glfw.poll_events()
    glfw.terminate()
#----------------------------------------------------------------------------------
#   Serial Connection Setup: 
#       Before starting the main loop, a serial connection to the IMU is established.
#       he port parameter should be set to the correct COM port where the IMU is connected, and the baud rate must match the IMU’s configuration.
#   Program Entry Point: 
#       The script checks if it is the main module being executed, and if so, it establishes the serial connection and starts the main function.
#----------------------------------------------------------------------------------
if __name__ == "__main__":
    # Configure serial connection
    ser = serial.Serial(
        port='COM5',        # Replace with your COMX port
        baudrate=115200,
        timeout=1
    )
    main()
