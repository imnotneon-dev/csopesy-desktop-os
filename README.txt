# CSOPESY Semi-Major Output 2 - Desktop-Style OS Mock-up

**Course:** CSOPESY
**Section:** S01
**Group:** Guillermo, Lee, Lim

**Members**

- Iain Guillermo
- Hannah Lee
- Jenrick Lim

## Entry File

`csopesy-desktop-os/main.cpp`

The `main()` function is located in this file.

## About

A desktop-style OS mock-up built with Dear ImGui on top of GLFW and OpenGL. The program draws the desktop, taskbar, and windows every frame, similar to the compositor of a real operating system shell.

## Requirements

- Windows 10 or 11 (64-bit)
- Visual Studio 2022 (17.10 or newer, needed for `.slnx` files) with the **Desktop development with C++** workload installed
- GLFW and Dear ImGui are already included in the repository, so no extra installation is needed

## How to Run

1. Clone or download the repository.
2. Open `csopesy-desktop-os.slnx` in Visual Studio. If it does not open, open `csopesy-desktop-os/csopesy-desktop-os.vcxproj` instead.
3. In the toolbar, set the configuration to **Debug** and the platform to **x64**.
4. Press **F5** (Run/Debug) to build and start the program.
5. To close the program, click the **PWR** button on the taskbar. The window's X button is disabled on purpose, as required.

## Features

### Desktop

- Full-window background that is drawn first every frame
- Wallpaper
- Real-time clock in the top-right corner, updated every frame
- System status text (System Online / System Stopped)

### Taskbar

A fixed panel at the bottom of the screen.

| Button | Action |
| --- | --- |
| Folder icon | Opens a File Explorer window (placeholder content) |
| INIT | Opens the System Initialization Details window (placeholder content) |
| START / STOP | Switches the system status shown on the desktop |
| Graph icon | Opens the Task Manager |
| SRCH | Opens the System Search & Index window (placeholder content) |
| VOL / NET | Placeholder system buttons |
| PWR | Shuts down the application (the only way to close it) |

Buttons of open windows are highlighted.

### Task Manager

- Window with a table of processes and their CPU and memory usage
- Uses dummy values

## Project Structure

```
csopesy-desktop-os.slnx              Visual Studio solution
csopesy-desktop-os/main.cpp          Entry point, desktop, taskbar, windows
csopesy-desktop-os/imgui/            Dear ImGui source files
csopesy-desktop-os/GLFW/             GLFW headers
csopesy-desktop-os/glfw3.lib         GLFW library (Visual Studio)
csopesy-desktop-os/wallpaper.png     Desktop wallpaper image
```