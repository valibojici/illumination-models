#include "App.h"

App::App() {
    /****************************
    *		setup GLFW
    *****************************/

    if (!glfwInit()) {
        throw std::runtime_error("Error initializing GLFW");
    }

    // set GLSL version
    const char* glsl_version = nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // create window 
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Lighting demo", NULL, NULL);

    glfwMakeContextCurrent(m_window);

    /****************************
    *		setup GLEW
    *****************************/
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    /***************************
    *  register event callbacks
    ***************************/
    EventManager::getInstance().registerCallbacks(m_window);
    EventManager::getInstance().addHandler(this);


    /****************************
    *		setup ImGui
    *****************************/

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    m_scene = new SceneMenu(m_scene, m_windowWidth, m_windowHeight);
}

void App::run()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    EventManager::setImGuiIO(&io);

    // render loop taken from example_glfw_opengl3/main.cpp
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (m_showImguiWindow) {
            if (!m_setImguiWindowPos) {
                m_setImguiWindowPos = true;
                ImGui::SetNextWindowPos(ImGui::GetMousePos());
            }
            ImGui::Begin("Window", &m_showImguiWindow);
            m_scene->onRenderImGui();
            ImGui::End();
        }

        ImGui::EndFrame();
        ImGui::Render();
        m_scene->onRender();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(m_window);
    }
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}


void App::handleEvent(const Event& e)
{
    switch (e.type()) {
    case Event::Type::WINDOW_RESIZE:
        m_windowWidth = (unsigned int)e.window.width;
        m_windowHeight = (unsigned int)e.window.height;
        m_scene->updateWidthHeight(m_windowWidth, m_windowHeight); // update FBOs 
        break;
    case Event::Type::MOUSE_BUTTON_PRESS:
        if (e.mouse.keyCode == GLFW_MOUSE_BUTTON_2) {
            m_showImguiWindow = !m_showImguiWindow; // toggle flag to show/hide window
            if (m_showImguiWindow) { // if window will be shown set the flag to set-window-position-to-mouse to true
                m_setImguiWindowPos = false;
            }
        }
        break;
    };
}

void App::glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
