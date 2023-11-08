#pragma once

namespace opengles_workspace {
	class Context
	{
	public:
		Context(void* window)
			: mWindow(window)
		{}
		void* window() const { return mWindow; }
	private:
		void* mWindow;
	};
}