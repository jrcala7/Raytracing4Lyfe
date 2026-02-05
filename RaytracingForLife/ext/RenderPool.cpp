#include "RenderPool.h"

void RenderPool::InitPool(int total)
{
	totalPixels = total;
	currentPixels = 0;

	outputPixels.clear();
	outputPixels.resize(totalPixels);
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
			std::this_thread::yield();
		}
	}
}

void RenderPool::OnFinishedTask(int id, PixelColor pixelOut)
{
	{
		std::unique_lock<std::shared_mutex> lock(currPixelMutex);
		outputPixels[id] = pixelOut;
		currentPixels++;
		currWorkerCount--;

		//std::clog << "\rScanline done " << id << ": " << currentPixels << "/" << totalPixels << std::flush;

		if (currentPixels >= totalPixels) {
			isRunning = false;
		}
	}
	IsRenderDone.notify_all();
}

void RenderPool::Wait()
{
	std::shared_lock<std::shared_mutex> lock(currPixelMutex);
	IsRenderDone.wait(lock, [this] {
		return currentPixels >= totalPixels;
		});
}
