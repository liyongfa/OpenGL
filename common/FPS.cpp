#include "FPS.h"
#include <windows.h>

#define DEFAULT_EXPECT_RENDER_FPS 30

namespace e
{
	static double GetTicks(void)
	{
		__int64 ret, freq;
		if (QueryPerformanceCounter((LARGE_INTEGER*)&ret) &&
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq))
		{
			return double(ret) / freq;
		}
		else
		{
			return double(::GetTickCount()) / 1000;
		}
	}

	FPS::FPS()
	{
		renderFPS = 0;
		skipedFPS = 0;
		renderFrameCount = 0;
		skipedFrameCount = 0;
		continuesSkipCount = 0;
		thisCycleRenderFrames = 0;
		renderFrameCycleT0 = GetTicks();
		lastState = stateIdle;
		workTimeSpan = 0;
		lastTime = GetTicks();
		load = 1.0;
		expectRenderFPS = DEFAULT_EXPECT_RENDER_FPS;
	}


	FPS::~FPS()
	{

	}

	FPS::State FPS::Step(void)
	{
		double t = GetTicks();

		if (lastState == stateSkip || lastState == stateRender)
		{
			workTimeSpan += t - lastTime;
		}

		double renderFrameSpan = t - renderFrameCycleT0;
		double totalFrameCount = renderFrameCount + skipedFrameCount;

		if (totalFrameCount >= expectRenderFPS)
		{
			renderFrameCycleT0 = t;
			renderFPS = renderFrameCount / renderFrameSpan;
			skipedFPS = skipedFrameCount / renderFrameSpan;

			if (renderFrameCount > 0)
			{
				double newLoad = workTimeSpan * expectRenderFPS / renderFrameSpan / renderFrameCount;
				load = newLoad;
			}

			renderFrameCount = 0;
			skipedFrameCount = 0;
			renderFrameSpan = 0;
			workTimeSpan = 0;
		}
		else if (renderFrameSpan < 0)
		{
			renderFrameCycleT0 = t;
			renderFrameSpan = 0;
		}

		lastTime = t;

		int n = expectRenderFPS;
		double frameShouleBe = renderFrameSpan * n;
		// �������̫�����Ϣһ��, �����0.5�Ǿ������ֵ
		if (frameShouleBe < renderFrameCount + (1.1 - load) * 0.5)
		{
			lastState = stateIdle;
			return stateIdle;
		}

		thisCycleRenderFrames++;
		if (thisCycleRenderFrames >= expectRenderFPS)
		{
			thisCycleRenderFrames = 0;
		}

		State ret;
		// ����, ���֡���½�С�����ֵ, ��û��Ҫ��֡
		static const double threshold_a = 2;
		static const double max_continus_skip = 1;

		if (continuesSkipCount >= max_continus_skip
			|| frameShouleBe < totalFrameCount + threshold_a)
		{
			renderFrameCount++;
			continuesSkipCount = 0;
			ret = stateRender;
		}
		else
		{
			continuesSkipCount++;
			skipedFrameCount++;
			ret = stateSkip;
		}

		lastState = ret;
		return ret;
	}
}
