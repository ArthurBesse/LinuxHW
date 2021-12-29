#include <chrono>
#include <thread>
#include <atomic>
#include <string.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h> 
#include <logger.h>

class DirWatcherCallbackBase
{
public:
    DirWatcherCallbackBase(void) = default;
    virtual ~DirWatcherCallbackBase(void) = default;

    enum action_t { CREATE, MODIFY, DELETE, UNEXPECTED_ACTION };
    enum file_t { DIRECTORY, REGULAR, UNEXPECTED_FILE };

    static char const* get_action_str(action_t action)
    {
        switch (action)
        {
        case DirWatcherCallbackBase::CREATE:
            return "created";
        case DirWatcherCallbackBase::MODIFY:
            return "modified";
        case DirWatcherCallbackBase::DELETE:
            return "deleted";
        case DirWatcherCallbackBase::UNEXPECTED_ACTION:
            throw std::runtime_error("Unexpected action has been detected");
        default:
            return nullptr;
        }
    }

    static char const* get_file_str(file_t file)
    {
        switch (file)
        {
        case DirWatcherCallbackBase::DIRECTORY:
            return "Directory";
        case DirWatcherCallbackBase::REGULAR:
            return "Regular file";
        case DirWatcherCallbackBase::UNEXPECTED_FILE:
            throw std::runtime_error("Action has been detected for unexpected file type");
        default:
            return nullptr;
        }
    }

    virtual void log(action_t action, file_t file, std::string const& name) const = 0;
};

void DirWatcherCallbackBase::log(action_t action, file_t file, std::string const& name) const
{
    std::cout << "The " << get_file_str(file) << " " << name << " has been " << get_action_str(action) << std::endl;
}


class DirWatcher final
{
    static constexpr size_t name_len = 1024;
    static constexpr size_t event_size = sizeof(struct inotify_event);
    static constexpr size_t buf_len = event_size + name_len;
    static inline DirWatcher* this_ptr = nullptr;

    std::atomic<bool> stop_flag;
    std::unique_ptr<std::thread> runner;

    int wd;
    int fd;

public:

    DirWatcher(std::string const& path) : stop_flag(false), wd(-1), fd(-1)
    {
        if(this_ptr)
            throw std::runtime_error("Only one instance of DirWatcher can be created");

        if(0 > (fd = inotify_init()))
            throw std::runtime_error(strerror(errno));

        if (0 > fcntl(fd, F_SETFL, O_NONBLOCK) || 0 > (wd = inotify_add_watch(fd, path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE)))
            throw std::runtime_error(strerror(errno));

        this_ptr = this;
    }

    ~DirWatcher(void)
    {
        stop_flag = true;
        runner->join();
        inotify_rm_watch(fd, wd);
        close(fd);
        this_ptr = nullptr;
    }

    DirWatcher(const DirWatcher&) = delete;

    DirWatcher& operator=(const DirWatcher&) = delete;

    DirWatcher(DirWatcher&&) = delete;

    DirWatcher& operator=(DirWatcher&&) = delete;

    void run(DirWatcherCallbackBase* callback)
    {
        runner.reset(new std::thread(&DirWatcher::run_internal, this, callback));
    }

    void wait()
    {
        runner->join();
    }

    static DirWatcher* get_instance(void)
    {
        return DirWatcher::this_ptr;
    }

    static void stop()
    {	
	if(this_ptr)
	    this_ptr->~DirWatcher();
    }

private:

    void run_internal(DirWatcherCallbackBase * callback)
    {
        while (false == stop_flag.load(std::memory_order_relaxed))
        {
	    using namespace std::chrono_literals;
            int i = 0, length;
            char buffer[buf_len];

            length = read(fd, buffer, buf_len);

            while (i < length)
            {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);
                
                if (event->len > 0)
                {
                    DirWatcherCallbackBase::action_t action = DirWatcherCallbackBase::UNEXPECTED_ACTION;
                    DirWatcherCallbackBase::file_t file = DirWatcherCallbackBase::UNEXPECTED_FILE;

                    if (event->mask & IN_CREATE)
                        action = DirWatcherCallbackBase::CREATE;
                    else if (event->mask & IN_DELETE)
                        action = DirWatcherCallbackBase::DELETE;
                    else if (event->mask & IN_MODIFY)
                        action = DirWatcherCallbackBase::MODIFY;

                    if (event->mask & IN_ISDIR)
                        file = DirWatcherCallbackBase::DIRECTORY;
                    else
                        file = DirWatcherCallbackBase::REGULAR;

                    callback->log(action, file, event->name);
                }
                i += event_size + event->len;
		std::this_thread::sleep_for(50ms);
            }
        }
    }
};


class DirWatcherCallback final : public DirWatcherCallbackBase
{
    virtual void log(action_t action, file_t file, std::string const& name) const override
    {
        if (DirWatcherCallbackBase::UNEXPECTED_ACTION == action && DirWatcherCallbackBase::UNEXPECTED_FILE == file)
            Logger::logf(Logger::WARNING, __FILE__, __LINE__, "Unexpected action has been detected for unexpected file type: %s", name.c_str());
        else if (DirWatcherCallbackBase::UNEXPECTED_ACTION == action)
            Logger::logf(Logger::WARNING, __FILE__, __LINE__, "Unexpected action has been detected for %s %s", get_file_str(file), name.c_str());
        else if (DirWatcherCallbackBase::UNEXPECTED_FILE == file)
            Logger::logf(Logger::WARNING, __FILE__, __LINE__, "Unexpected file %s has been %s", name.c_str(), get_action_str(action));
        else
            Logger::logf(Logger::INFO, __FILE__, __LINE__, "%s %s has been %s", get_file_str(file), name.c_str(), get_action_str(action));
    }
};



void sig_handler(int sig)
{
    DirWatcher::stop();
    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv)
{
    Logger logger(nullptr, false, false, true);

    if(2 != argc)
    {
	Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Invalid arguments count: %d", argc - 1);
	exit(EXIT_FAILURE);
    }
    signal(SIGINT, sig_handler);

    DirWatcher watcher(argv[1]);
    DirWatcherCallback cb;
    watcher.run(&cb);
    watcher.wait();
    return 0;
}
