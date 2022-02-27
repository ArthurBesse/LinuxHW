#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <string>
#include <queue>
#include <condition_variable>
#include <logger.h>

template <class T>
struct QueueWrapper
{
	std::queue<T> m_queue;
	std::size_t const m_capacity;
	std::condition_variable m_cv;
	std::mutex m_mutex;

	QueueWrapper(std::size_t capacity)
		: m_capacity(capacity)
	{}
};


template<typename T>
class Producer final
{
	QueueWrapper<T>& m_queue;
	std::unique_ptr<std::thread> m_runner;
	std::function<T()> m_generator;
	std::atomic<bool> m_stop_requested;
	bool stopped;
	std::mutex m_mutex;
public:
	Producer(QueueWrapper<T>& q, std::function<T()> generator)
		: m_queue(q)
		, m_generator(generator)
		, m_stop_requested(false)
		, stopped(false)
	{
	}

	Producer(const Producer&) = delete;

	Producer& operator=(const Producer&) = delete;

	Producer(Producer&&) = delete;

	Producer& operator=(Producer&&) = delete;

	void start(void)
	{
		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Starting producer...");
		try
		{
			m_runner.reset(new std::thread(&Producer<T>::start_internal, this));
		}
		catch (const std::exception& e)
		{
			Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Error occured in Producer::start(): %s", e.what());
			this->stop();
		}
		catch (...)
		{
			Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Unexpected error occured in Producer::start()");
			this->stop();
		}
	}
	void stop(void)
	{
		if(true == this->stopped) return;

		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Shuting down producer...");
		this->m_stop_requested = true;
		this->m_queue.m_cv.notify_all();
		if (this->m_runner && m_runner->joinable())
			this->m_runner->join();
		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Producer has been shut down");
		this->stopped = true;
	}
	~Producer(void)
	{
		this->stop();
	}
private:
	void start_internal(void)
	{
		while (false == this->m_stop_requested.load(std::memory_order_consume))
		{
			while (this->m_queue.m_capacity > this->m_queue.m_queue.size())
			{
				this->m_queue.m_queue.push(m_generator());
				this->m_queue.m_cv.notify_one();
				if (true == this->m_stop_requested.load(std::memory_order_consume))
					break;
			}
			if (true == this->m_stop_requested.load(std::memory_order_consume))
				break;

			std::unique_lock<std::mutex> lk(this->m_queue.m_mutex);
			this->m_queue.m_cv.wait(lk, [this]() {return ((this->m_queue.m_capacity > this->m_queue.m_queue.size()) || this->m_stop_requested); });
		}
		this->m_queue.m_cv.notify_one();
	}
};

template<class T>
class Consumer final
{
	QueueWrapper<T>& m_queue;
	std::unique_ptr<std::thread> m_runner;
	std::function<void(T const&)> m_callback;
	std::atomic<bool> m_stop_requested;
	bool stopped;
public:
	Consumer(QueueWrapper<T>& q, std::function<void(T const&)> callback)
		: m_queue(q)
		, m_callback(callback)
		, m_stop_requested(false)
		, stopped(false)
	{
	}

	Consumer(const Consumer&) = delete;

	Consumer& operator=(const Consumer&) = delete;

	Consumer(Consumer&&) = delete;

	Consumer& operator=(Consumer&&) = delete;

	void start(void)
	{
		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Starting consumer...");
                try
                {
                        m_runner.reset(new std::thread(&Consumer<T>::start_internal, this));
                }
                catch (const std::exception& e)
                {
                        Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Error occured in Consumer::start(): %s", e.what());
			this->stop();
		}
                catch (...)
                {
                        Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Unexpected error occured in Consumer::start()");
			this->stop();
		}
	}
	void stop(void)
	{
		if(true == this->stopped) return;

		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Shuting down consumer...");
                this->m_stop_requested = true;
                this->m_queue.m_cv.notify_all();
                if (this->m_runner && m_runner->joinable())
                        this->m_runner->join();
                Logger::logf(Logger::INFO, __FILE__, __LINE__, "Consumer has been shut down");
		this->stopped = true;
	}
	~Consumer(void)
	{
		this->stop();
	}
private:
	void start_internal(void)
	{
		while (true)
		{
			while (this->m_queue.m_queue.size() > 0)
			{
				//std::this_thread::sleep_for(1000ms);
				this->m_callback(this->m_queue.m_queue.front());
				this->m_queue.m_queue.pop();	
				this->m_queue.m_cv.notify_one();
				if (true == this->m_stop_requested.load(std::memory_order_consume))
					break;
			}
			if (true == this->m_stop_requested.load(std::memory_order_consume))
				break;
			std::unique_lock<std::mutex> lk(this->m_queue.m_mutex);
			this->m_queue.m_cv.wait(lk, [this]() {return ((this->m_queue.m_queue.size() > 0) || this->m_stop_requested); } );
		}
		this->m_queue.m_cv.notify_one();
	}
};

using ItemType = std::string;

int main(int argc, char const** argv)
{
	Logger logger(nullptr);
	QueueWrapper<ItemType> qw(1000);
	Producer<ItemType> p(qw, []()
		{
			return "abacaba";
		});

	Consumer<ItemType> c(qw, [](ItemType const& val) { std::cout << val << std::endl; });

	p.start();
	c.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(3));
	p.stop();
	c.stop();
	return 0;
}
