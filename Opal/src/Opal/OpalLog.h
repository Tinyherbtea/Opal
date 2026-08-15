#pragma once
#include"opalpch.h"
#include<stdio.h>
#ifdef OPALLOG_EXPORTS
#define OPALLOG_API __declspec(dllexport)
#else
#define OPALLOG_API __declspec(dllimport)
#endif

#define MAX_REGISTER_SINK 16
#define MAX_LOG_SIZE 1024
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
namespace OpalLog
{
	extern OPALLOG_API std::string pattern;
	OPALLOG_API void pattern_set(const std::string& string);
	typedef unsigned long long size;
	typedef enum Level
	{
		trace,//追踪
		debug,//调试
		info,//信息
		warn,
		error
	};
	typedef enum SinkType
	{
		Console,
		File
	};
	//----------队列----------
	typedef struct Queue
	{
		void* buffer;
		size capacity;
		size mask;

		volatile size push_line;
		volatile size pop_line;
	}Queue;
	OPALLOG_API int Queue_Init(Queue* queue, void* buffer, size capacity);
	OPALLOG_API int push(Queue* queue, const char* message, size logsize);
	OPALLOG_API int pop(Queue* queue, char* buffer, unsigned int buffersize, unsigned int* out_logsize);
	//----------格式化处理----------
	inline std::string to_str(const char* value)
	{
		return value ? value : "(null)";
	}
	inline std::string to_str(const std::string& value) {
		return value;
	}
	template <typename T>
	inline std::string to_str(const T& value) {
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}
	inline std::string argsformat(const std::string& format) {
		return format;
	}
	template <typename T, typename... Args>
	std::string argsformat(const std::string& format, T&& arg, Args&&... args) {
		size_t pos = format.find("{}");
		std::string str_of_arg = to_str(std::forward<T>(arg));

		if (pos == std::string::npos) {
			return format + str_of_arg + argsformat("", std::forward<Args>(args)...);
		}

		std::string result = format.substr(0, pos) + str_of_arg;
		result += argsformat(format.substr(pos + 2), std::forward<Args>(args)...);

		return result;
	}
	//----------sink----------
	class OPALLOG_API Sink
	{
	protected:
		Queue queue;
		void* buffer;
		std::thread thread;
		volatile bool running;
		virtual void Sent(const char* msg, unsigned int len) = 0;
	public:
		Sink(size size) :running(true)
		{
			buffer = new char[size];
			Queue_Init(&queue, buffer, size);
			thread = std::thread([this]()
				{
					char logbuffer[MAX_LOG_SIZE];//单条日志消息的最大长度
					unsigned int logsize;

					int idle_count = 0;
					while (running)
					{
						bool has_data = false;
						while (pop(&queue, logbuffer, sizeof(logbuffer), &logsize))
						{
							Sent(logbuffer, logsize);
							has_data = true;
						}
						if (has_data) { idle_count = 0; }
						else
						{
							if (idle_count < 100) std::this_thread::yield();
							else std::this_thread::sleep_for(std::chrono::milliseconds(1));
							idle_count++;
						}
					}
				});
		}
		virtual ~Sink()
		{
			running = false;
			if (thread.joinable()) {
				thread.join();
			}
			delete[](char*)buffer;
		}
		Queue* GetQueue() { return &queue; }
	};
	class OPALLOG_API ConsoleSink : public Sink
	{
	protected:
		void Sent(const char* m, unsigned int l) override
		{
			std::string msg;
			msg.reserve(l + 1);
			msg.append(m, l);
			msg += '\n';
			std::cout.write(msg.c_str(),msg.size());
			std::cout.flush();
		}
	public:
		ConsoleSink(size s) : Sink(s) {}
		~ConsoleSink() { }
	};
	class OPALLOG_API FileSink : public Sink
	{
	protected:
		void Sent(const char* m, unsigned int l) override
		{
			if (!file.is_open() || file.fail())
			{
				std::cerr << "Log file error!" << std::endl;
				return;
			}
			file.write(m, l);
			file.put('\n');
			file.flush();
		}
	private:
		std::ofstream file;
	public:
		FileSink(size s, const char* path) :Sink(s), file(path) {}
		~FileSink() { if (file.is_open()) {  file.close(); } }
	};
	OPALLOG_API Sink* CreateSink(SinkType type, size Size);
	//----------sink_route----------
	struct SinkRoute
	{
		Queue* queue;
		Level level;
		std::string sinkname;
	};
	extern OPALLOG_API SinkRoute sinkroute[MAX_REGISTER_SINK];
	extern OPALLOG_API int sink_count;
	OPALLOG_API void RegisterSink(Queue* queue, Level level, const char* sinkname);
	//----------Log----------
	inline const char* GetColorCode(Level level) {
		switch (level) {
		case trace: return "\033[32m"; // 绿色
		case debug:	return "\033[34m"; // 蓝色
		case info:  return "\033[32m"; // 绿色
		case warn:	return "\033[33m"; // 黄色
		case error:	return "\033[31m"; // 红色
		default:              return "\033[0m";  // 默认白色
		}
	}
	template <typename... Args>
	void Log(Level level, const char* fmt, Args&&... args)
	{


		std::string user_msg = argsformat(fmt, std::forward<Args>(args)...);

		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;
		std::tm tm;
		localtime_s(&tm, &time);

		std::string message;
		message.reserve(MAX_LOG_SIZE);

		size pos = 0;
		while (pos < pattern.size())
		{
			size next_pos = pattern.find('%', pos);
			if (next_pos == std::string::npos) {
				message.append(pattern, pos, std::string::npos);
				break;
			}
			message.append(pattern, pos, next_pos - pos);

			if (next_pos + 1 >= pattern.size())
				break;

			char placeholder = pattern[next_pos + 1];
			switch (placeholder)
			{
			case '^':
				message += GetColorCode(level); break;
			case '$':
				message += "\033[0m"; break;
			case 'v':
				message += user_msg; break;
			case 'Y':
				message += std::to_string(tm.tm_year + 1900); break;
			case 'm':
				message += (tm.tm_mon < 9 ? "0" : "") + std::to_string(tm.tm_mon + 1); break;
			case 'd':
				message += (tm.tm_mday < 10 ? "0" : "") + std::to_string(tm.tm_mday); break;
			case 'H':
				message += (tm.tm_hour < 10 ? "0" : "") + std::to_string(tm.tm_hour); break;
			case 'M':
				message += (tm.tm_min < 10 ? "0" : "") + std::to_string(tm.tm_min); break;
			case 'S':
				message += (tm.tm_sec < 10 ? "0" : "") + std::to_string(tm.tm_sec); break;
			case 'f':
			{
				char buf[7];
				snprintf(buf, sizeof(buf), "%06lld", (long long)micros.count());
				message += buf;
				break;
			}
			case 'T':
				message += (tm.tm_hour < 10 ? "0" : "") + std::to_string(tm.tm_hour) + ":" +
					(tm.tm_min < 10 ? "0" : "") + std::to_string(tm.tm_min) + ":" +
					(tm.tm_sec < 10 ? "0" : "") + std::to_string(tm.tm_sec);
				break;
			case 'F':
				message += std::to_string(tm.tm_year + 1900) + "-" +
					(tm.tm_mon < 9 ? "0" : "") + std::to_string(tm.tm_mon + 1) + "-" +
					(tm.tm_mday < 10 ? "0" : "") + std::to_string(tm.tm_mday);
				break;
			case 'L': // 日志级别
				switch (level)
				{
				case trace: message += "TRACE"; break;
				case debug: message += "DEBUG"; break;
				case info:  message += "INFO";  break;
				case warn:  message += "WARN";  break;
				case error:   message += "ERROR"; break;
				default:              message += "UNKNOWN"; break;
				}
				break;
			case 'n':
				message += "__SINK_NAME__";
				break;
			default:
				message.push_back(placeholder);
				break;
			}
			pos = next_pos + 2;
		}
		void* p = &sinkroute;
		for (int i = 0; i < sink_count; ++i)
		{
			if (level >= sinkroute[i].level)
			{
				std::string msg = message;
				size_t sink_name_pos = msg.find("__SINK_NAME__");
				if (sink_name_pos != std::string::npos) {
					msg.replace(sink_name_pos, 13, sinkroute[i].sinkname);
				}
				push(sinkroute[i].queue, msg.c_str(), (unsigned int)msg.size());
			}
		}
	}
}
