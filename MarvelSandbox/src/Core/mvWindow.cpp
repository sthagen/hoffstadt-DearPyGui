#include "mvWindow.h"

namespace Marvel {

	void mvWindow::run()
	{

		setup();
		while (m_running)
		{
			prerender();
			if (m_app->isOk())
				m_app->render();
			if(mvApp::GetApp()->showLog())
				Marvel::AppLog::getLogger()->Draw("Marvel Sandbox", &mvApp::GetApp()->showLog());
			postrender();
		}

	}

}