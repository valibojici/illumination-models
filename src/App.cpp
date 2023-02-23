#include "App.h"

void App::run()
{
	m_renderer.init();
	m_renderer.render();
	m_renderer.close();
}
