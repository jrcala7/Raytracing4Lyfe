#include "RenderPool.h"

void RenderPool::InitPool(int total, int _pixelWidth)
{
	totalScanlines = total;
	currentScanline = 0;
	pixelWidth = _pixelWidth;

	outputPixels.clear();
	outputPixels.resize(totalScanlines * pixelWidth);
}

void RenderPool::AddPixel(PixelThread* pixel)
{
	pixel->AssignCallback(this);
	PixelQueue.push(pixel);
}

void RenderPool::StartRendering()
{
	isRunning = true;

	std::cout << "Starting render task of " << PixelQueue.size() << std::endl;

	this->start();
}

void RenderPool::run()
{
	while (isRunning) {
		std::unique_lock<std::shared_mutex> lock(currPixelMutex);

		if (!PixelQueue.empty() && currWorkerCount < workerCount) {
			auto task = PixelQueue.front();
			PixelQueue.pop();

			currWorkerCount++;
			lock.unlock();
			task->start();
		}
		else {
			lock.unlock();
		}
	}
}

void RenderPool::OnFinishedTask(int y, const std::vector<PixelColor>& pixelOut, IETThread* threadTask)
{
	{
		std::unique_lock<std::shared_mutex> lock(currPixelMutex);
		for (int i = 0; i < pixelWidth; i++) {
			outputPixels[y * pixelWidth + i] = pixelOut[i];
		}
		
		currentScanline++;
		currWorkerCount--;

		std::clog << "\rScanline done " << y << ": " << currentScanline << "/" << totalScanlines << std::flush;

		if (currentScanline >= totalScanlines) {
			isRunning = false;
		}
	}

	delete threadTask;
	IsRenderDone.notify_all();
}

void RenderPool::Wait()
{
	std::shared_lock<std::shared_mutex> lock(currPixelMutex);
	IsRenderDone.wait(lock, [this] {
		return currentScanline >= totalScanlines;
		});
}
