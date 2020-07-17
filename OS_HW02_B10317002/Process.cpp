#include"Process.h"


Process::Process(std::string _pName, int _pTime, int _pDeadline, int _pPriority){
	_processName = _pName;
	_processTime = _pTime;
	_processDeadline = _pDeadline;
	_processPriority = _pPriority;
	_processPeriod = _pDeadline;
	if (_processTime <= 0) _terminated = true;
}
Process::~Process(){ ; }

std::string Process::ProcessName(){ return _processName; }
int Process::ProcessTime(){ return _processTime; }
int Process::ProcessDeadline(){ return _processDeadline; }
int Process::ProcessPriority(){ return _processPriority; }
int Process::ProcessPeriod(){ return _processPeriod; }
bool Process::Terminated(){ return _terminated; }
void Process::execute(){ 
	if ((--_processTime) == 0)_terminated = true; 
}
void Process::deadlineExtend(int _tExtend){
	_processDeadline += _tExtend;
}