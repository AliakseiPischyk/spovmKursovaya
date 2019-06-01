#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
	ThreadPool(const size_t numThreads);
	template<class Func, class... Args>
	auto enqueue(Func&& func, Args&&... args)
		->std::future<std::_Invoke_result_t<Func, Args...>>;
	~ThreadPool();
private:
	std::vector< std::thread > executors;
	std::queue< std::function<void()> > tasks;

	std::mutex queueMutex;
	std::condition_variable conditionVariable;
	bool stoped;
};

ThreadPool::ThreadPool(const size_t numThreads)
	: stoped(false)
{
	for (size_t i = 0; i < numThreads; ++i)
		executors.emplace_back(
			[this]{
		while(true){
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(this->queueMutex);
				this->conditionVariable.wait(lock,
					[this] { return this->stoped || !this->tasks.empty(); });
				if (this->stoped && this->tasks.empty())
					return;
				task = std::move(this->tasks.front());
				this->tasks.pop();
			}
			task();
		}
	}
	);
}

template<class Func, class... Args>
auto ThreadPool::enqueue(Func&& func, Args&&... args)
-> std::future<typename std::invoke_result_t<Func, Args...>>
{
	using return_type = std::invoke_result_t<Func, Args...>; // дл€ выведени€ типа, возвращаемого переданной функцией, в врем€ компил€ции

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
		);//создаем shared_ptr на packeged_task дл€ последующего асинхронного выполнени€ задани€, поставленного в очередь. Ѕиндим передачу аргументов.

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queueMutex);//блокируем очередь

		if (stoped)//если вызван деструктор ThreadPool, выкидываем ошибку
			throw std::runtime_error("enqueue on stopedped ThreadPool");

		tasks.emplace([task]() { (*task)(); });//ставим задание в конец очереди+начинаем его выполнение
	}
	conditionVariable.notify_one();
	return res;
}

__forceinline ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		stoped = true;
	}
	conditionVariable.notify_all();
	for (std::thread &executor : executors) {
		if (executor.joinable()) {
			executor.join();
		}
		else {
			executor.detach();
		}
	}
}
