#include <thread>
#include <iostream>
#include <string>
#include <future>
#include <memory>
#include <list>
#include <optional>
#include <queue>
#include <chrono>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <logger.h>


class ThreadPool final
{

public:
	enum class priority_t { LOW, MINOR, HIGH, CRITICAL };

	ThreadPool(std::size_t capacity)
		: m_capacity(capacity)
		, m_stopped(true)
	{
		if (capacity > std::thread::hardware_concurrency())
			Logger::logf(Logger::WARNING, __FILE__, __LINE__, "Requested threads count is %d while reccomended count is %d. This may result in slow operation or unexpected termination of program.", 
				m_capacity, std::thread::hardware_concurrency());
	}

	~ThreadPool(void)
	{
		this->stop();
	}

	ThreadPool(ThreadPool&&) = delete;
	ThreadPool(ThreadPool const&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool const&) = delete;

	template<class T, class ...Args>
	std::optional<std::future<typename std::function<T(Args...)>::result_type>> 
		prepare_task(std::function<T(Args...)> task, priority_t priority, Args const& ... args)
	{
		if constexpr (false == std::is_void<T>::value)
		{
			using RetType = typename decltype(task)::result_type;
			auto promise = std::make_shared<std::promise<RetType> >();
			std::future<RetType> future = promise->get_future();

			this->prepare_task_internal
			(
				[task, promise, args...]()
				{
					try
					{
						promise->set_value(task(args...));
					}
					catch (std::exception const& e)
					{
						try{ promise->set_exception(std::current_exception()); }	
						catch (...) 
						{
							Logger::logf(Logger::ERROR, __FILE__, __LINE__, "%s", e.what());
						}
					}

				}, priority
			);
			return future;
		}
		else
		{
			this->prepare_task_internal
			(
				[task, args...]()
				{
					try
					{
						task(args...);
					}
					catch (const std::exception& e)
					{
						Logger::logf(Logger::ERROR, __FILE__, __LINE__, "%s", e.what());
					}
				}, priority
			);
			return std::nullopt;
		}
	}

	void start(void)
	{
		try 
		{
			this->m_stopped = false;
			this->start_internal();
		}
		catch (std::exception const& e)
		{
			Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Error occured in ThreadPool::start(): %s", e.what());
			this->stop();
		}
		catch (...)
		{
			Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Unexpected error occured in ThreadPool::start()");
			this->stop();
		}
	}

	void stop(void)
	{
		try
		{
			if (true == this->m_stopped) return;		
			this->m_stop_requested = true;
			m_cv.notify_all();
			for (auto& thread : m_threads)
				if(thread.joinable()) thread.join();
			this->m_stopped = true;
		}
		catch (std::exception const& e)
                {
                        Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Error occured in ThreadPool::stop(): %s", e.what());
                        this->stop();
                }
                catch (...)
                {
                        Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Unexpected error occured in ThreadPool::stop()");
                        this->stop();
                }
	}
private:

	void start_internal(void)
	{
		m_threads.reserve(m_capacity);
		for (size_t i = 0; i < m_capacity; i++)
		{
			m_threads.emplace_back([this]()
				{
					while (false == this->m_stop_requested.load())
					{
						Task task;
						std::unique_lock<std::mutex> ul(m_mutex);
						m_cv.wait(ul, 
							[this, &task] () 
							{ 
								if (m_stop_requested)
									return true;
								if (m_queue.size())
									return task = m_queue.top(), m_queue.pop(), true;
								return false;
							}
						);
						ul.unlock();
						task();
					}
				});
		}
	}

	template<class T>
	void prepare_task_internal(T const& task, ThreadPool::priority_t priority)
	{
		{
			std::lock_guard<std::mutex> lg(m_mutex);
			m_queue.emplace(std::function<void()>(task), priority);
		}
		m_cv.notify_one();
	}
	

	struct Task
	{
		std::function<void()> m_task;
		priority_t m_priority;
		bool operator<(Task const& task) const noexcept
		{
			return this->m_priority < task.m_priority;
		}
		void operator()() const
		{
			if(m_task)
				m_task();
		}
		Task(std::function<void()> task, priority_t priority)
			: m_task(task)
			, m_priority(priority)
		{

		}
		Task()
			: m_priority(ThreadPool::priority_t::MINOR)
		{}
	};
	std::priority_queue<Task> m_queue;
	std::vector<std::thread> m_threads;
	std::mutex m_mutex;
	std::atomic_bool m_stop_requested;
	std::condition_variable m_cv;
	std::size_t m_capacity;
	bool m_stopped;
};


int main(int argc, char** argv)
{
	Logger logger(nullptr, true, false);
	using namespace std::chrono_literals;
	ThreadPool tp(4);
	tp.start();
	int x, y;
	std::cin >> x >> y;
	{
		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Tests with return type started");
		/*============== TEST WITH RETURN TYPE ==============*/
		std::function<int(int, int)> fmul = [](int x, int y) {
			return x * y;
		};
		std::function<int(int, int)> fdiv = [](int x, int y) {
			return x / y;
		};
		std::function<int(int, int)> fsub = [](int x, int y) {
			return x - y;
		};
		std::function<int(int, int)> fsum = [](int x, int y) {
			return x + y;
		};

		auto future_mul = tp.prepare_task(fmul, ThreadPool::priority_t::LOW, x, y);
		auto future_div = tp.prepare_task(fdiv, ThreadPool::priority_t::MINOR, x, y);
		auto future_sub = tp.prepare_task(fsub, ThreadPool::priority_t::HIGH, x, y);
		auto future_sum = tp.prepare_task(fsum, ThreadPool::priority_t::CRITICAL, x, y);

		try
		{
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d * %d \t=\t%d", x, y, future_mul->get());
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d / %d \t=\t%d", x, y, future_div->get());
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d - %d \t=\t%d", x, y, future_sub->get());
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d + %d \t=\t%d", x, y, future_sum->get());
//			std::cout << x << " * " << y << "\t=\t" << future_mul->get() << std::endl;
//			std::cout << x << " / " << y << "\t=\t" << future_div->get() << std::endl;
//			std::cout << x << " - " << y << "\t=\t" << future_sub->get() << std::endl;
//			std::cout << x << " + " << y << "\t=\t" << future_sum->get() << std::endl;
		}
		catch (std::exception const& e)
		{
			Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Error occured when trying to access returned values: %s", e.what());
			return 1;
		}
		catch (...)
		{
			Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Unexpected error occured when trying to access returned values");
			return 1;
		}
	}

	std::this_thread::sleep_for(300ms);

	/*============== TEST WITHOUT RETURN TYPE ==============*/
	{
		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Tests without return type started");
		std::function<void(int, int)> fmul = [](int x, int y) {
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d * %d \t=\t%d", x, y, x * y);
		};
		std::function<void(int, int)> fdiv = [](int x, int y) {
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d / %d \t=\t%d", x, y, x / y);
		};
		std::function<void(int, int)> fsub = [](int x, int y) {
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d - %d \t=\t%d", x, y, x - y);
		};
		std::function<void(int, int)> fsum = [](int x, int y) {
			Logger::logf(Logger::INFO, __FILE__, __LINE__, "%d + %d \t=\t%d", x, y, x + y);
		};

		auto future_mul = tp.prepare_task(fmul, ThreadPool::priority_t::CRITICAL, x, y);
		auto future_div = tp.prepare_task(fdiv, ThreadPool::priority_t::HIGH, x, y);
		auto future_sub = tp.prepare_task(fsub, ThreadPool::priority_t::MINOR, x, y);
		auto future_sum = tp.prepare_task(fsum, ThreadPool::priority_t::LOW, x, y);

		if (future_mul.has_value()
			|| future_div.has_value()
			|| future_sub.has_value()
			|| future_sum.has_value())
		{
			Logger::logf(Logger::FATAL, __FILE__, __LINE__, "Objects returned are expected to not contain values");
			return 1;
		}
		std::this_thread::sleep_for(1000ms);
	}

	return 0;
}

