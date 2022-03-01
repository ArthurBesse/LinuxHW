#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <atomic>
#include <functional>
#include <string>
#include <queue>
#include <condition_variable>
#include <logger.h>

namespace rnd
{

template<class T>
class RandomGenerator;

template<>
class RandomGenerator<int>
{
	std::random_device m_device;
	std::mt19937 m_random;
	std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
public:
	RandomGenerator(int left, int right)
		: m_device()
		, m_random(m_device())
		, m_distribution(left, right)
	{
	}

	int generate() 
	{
		return m_distribution(m_random);
	}

};

template<>
class RandomGenerator<std::string>
{
	std::size_t m_lenght;
	RandomGenerator<int> m_generator;

public:
        RandomGenerator(std::size_t lenght)
                : m_lenght(lenght)
                , m_generator('a', 'z')
        {
        }

        std::string generate()
        {
		std::string res(m_lenght, '*');
		for(auto& c : res)
			c = this->m_generator.generate();
                return res;
        }

};

};

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
	std::atomic<bool> m_stopped;
public:
	Producer(QueueWrapper<T>& q, std::function<T()> generator)
		: m_queue(q)
		, m_generator(generator)
		, m_stop_requested(false)
		, m_stopped(true)
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
			m_stopped = false;
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
		if(true == this->m_stopped) return;

		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Shuting down producer...");
		this->m_stop_requested = true;
		this->m_queue.m_cv.notify_all();
		if (this->m_runner && m_runner->joinable())
			this->m_runner->join();
		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Producer has been shut down");
		this->m_stopped = true;
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
				std::lock_guard<std::mutex> lg(this->m_queue.m_mutex);
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
	std::atomic<bool> m_stopped;
public:
	Consumer(QueueWrapper<T>& q, std::function<void(T const&)> callback)
		: m_queue(q)
		, m_callback(callback)
		, m_stop_requested(false)
		, m_stopped(true)
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
			m_stopped = false;
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
		if(true == this->m_stopped) return;

		Logger::logf(Logger::INFO, __FILE__, __LINE__, "Shuting down consumer...");
                this->m_stop_requested = true;
                this->m_queue.m_cv.notify_all();
                if (this->m_runner && m_runner->joinable())
                        this->m_runner->join();
                Logger::logf(Logger::INFO, __FILE__, __LINE__, "Consumer has been shut down");
		this->m_stopped = true;
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
				std::lock_guard<std::mutex> lg(this->m_queue.m_mutex);
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
#define ITEM_TYPE_FORMAT "%s"

int main(int argc, char const** argv)
{
	Logger logger(nullptr, true, false);
	QueueWrapper<ItemType> qw(100);


	rnd::RandomGenerator<ItemType> rg(10);
	Consumer<ItemType> c(qw, [](ItemType const& val) { Logger::logf(Logger::INFO, __FILE__, __LINE__, "Consumer received: " ITEM_TYPE_FORMAT, val.c_str()); });
	Producer<ItemType> p(qw, [&rg] () { return rg.generate(); });

        c.start();
        p.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	//p.stop();
	//c.stop();
	return 0;
}
