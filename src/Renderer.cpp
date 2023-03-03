#include "Renderer.h"

void Renderer::init()
{
	/****************************
	*		setup GLFW
	*****************************/

	if (!glfwInit()) {
		throw std::runtime_error("Error initializing GLFW");
	}

	// set GLSL version
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// create window 
    // HACK: dont hardode window size
	m_window = glfwCreateWindow(1280, 720, "Lighting demo", NULL, NULL);

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
}

void Renderer::render(Scene*& scene)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    while (!glfwWindowShouldClose(m_window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#if 0
       

        // Rendering
        ImGui::Render();
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        m_vao.bind();
        m_shader.setVec3("u_color", { clear_color.x, clear_color.y, clear_color.z });
        glDrawArrays(GL_TRIANGLES, 0, 3);
#else
        
        

        ImGui::Begin("Window");
        scene->onRenderImGui();
        ImGui::End();
        ImGui::EndFrame();
        ImGui::Render();
        scene->onRender();
#endif

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

void Renderer::close()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}


void Renderer::handleEvent(const Event& e)
{
    switch (e.type()) {
    case Event::Type::WINDOW_RESIZE:
        printf("New window size: %d %d \n", e.window.width, e.window.height);
        break;
    case Event::Type::MOUSE_MOVE:
        printf("Mouse move: %f %f \n", e.mouse.x, e.mouse.x);
        break;
    case Event::Type::KEY_PRESS:
        printf("Key press: %d \n", e.key.keyCode);
        break;
    case Event::Type::KEY_RELEASE:
        printf("Key release: %d \n", e.key.keyCode);
        break;
    case Event::Type::MOUSE_BUTTON_PRESS:
        printf("Mouse button press: %d \n", e.key.keyCode);
        break;
    case Event::Type::MOUSE_BUTTON_RELEASE:
        printf("Mouse button release: %d \n", e.key.keyCode);
        break;
    };
}
