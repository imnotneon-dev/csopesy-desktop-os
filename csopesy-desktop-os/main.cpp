#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// global flag to control application shutdown
bool g_AllowShutdown = false;

// taskbar state
bool g_ShowFiles = false;
bool g_ShowInit = false;
bool g_ShowSearch = false;
bool g_ShowTaskManager = false;
bool g_SystemOnline = true;

const float TASKBAR_HEIGHT = 80.0f;
const float ICON_SIZE = 66.0f;
const float TASKBAR_PAD_X = 14.0f;
const float TASKBAR_PAD_Y = (TASKBAR_HEIGHT - ICON_SIZE) / 2.0f;
const float FLAT_BTN_W = 50.0f;

// helper function to get formatted time & date
std::string GetCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    struct tm time_info;

#if defined(_MSC_VER)
    localtime_s(&time_info, &time_now);
#else
    localtime_r(&time_now, &time_info);
#endif

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%A, %b %d, %Y | %I:%M:%S %p", &time_info);
    return std::string(buffer);
}

void RenderTaskManager() {
    if (!g_ShowTaskManager)
        return;

    // gets current window size
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // sets window size of task manager (scaled to 45% of desktop size)
    ImGui::SetNextWindowSize(ImVec2(displaySize.x * 0.65f, displaySize.y * 0.65f), ImGuiCond_FirstUseEver);

    // creates new window for task manager
    if (ImGui::Begin("Task Manager", &g_ShowTaskManager)) {
        // data struct for processes data
        struct ProcessRow {
            const char* name;   // process name
            float cpu;          // cpu usage
            float memory;       // memory usage
            float disk;         // disk usage
            float network;      // network usage
        };

        // dummy data
        static const ProcessRow processes[] = {
            { "Search", 17.4f, 6113.36f, 0.1f, 0.1f },
            { "Initialization Details", 1.4f, 179.1f, 0.0f, 0.0f },
            { "Task Manager", 0.8f, 107.0f, 0.0f, 0.0f },
            { "File Explorer", 1.5f, 156.2f, 0.0f, 0.0f }
        };

        // number of processes
        const int numProcessRows = sizeof(processes) / sizeof(processes[0]);

        // system capacity (dummy values)
        const float totalRAM = 8.0f * 1024.0f;  // 8 GB
        const float maxDisk = 500.0f;        // 500 MB/s
        const float maxNetwork = 100.0f;        // 100 Mbps

        // total usage values
        float totalCPU = 0.0f;
        float totalMemory = 0.0f;
        float totalDisk = 0.0f;
        float totalNetwork = 0.0f;

        for (int i = 0; i < numProcessRows; i++) {
            totalCPU += processes[i].cpu;
            totalMemory += processes[i].memory;
            totalDisk += processes[i].disk;
            totalNetwork += processes[i].network;
        }

        // converting to percentage values
        float totalMemoryPercent = (totalMemory / totalRAM) * 100.0f;
        float totalDiskPercent = (totalDisk / maxDisk) * 100.0f;
        float totalNetworkPercent = (totalNetwork / maxNetwork) * 100.0f;

        // format table headers
        char cpuHeader[32], memHeader[32], diskHeader[32], netHeader[32];
        snprintf(cpuHeader, sizeof(cpuHeader), "CPU (%.0f%%)", totalCPU);
        snprintf(memHeader, sizeof(memHeader), "Memory (%.0f%%)", totalMemoryPercent);
        snprintf(diskHeader, sizeof(diskHeader), "Disk (%.0f%%)", totalDiskPercent);
        snprintf(netHeader, sizeof(netHeader), "Network (%.0f%%)", totalNetworkPercent);

        ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders 
                                    | ImGuiTableFlags_RowBg 
                                    | ImGuiTableFlags_Resizable;

        // creates table to display task manager details
        if (ImGui::BeginTable("ProcessesTable", 5, tableFlags)) {
            ImGui::TableSetupColumn("Process Name");
            ImGui::TableSetupColumn(cpuHeader);
            ImGui::TableSetupColumn(memHeader);
            ImGui::TableSetupColumn(diskHeader);
            ImGui::TableSetupColumn(netHeader);
            ImGui::TableHeadersRow();

            // displays rows
            for (int i = 0; i < numProcessRows; i++) {
                ImGui::TableNextRow();

                // process name
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", processes[i].name);

                // cpu usage
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.1f%%", processes[i].cpu);

                // memory usage
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.1f MB", processes[i].memory);

                // disk usage
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.1f MB/s", processes[i].disk);

                // network usage
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.1f Mbps", processes[i].network);
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void RenderDesktop() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiWindowFlags desktop_flags = ImGuiWindowFlags_NoTitleBar
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                                    | ImGuiWindowFlags_NoNavFocus
                                    | ImGuiWindowFlags_NoScrollbar
                                    | ImGuiWindowFlags_NoSavedSettings;

    ImDrawList* draw_list = nullptr;
    ImVec2 pos = ImVec2(0.0f, 0.0f);
    ImVec2 size = ImVec2(0.0f, 0.0f);
    ImU32 col_top = IM_COL32(15, 32, 67, 255);
    ImU32 col_bottom = IM_COL32(52, 101, 164, 255);

    std::string clock_str = "";
    float clock_width = 0.0f;

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::Begin("DesktopBaseLayer", nullptr, desktop_flags);

    draw_list = ImGui::GetWindowDrawList();
    pos = ImGui::GetWindowPos();
    size = ImGui::GetWindowSize();

    draw_list->AddRectFilledMultiColor(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        col_top, col_top, col_bottom, col_bottom
    );

    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.0f, 1.0f), "CSOPESY OS MO4");
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.0f, 1.0f), "S01 - Guillermo, Lee, Lim");

    clock_str = GetCurrentTimeString();
    clock_width = ImGui::CalcTextSize(clock_str.c_str()).x;
    ImGui::SetCursorPos(ImVec2(viewport->WorkSize.x - clock_width - 20.0f, 15.0f));
    ImGui::TextUnformatted(clock_str.c_str());

    ImGui::SetCursorPos(ImVec2(20.0f, viewport->WorkSize.y * 0.62f));
    if (g_SystemOnline)
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "CSOPESY OS v1.0. System Online");
    else
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "CSOPESY OS v1.0. System Stopped");

    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

static bool CircleButton(const char* label, ImVec4 textColor, bool active = false) {
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushStyleColor(ImGuiCol_Button, active ? ImVec4(0.28f, 0.28f, 0.36f, 1.0f) : ImVec4(0.14f, 0.14f, 0.18f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.24f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.30f, 0.38f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ICON_SIZE / 2.0f);

    bool clicked = ImGui::Button(label, ImVec2(ICON_SIZE, ICON_SIZE));

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    return clicked;
}

static bool FlatButton(const char* label, ImVec4 textColor) {
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.08f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.15f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    bool clicked = ImGui::Button(label, ImVec2(FLAT_BTN_W, ICON_SIZE));

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    return clicked;
}

// type 0 = folder icon, type 1 = graph icon
static bool IconButton(const char* id, int type, bool active) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    bool clicked = ImGui::InvisibleButton(id, ImVec2(ICON_SIZE, ICON_SIZE));
    bool hovered = ImGui::IsItemHovered();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 c(p.x + ICON_SIZE / 2.0f, p.y + ICON_SIZE / 2.0f);

    ImU32 bg = active ? IM_COL32(72, 72, 92, 255) : (hovered ? IM_COL32(62, 62, 76, 255) : IM_COL32(36, 36, 46, 255));
    dl->AddCircleFilled(c, ICON_SIZE / 2.0f, bg);

    if (type == 0) {
        dl->AddRectFilled(ImVec2(c.x - 20, c.y - 17), ImVec2(c.x - 3, c.y - 8), IM_COL32(230, 170, 30, 255), 3.0f);
        dl->AddRectFilled(ImVec2(c.x - 22, c.y - 12), ImVec2(c.x + 22, c.y + 18), IM_COL32(250, 200, 60, 255), 4.0f);
        dl->AddRectFilled(ImVec2(c.x - 8, c.y + 6), ImVec2(c.x + 8, c.y + 16), IM_COL32(40, 120, 220, 255), 2.0f);
    }
    else {
        dl->AddRectFilled(ImVec2(c.x - 22, c.y - 18), ImVec2(c.x + 22, c.y + 18), IM_COL32(60, 150, 230, 255), 6.0f);
        ImVec2 wave[5] = {
            ImVec2(c.x - 16, c.y + 8), ImVec2(c.x - 8, c.y - 2), ImVec2(c.x - 2, c.y + 4),
            ImVec2(c.x + 6, c.y - 10), ImVec2(c.x + 16, c.y + 6)
        };
        dl->AddPolyline(wave, 5, IM_COL32(255, 255, 255, 255), ImDrawFlags_None, 3.0f);
    }
    return clicked;
}

void RenderTaskbar() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y + vp->WorkSize.y - TASKBAR_HEIGHT));
    ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, TASKBAR_HEIGHT));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(TASKBAR_PAD_X, TASKBAR_PAD_Y));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.12f, 1.0f));

    ImGui::Begin("Taskbar", nullptr, flags);

    // left side
    if (IconButton("##files", 0, g_ShowFiles))
        g_ShowFiles = !g_ShowFiles;

    ImGui::SameLine();
    ImVec2 sp = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddLine(ImVec2(sp.x, sp.y + 8.0f), ImVec2(sp.x, sp.y + ICON_SIZE - 8.0f), IM_COL32(90, 90, 100, 255), 1.5f);
    ImGui::Dummy(ImVec2(1.0f, ICON_SIZE));

    ImGui::SameLine();
    if (CircleButton("INIT", ImVec4(0.3f, 0.9f, 1.0f, 1.0f), g_ShowInit))
        g_ShowInit = !g_ShowInit;
    ImGui::SameLine();
    if (CircleButton("START", ImVec4(0.3f, 1.0f, 0.4f, 1.0f), g_SystemOnline))
        g_SystemOnline = true;
    ImGui::SameLine();
    if (CircleButton("STOP", ImVec4(1.0f, 0.3f, 0.3f, 1.0f), !g_SystemOnline))
        g_SystemOnline = false;
    ImGui::SameLine();
    if (IconButton("##taskmgr", 1, g_ShowTaskManager))
        g_ShowTaskManager = !g_ShowTaskManager;
    ImGui::SameLine();
    if (CircleButton("SRCH", ImVec4(0.7f, 0.4f, 1.0f, 1.0f), g_ShowSearch))
        g_ShowSearch = !g_ShowSearch;

    // right side
    float rightWidth = FLAT_BTN_W * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - rightWidth - TASKBAR_PAD_X, TASKBAR_PAD_Y));
    FlatButton("VOL", ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SameLine();
    FlatButton("NET", ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SameLine();
    if (FlatButton("PWR", ImVec4(1.0f, 0.3f, 0.3f, 1.0f))) {
        g_AllowShutdown = true;
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

void RenderFilesWindow() {
    if (!g_ShowFiles) return;
    static int selected = -1;
    const char* items[] = { "Documents", "Downloads", "Pictures", "Music", "System" };

    ImGui::SetNextWindowPos(ImVec2(80, 80), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360, 260), ImGuiCond_FirstUseEver);

    ImGui::Begin("File Explorer", &g_ShowFiles);
    ImGui::TextDisabled("C:/Users/Student");
    ImGui::Separator();
    for (int i = 0; i < 5; i++) {
        if (ImGui::Selectable(items[i], selected == i))
            selected = i;
    }
    ImGui::End();
}

void RenderInitWindow() {
    if (!g_ShowInit) return;
    ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 220), ImGuiCond_FirstUseEver);

    ImGui::Begin("System Initialization Details", &g_ShowInit);
    ImGui::Text("CSOPESY OS v1.0");
    ImGui::Separator();
    ImGui::Text("Kernel:    Loaded");
    ImGui::Text("Memory:    64000K OK");
    ImGui::Text("Display:   GLFW + OpenGL + Dear ImGui");
    ImGui::Text("Status:    %s", g_SystemOnline ? "System Online" : "System Stopped");
    ImGui::End();
}

void RenderSearchWindow() {
    if (!g_ShowSearch) return;
    static char query[64] = "";
    ImGui::SetNextWindowPos(ImVec2(500, 60), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("System Search & Index", &g_ShowSearch);
    ImGui::InputText("Search", query, sizeof(query));
    ImGui::Separator();
    ImGui::BulletText("C:/Documents/report.docx");
    ImGui::BulletText("C:/Pictures/wallpaper.png");
    ImGui::BulletText("C:/System/config.ini");
    ImGui::End();
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    int windowWidth = static_cast<int>(mode->width * 0.8f);
    int windowHeight = static_cast<int>(mode->height * 0.8f);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight,
        "CSOPESY Operating System Emulator", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwSetWindowCloseCallback(window, [](GLFWwindow* win) {
        if (!g_AllowShutdown) {
            glfwSetWindowShouldClose(win, GLFW_FALSE); 
        }
        });

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderDesktop();

        RenderTaskbar();
        RenderFilesWindow();
        RenderInitWindow();
        RenderSearchWindow();

        // task manager
        if (g_ShowTaskManager) 
            RenderTaskManager();

        // render frame
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.08f, 0.08f, 0.08f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}