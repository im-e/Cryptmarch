#pragma once
#include <iostream>

enum LogLevel {
	TraceLevel, DebugLevel, InfoLevel, ErrorLevel, CriticalLevel
};

class Logger
{
private:
	//inline tells the compiler and linker to merge the multiple definitions into one
	//keep private so that nothing else can access it
#ifdef _DEBUG
	inline static LogLevel level = TraceLevel; //if debug set to trace
#else
	inline static LogLevel level = ErrorLevel; //if release set to error only
#endif


	static void Log(LogLevel messageLevel, std::string prefix, std::string message)
	{
		if (level <= messageLevel)
		{
			std::cout << prefix << message << std::endl;
		}
	}

public:
	static void setLevel(LogLevel newLevel) 
	{
		level = newLevel; 
	}

	static void Trace(std::string message) 
	{
		Log(TraceLevel, "[TRACE]\t", message); 
	}

	static void Debug(std::string message)
	{
		Log(DebugLevel, "[DEBUG]\t", message);
	}

	static void Info(std::string message)
	{
		Log(InfoLevel, "[INFO]\t", message);
	}

	static void Error(std::string message)
	{
		Log(ErrorLevel, "[ERROR]\t", message);
	}

	static void Critical(std::string message)
	{
		Log(CriticalLevel, "[CRITICAL]\t", message);
	}
};

