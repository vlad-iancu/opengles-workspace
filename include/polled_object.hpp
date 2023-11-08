#pragma once

namespace opengles_workspace 
{
	class PolledObject 
	{
	public:
		virtual ~PolledObject() {}

		virtual bool poll() = 0;
	};
}