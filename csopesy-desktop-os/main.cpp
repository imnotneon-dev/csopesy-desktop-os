#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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

void RenderDesktop() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiWindowFlags desktop_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImDrawList* draw_list = nullptr;
    ImVec2 pos = ImVec2(0.0f, 0.0f);
    ImVec2 size = ImVec2(0.0f, 0.0f);
    ImU32 col_top = IM_COL32(15, 32, 67, 255);
    ImU32 col_bottom = IM_COL32(52, 101, 164, 255);

    std::string clock_str = "";
    float clock_width = 0.0f;

    float btn_width = 75.0f;
    float btn_height = 35.0f;
    GLFWwindow* current_win = nullptr;

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

    ImGui::SetCursorPos(ImVec2(
        viewport->WorkSize.x - btn_width - 20.0f,
        viewport->WorkSize.y - btn_height - 20.0f
    ));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.10f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.05f, 0.05f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    if (ImGui::Button("POWER", ImVec2(btn_width, btn_height))) {
        current_win = glfwGetCurrentContext();
        if (current_win) {
            g_AllowShutdown = true;
            glfwSetWindowShouldClose(current_win, GLFW_TRUE);
        }
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
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
            glfwSetWindowShouldClose(win, GLFW_FALSE); // Reject exit request
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

        // start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // desktop base layer
        RenderDesktop();

        // taskbar


        // task manager


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

    // cleanup & shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
