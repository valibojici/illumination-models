#include "App.h"

void App::run()
{
	m_renderer.init();
	m_renderer.render(m_scene);
	m_renderer.close();
}

App::App() : m_scene(new SceneMenu(m_scene)) {}