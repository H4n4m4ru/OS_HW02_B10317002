#ifndef _PROCESS_H_
#define _PROCESS_H_
#include<string>

class Process{
public:
	Process(std::string = "", int = 0, int = 0, int = 0);
	~Process();
	
	std::string ProcessName();
	int ProcessTime();
	int ProcessDeadline();
	int ProcessPriority();
	int ProcessPeriod();
	bool Terminated();
	void execute();
	void deadlineExtend(int);
private:
	std::string _processName;
	int _processTime = 0, _processDeadline = 0, _processPriority = 0, _processPeriod = 0;
	bool _terminated = false;
};











#endif