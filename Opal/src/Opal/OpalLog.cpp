#include"opalpch.h"
#include "OpalLog.h"
namespace OpalLog
{
	OPALLOG_API std::string pattern= "%^ %T %n %L: %v %$";
	void pattern_set(const std::string& string)
	{
		pattern = string;
	}
	Logger::Logger(std::string name, int mainqueuesize)
		:Logger_name(name), LogQueue(mainqueuesize), StopRequested(false)
	{

	}
	Logger::~Logger()
	{
		Stopping();
	}
	void Logger::RegisterSink(const std::shared_ptr<Logsink> Sink, int QueueSize)
	{
		SinkQueues.emplace_back(Sink, std::make_unique<LockfreeQueue<LogMessage>>(QueueSize));
		//std::cerr << "[Logger] Registered sink for logger '" << Logger_name << "' this=" << static_cast<const void*>(this)
		//		  << " LogQueue=" << static_cast<const void*>(&LogQueue) << " sinkQueue=" << static_cast<const void*>(SinkQueues.back().second.get()) << "\n";
	}
	//--------------------线程处理----------------------

	void Logger::thread()
	{
		while (1)
		{
			LogMessage msg;
			if (!LogQueue.out(msg))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			//std::cerr << "[Logger::thread] popped message for logger='" << Logger_name << "' LogQueue=" << static_cast<const void*>(&LogQueue)
			//		  << " message='" << msg.message << "'\n";
			for (auto& [sink, queue] : SinkQueues)
			{
				if (msg.level >= sink->getLevel())
				{
					//std::cerr << "[Logger::thread] direct deliver to sink for logger='" << Logger_name << "'\n";
					queue->join(msg);
				}
			}
			if (StopRequested)
			{
				break;
			}
		}
	}
	void Logger::Sinkthread(LockfreeQueue<LogMessage>& queue, Logsink* Sink)
	{
		//std::cerr << "[Sinkthread] started for logger '" << Logger_name << "' this=" << static_cast<const void*>(this)
		//		  << " sinkQueue=" << static_cast<const void*>(&queue) << " thread id=" << std::this_thread::get_id() << "\n";
		while (1)
		{
			LogMessage msg;
			if (!queue.out(msg))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			//std::cerr << "[Sinkthread] delivering from sinkQueue=" << static_cast<const void*>(&queue) << " message='" << msg.message << "'\n";
			mutex.lock();
			Sink->log(msg.message);
			mutex.unlock();
			if (StopRequested)
			{
				break;
			}
		}
	}
	void Logger::StartLoging()
	{
		StopRequested.store(false);
		//std::cerr << "[Logger] Starting threads for logger '" << Logger_name << "' this=" << static_cast<const void*>(this)
		//		  << " LogQueue=" << static_cast<const void*>(&LogQueue) << "\n";
		LogThread = std::thread(&Logger::thread, this);
		for (auto& [sink, queue] : SinkQueues)
		{
			SinkThread.emplace_back(&Logger::Sinkthread, this, std::ref(*queue), sink.get());
			//std::cerr << "[Logger::StartLoging] SinkThread created for logger '" << Logger_name
			//		  << "' sinkQueue=" << static_cast<const void*>(queue.get()) << " joinable=" << SinkThread.back().joinable()
			//		  << " id=" << SinkThread.back().get_id() << "\n";
		}
	}
	void Logger::Stopping()
	{
		//std::cerr << "[Logger] Stopping threads for logger '" << Logger_name << "' this=" << static_cast<const void*>(this)
		//	<< " LogQueue=" << static_cast<const void*>(&LogQueue) << "\n";
		StopRequested = true;
		if (LogThread.joinable())
		{
			LogThread.join();
		}

		for (auto& s : SinkThread)
		{
			if (s.joinable())
			{
				s.join();
			}
		}
		SinkThread.clear();
	}
}
