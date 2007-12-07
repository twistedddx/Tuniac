#pragma once

class INotificationCallback
{
public:

	virtual void NCNotify(unsigned long event, void * params) = 0;
};

class CNotificationCenter
{
public:
	CNotificationCenter(void);
	~CNotificationCenter(void);
};
