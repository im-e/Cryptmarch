#pragma once
#include <cstdio>

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

public:

	static void setLevel(LogLevel newLevel)
	{
		level = newLevel;
	}


	template<typename ... Args> //declares template pack, args stands for a list or zero or more types
	static void Trace(const char* message, Args... args) {
		//const char* is the format string

		if (level <= TraceLevel)
		{
			printf("[TRACE]\t"); //trace, tab
			printf(message, args...); //string then unpack args
			printf("\n"); //new line
		}
	}

	template<typename ... Args> //declares template pack, args stands for a list or zero or more types
	static void Debug(const char* message, Args... args) {
		//const char* is the format string

		if (level <= DebugLevel)
		{
			printf("[DEBUG]\t"); //trace, tab
			printf(message, args...); //string then unpack args
			printf("\n"); //new line
		}
	}

	template<typename ... Args> //declares template pack, args stands for a list or zero or more types
	static void Info(const char* message, Args... args) {
		//const char* is the format string

		if (level <= InfoLevel)
		{
			printf("[INFO]\t"); //trace, tab
			printf(message, args...); //string then unpack args
			printf("\n"); //new line
		}
	}

	template<typename ... Args> //declares template pack, args stands for a list or zero or more types
	static void Error(const char* message, Args... args) {
		//const char* is the format string

		if (level <= ErrorLevel)
		{
			printf("[ERROR]\t"); //trace, tab
			printf(message, args...); //string then unpack args
			printf("\n"); //new line
		}
	}

	template<typename ... Args> //declares template pack, args stands for a list or zero or more types
	static void Critical(const char* message, Args... args) {
		//const char* is the format string

		if (level <= CriticalLevel)
		{
			printf("[CRITICAL]\t"); //trace, tab
			printf(message, args...); //string then unpack args
			printf("\n"); //new line
		}
	}

};

