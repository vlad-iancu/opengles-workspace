#include <main_loop.hpp>
#include <exception.hpp>

#include <cassert>

namespace opengles_workspace
{
	void MainLoop::addPolledObject(std::shared_ptr<PolledObject> polledObject) {
		assert(polledObject);
		mPolledObjects.emplace_back(std::move(polledObject));
	}

	void MainLoop::run() {
		bool done = false;
		do {
			for (const auto& obj : mPolledObjects) {
				if (!obj->poll()) {
					done = true;
					break;
				}
			}
		} while (!done);
		
	}
}