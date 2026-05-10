#pragma once
#include<string>
#include<sstream>
#include<fstream>
#include<iostream>
#include<vector>
#include<thread>
#include<memory>
#include<chrono>
#include<atomic>
#include <utility>
#include<mutex>

#ifdef OPALLOG_EXPORTS
#define OPALLOG_API __declspec(dllexport)
#else
#define OPALLOG_API __declspec(dllimport)
#endif

namespace OpalLog
{
	extern OPALLOG_API std::string pattern;
	void pattern_set(const std::string& string);
	enum class  Level
	{
		trace,//追踪
		debug,//调试
		info,//信息
		warn,
		error
	};
	//日志系统
	//日志格式pattern标识符：
	//"%v":日志信息
	//"%Y":四位数的年份
	//"%m":两位数的月份
	//"%d":两位数的日期
	//"%H":两位数的小时
	//"%M":两位数的分钟
	//"%S":两位数的秒钟
	//"%T":%H:%M:%S
	//"%F":%Y-%m-%d
	//"%L":日志级别
	//"%^":开始颜色编码
	//"%n":日志记录器名称
	//--------日志接收器基类--------
	class  Logsink
	{
	public:
		virtual ~Logsink() = default;
		virtual void log(const std::string& message) = 0;
		virtual void setLevel(Level level) = 0;
		virtual Level getLevel() const = 0;
	};
	//--------控制台接收器-----------
	class  Consolesink : public Logsink
	{
	public:
		void log(const std::string& message) override
		{
			std::cout << message << std::endl;
		}
		void setLevel(Level level) override
		{
			LogLevel = level;
		}

		Level getLevel() const override
		{
			return LogLevel;
		}
	private:
		Level LogLevel{ Level::info };
	};
	//----------文件接收器-----------
	class  Filesink : public Logsink
	{
	public:
		explicit Filesink(const std::string& filename) : file(filename, std::ios::app)
		{
			if (!file.is_open())
			{
				throw std::runtime_error("Failed to open file for logging.");
			}
		}
		void log(const std::string& message) override
		{
			file << message << std::endl;
		}
		void setLevel(Level level) override
		{
			LogLevel = level;
		}

		Level getLevel() const override
		{
			return LogLevel;
		}
	private:
		Level LogLevel{ Level::info };
		std::ofstream file;
	};
	//-----------数组实现的无锁队列------------
	template<typename T>
	class  LockfreeQueue
	{
	public:
		LockfreeQueue(size_t capacity) :Capacity(capacity), Head(0), Tail(0)
		{
			Buffer.resize(capacity);
		};
		//入队操作
		bool join(const T& item) {
			size_t tail = Tail.load(std::memory_order_relaxed);
			size_t nextTail = (tail + 1) % Capacity;

			if (nextTail == Head.load(std::memory_order_acquire)) {
				return false; //队列已满
			}

			Buffer[tail] = item;
			Tail.store(nextTail, std::memory_order_release);
			return true;
		}
		//出队操作
		bool out(T& item) {
			size_t head = Head.load(std::memory_order_relaxed);

			if (head == Tail.load(std::memory_order_acquire)) {
				return false; //队列为空
			}

			item = Buffer[head];
			Head.store((head + 1) % Capacity, std::memory_order_release);
			return true;
		}
		LockfreeQueue(LockfreeQueue&& other) noexcept
			: Buffer(std::move(other.Buffer)),
			Capacity(other.Capacity),
			Head(other.Head.load()),
			Tail(other.Tail.load())
		{
		}
		LockfreeQueue& operator=(LockfreeQueue&& other) noexcept
		{
			if (this != &other)
			{
				Buffer = std::move(other.Buffer);
				// Capacity 是 const，不能改变；这里假设两个队列构造时的 capacity 相同或移动后无需使用原 capacity 的值修改。
				Head.store(other.Head.load());
				Tail.store(other.Tail.load());
			}
			return *this;
		}

	private:
		std::vector<T> Buffer;
		const size_t Capacity;
		std::atomic<size_t> Head;
		std::atomic<size_t> Tail;
	};
	//-------------日志--------------
	class  Logger
	{
		//---------基础信息--------
	private:
		struct LogMessage
		{
			Level level{ Level::info };
			std::string message;

		};
	public:
		std::string Logger_name;
		Level Logger_level{ Level::info };
		//----------线程管理---------
	private:
		LockfreeQueue<LogMessage> LogQueue;//日志消息队列
		std::vector<std::pair<std::shared_ptr<Logsink>, std::unique_ptr<LockfreeQueue<LogMessage>>>> SinkQueues;//接收器队列
		std::thread  LogThread;//日志主线程
		std::vector<std::thread> SinkThread;//接收器线程
		std::atomic<bool> StopRequested{ false };//线程停止通知标志
		std::mutex mutex;
	public:
		Logger(std::string name, int mainqueuesize);
		~Logger();
		void RegisterSink(const std::shared_ptr<Logsink> Sink, int QueueSize);
		template<typename... Args>
		void Trace(const std::string& format, Args&&... args)
		{
			std::string message = Format(Level::trace, format, std::forward<Args>(args)...);
			LogMessage msg{ Level::trace, message };
			LogQueue.join(msg);
		}
		template<typename... Args>
		void Debug(const std::string& format, Args&&... args)
		{
			std::string message = Format(Level::debug, format, std::forward<Args>(args)...);
			LogMessage msg{ Level::debug, message };
			LogQueue.join(msg);
		}
		template<typename... Args>
		void Info(const std::string& format, Args&&... args)
		{
			std::string message = Format(Level::info, format, std::forward<Args>(args)...);
			LogMessage msg{ Level::info, message };
			LogQueue.join(msg);
		}
		template<typename... Args>
		void Warn(const std::string& format, Args&&... args)
		{
			std::string message = Format(Level::warn, format, std::forward<Args>(args)...);
			LogMessage msg{ Level::warn, message };
			LogQueue.join(msg);
		}
		template<typename... Args>
		void Error(const std::string& format, Args&&... args)
		{
			std::string message = Format(Level::error, format, std::forward<Args>(args)...);
			LogMessage msg{ Level::error, message };
			LogQueue.join(msg);
		}
		void StartLoging();
		void Stopping();
	private:
		void thread();//线程函数，各个sink的函数
		void Sinkthread(LockfreeQueue<LogMessage>& queue, Logsink* Sink);
		template<typename... Args>
		std::string Format(Level level, const std::string& format, Args&&... args)
		{
			//---------------------------------获取时间信息-----------------------------------------
			auto now = std::chrono::system_clock::now();//获取当前时间点
			std::time_t time = std::chrono::system_clock::to_time_t(now);//将时间点转换为time_t类型
			std::tm tm;
			localtime_s(&tm, &time);//将time_t类型转换为tm结构体，包含年月日时分秒等信息
			//-------------------------------以字符串方式获取格式-----------------------------------
			size_t size = snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...) + 1;//通过snprintf的返回值获取格式化字符串的长度
			std::unique_ptr<char> buffer(new char[size]);										 //创建一个智能指针来管理格式化字符串的内存
			snprintf(buffer.get(), size, format.c_str(), std::forward<Args>(args)...);			 //将格式化字符串写入缓冲区
			std::string message(buffer.get(), buffer.get() + size - 1);						 //将格式化字符串转换为std::string对象
			//---------------------------------------------------------------------------------------
			std::stringstream ss;
			char placeholder;
			size_t pos = 0;
			while (pos != std::string::npos)
			{
				size_t nextpos = pattern.find("%", pos);
				if (nextpos == std::string::npos)
				{
					ss << pattern.substr(pos);
					break;
				}
				ss << pattern.substr(pos, nextpos - pos);
				if (nextpos + 1 >= pattern.size())
				{
					break;
				}
				placeholder = pattern[nextpos + 1];//提取占位符
				switch (placeholder)
				{
				case'^':
					ss << colorcode(level);//这里只加入开始的颜色编码，因为接收器的log函数会自动加入结束颜色编码
					break;
				case'$':
					ss << "\033[0m";
					break;
				case'v':
					ss << message;
					break;
				case 'Y':
					ss << tm.tm_year + 1900;//tm.tm_year是从1900年开始的，所以需要加上1900
					break;
				case 'm':
					ss << (tm.tm_mon < 9 ? "0" : "") << tm.tm_mon + 1;
					break;
				case 'd':
					ss << (tm.tm_mday < 10 ? "0" : "") << tm.tm_mday;
					break;
				case 'H':
					ss << (tm.tm_hour < 10 ? "0" : "") << tm.tm_hour;
					break;
				case 'M':
					ss << (tm.tm_min < 10 ? "0" : "") << tm.tm_min;
					break;
				case 'S':
					ss << (tm.tm_sec < 10 ? "0" : "") << tm.tm_sec;
					break;
				case 'T':
					ss << (tm.tm_hour < 10 ? "0" : "") << tm.tm_hour << ":" << (tm.tm_min < 10 ? "0" : "") << tm.tm_min << ":" << (tm.tm_sec < 10 ? "0" : "") << tm.tm_sec;
					break;
				case 'F':
					ss << tm.tm_year + 1900 << "-" << (tm.tm_mon < 9 ? "0" : "") << tm.tm_mon + 1 << "-" << (tm.tm_mday < 10 ? "0" : "") << tm.tm_mday;
					break;
				case 'L':
					switch (level)
					{
					case Level::trace:
						ss << "TRACE";
						break;
					case Level::debug:
						ss << "DEBUG";
						break;
					case Level::info:
						ss << "INFO";
						break;
					case Level::warn:
						ss << "WARN";
						break;
					case Level::error:
						ss << "ERROR";
						break;
					}
					break;
				case'n':
					ss << Logger_name;
					break;
				default:
					ss << placeholder;
				}
				pos = nextpos + 2;
			}
			return ss.str();
		}
		std::string colorcode(Level level)
		{
			switch (level)
			{
			case Level::trace:
				return "\033[32m";//绿色
			case Level::debug:
				return "\033[34m";//蓝色
			case Level::info:
				return "\033[32m";//绿色
			case Level::warn:
				return "\033[33m";//黄色
			case Level::error:
				return "\033[31m";//红色
			default:
				return"";
			}
		}
	};
	class  LogRegister
	{
	public:
		static std::shared_ptr<Logger> Console_(const std::string& loggerName)
		{
			std::shared_ptr<Logger> logger = std::make_shared<Logger>(loggerName, 1024);
			std::shared_ptr<Consolesink> sink = std::make_shared<Consolesink>();
			sink->setLevel(Level::trace);
			logger->RegisterSink(sink, 1024);
			return logger;
		}
		static std::shared_ptr<Logger> File_(const std::string& loggerName, const std::string& filename)
		{
			std::shared_ptr<Logger> logger = std::make_shared<Logger>(loggerName, 1024);
			std::shared_ptr<Filesink> sink = std::make_shared<Filesink>(filename);
			sink->setLevel(Level::trace);
			logger->RegisterSink(sink, 1024);
			return logger;
		}
	};
}
