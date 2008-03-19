#pragma once

typedef enum LogLevel
{
	Unimportant,
	Note,
	Debug,
	Warning,
	Critical
} LogLevel;

class ITuniacLog
{
public:
	virtual bool	Log(wchar_t * message, LogLevel Level = Note) = 0;
	virtual bool	EOL() = 0;
};
