#pragma once

#include "PixelThread.h"
#include <queue>
#include <vector>
#include <shared_mutex>

class RenderPool : public IETThread, public IFinishedTask {

public:
	RenderPool() {
		std::cout << "Thread to spawn: " <<
			std::thread::hardware_concurrency() << std::endl;
	};

	void InitPool(int total, int pixelWidth);
	void AddPixel(PixelThread* pixel);

	void StartRendering();

	std::vector<PixelColor> outputPixels;

	std::shared_mutex currPixelMutex;
	std::condition_variable_any  IsRenderDone;

	bool IsRunning()
	{
		return currentScanline >= totalScanlines;
	}

	void Wait();

private:
	bool isRunning = false;
	

	void run() override;
	void OnFinishedTask(int id, const std::vector<PixelColor>& pixelOut, IETThread* threadTask);

	int pixelWidth = 10;
	int workerCount = 20;
	int currWorkerCount = 0;

	int totalScanlines = 1;
	int currentScanline = 0;

	std::queue<PixelThread*> PixelQueue;
};