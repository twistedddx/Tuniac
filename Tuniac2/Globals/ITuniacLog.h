#pragma once

typedef enum LogLevel
{
	Unimportant,
	Note,
	Warning,
	Critical
} LogLevel;

class ITuniacLog
{
public:
	virtual bool	Log(String message, LogLevel Level = Note) = 0;
	virtual bool	LogWithEOL(String message, LogLevel Level = Note) = 0;
	virtual bool	EOL() = 0;
};
