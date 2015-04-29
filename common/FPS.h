#pragma once

namespace e
{
	class FPS
	{
	public:
		FPS(void);
		~FPS(void);

		enum State{
			stateIdle,
			stateSkip,
			stateRender
		};

		State Step(void);

		double GetRenderFPS(void)
		{
			return renderFPS;
		}
		double GetSkipedFPS(void)
		{
			return skipedFPS;
		}
		int GetThisCycleFrameCount(void)
		{
			return thisCycleRenderFrames;
		}
		double GetLoad(void)
		{
			return load;
		}
		void SetRenderFPS(int fps)
		{
			expectRenderFPS = fps;
		}
	private:
		double load;
		double lastTime;
		State lastState;
		double workTimeSpan;
		double renderFrameCount;
		double skipedFrameCount;
		double renderFPS;
		double skipedFPS;
		double renderFrameCycleT0;
		int thisCycleRenderFrames;
		int continuesSkipCount;
		int expectRenderFPS;
	};
}

