#include"opalpch.h"
#include "OpalLog.h"
namespace OpalLog
{
	std::string pattern = "%^ %T %n %L: %v %$";
	SinkRoute sinkroute[MAX_REGISTER_SINK];
	int sink_count=0;

	void pattern_set(const std::string& string)
	{
		pattern = string;
	}
	int Queue_Init(Queue* queue, void* buffer, size capacity)
	{
		queue->buffer = buffer;
		queue->capacity = capacity;
		queue->mask = queue->capacity - 1;
		queue->push_line = 0;
		queue->pop_line = 0;
		return 1;
	}
	int push(Queue* queue, const char* message, size logsize)
	{
		unsigned int total_size = sizeof(unsigned int) + logsize;

		size push = queue->push_line;
		size pop = queue->pop_line;

		size write_line = queue->push_line & queue->mask;
		size tail_space = queue->capacity - write_line;
		if (tail_space < total_size)
		{
			if (tail_space >= sizeof(unsigned int))
				*(unsigned int*)((char*)queue->buffer + write_line) = 0;

			push += tail_space;
			write_line = 0;
		}
		
		if ((push - pop) >= (queue->capacity - total_size))
		{
			return 0;
		}

		*(unsigned int*)((char*)queue->buffer + write_line) = logsize;
		memcpy((char*)queue->buffer + write_line + sizeof(unsigned int), message, logsize);

		queue->push_line = push + total_size;
		return 1;
	}
	int pop(Queue* queue, char* buffer, unsigned int buffersize, unsigned int* out_logsize)
	{
		while(true)
		{
			size push = queue->push_line;
			size pop = queue->pop_line;

			size read_space = push - pop;

			if (read_space < sizeof(unsigned int))
			{
				return 0;
			}
			size read_line = pop & queue->mask;

			size tail_space = queue->capacity - read_line;
			if (tail_space < sizeof(unsigned int))
			{
				queue->pop_line = pop + tail_space;
				continue;
			}

			unsigned int logsize = *(unsigned int*)((char*)queue->buffer + read_line);

			if (logsize == 0)
			{
				queue->pop_line = pop + tail_space;
				continue;
			}

			if (read_space < sizeof(unsigned int) + logsize || logsize > buffersize)
			{
				return 0;
			}

			memcpy(buffer, (char*)queue->buffer + read_line + sizeof(unsigned int), logsize);

			queue->pop_line = pop + sizeof(unsigned int) + logsize;

			*out_logsize = logsize;
			return 1;
		}
	}
	Sink* CreateSink(SinkType type, size Size)
	{
		if (type == Console) return new ConsoleSink(Size);
		if (type == File) return new FileSink(Size,"Log.txt");
		return nullptr;
	}
	void RegisterSink(Queue* queue, Level level,const char* sinkname)
	{
		if (sink_count < MAX_REGISTER_SINK)
		{
			sinkroute[sink_count++] = { queue, level ,sinkname};
		}
	}
}
