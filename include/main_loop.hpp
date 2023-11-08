#pragma once
#include <renderer.hpp>
#include <polled_object.hpp>
#include <vector>
#include <memory>


namespace opengles_workspace
{
	class MainLoop
	{
	public:
		MainLoop() = default;

		void addPolledObject(std::shared_ptr<PolledObject> polledObject);
		void run();
	private:
		std::vector<std::shared_ptr<PolledObject>> mPolledObjects;
	};
}