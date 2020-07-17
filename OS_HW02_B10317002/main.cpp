#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"Process.h"
using namespace std;

bool FirstPriorityHigher(Process _p1, Process _p2){
	if (_p1.ProcessPriority() > _p2.ProcessPriority())return true;
	return false;
};

bool FirstPeriodLower(Process _p1, Process _p2){
	if (_p1.ProcessPeriod() < _p2.ProcessPeriod())return true;
	return false;
};

bool FirstDaedlineLower(Process _p1, Process _p2){
	if (_p1.ProcessDeadline() < _p2.ProcessDeadline())return true;
	return false;
};

void main(){

	bool _idling = false;
	string _filePath, _pName;
	string info[3] = {"(Priority-based Scheduling)","(Rate Monotonic Scheduling)","(Earliest Deadline First Scheduling (EDF))"};
	int _pTime, _pDeadline, _pPriority, _clock = 0, _time_termi = 0, _time_start = 0;
	ifstream _InputData;
	vector<Process> _JobPool, _JobPool_dup, _ProcessPool, _CPU, _MissDLPool;
	char *_Buffer = new char[50];
	Process *_POGGERSProcess = nullptr;

	bool(*priorityCompare)(Process, Process) = FirstPriorityHigher;
	bool(*periodCompare)(Process, Process) = FirstPeriodLower;
	bool(*deadlineCompare)(Process, Process) = FirstDaedlineLower;
	bool(*CompareFuncz[3])(Process, Process) = { priorityCompare, periodCompare, deadlineCompare };

	//Reserve capsity for vectors,otherwise it would occurs pointer errors
	_JobPool.reserve(50);
	_JobPool_dup.reserve(50);
	_ProcessPool.reserve(50);
	_MissDLPool.reserve(50);

	//Load data from input.txt
	do{
		cout << "Enter input file\'s path to load data : ";
		cin >> _filePath;
		_InputData.open(_filePath);
	} while (!_InputData);

	//Initialize job pool
	while (_InputData >> _pName){
		_InputData.getline(_Buffer, 50, ':'); _InputData >> _pTime;
		_InputData.getline(_Buffer, 50, ':'); _InputData >> _pDeadline;
		_InputData.getline(_Buffer, 50, ':'); _InputData >> _pPriority;
		_JobPool.push_back(Process(_pName, _pTime, _pDeadline, _pPriority));
	}

	delete[]_Buffer;

	for (int i = 0; i < (int)_JobPool.size(); i++)
		if (_JobPool[i].ProcessDeadline() > _time_termi)_time_termi = _JobPool[i].ProcessDeadline();

	//------------------------------------^^ Initialization end ^^------------------------------------

	for (int z = 0; z < 3; z++){

		//Start
		_idling = false;
		_time_start = 0;
		_clock = 0;
		_JobPool_dup.clear();
		_ProcessPool.clear();
		_CPU.clear();
		_MissDLPool.clear();

		cout << "Start-" << info[z] << "\n";
		_JobPool_dup.assign(_JobPool.begin(), _JobPool.end());	//copy processes from original job pool

		while (_clock < _time_termi){

			_POGGERSProcess = nullptr;
			//Check job pool if any process arrived(completed a period)
			for (int i = 0; i < (int)_JobPool_dup.size(); i++){
				if (_clock%_JobPool_dup[i].ProcessDeadline() != 0)continue;
				_ProcessPool.push_back(_JobPool_dup[i]);	//Add to process pool
				_ProcessPool.back().deadlineExtend(_clock);

				if (_POGGERSProcess == nullptr){
					_POGGERSProcess = &_ProcessPool.back();
				}
				else if (CompareFuncz[z](_ProcessPool.back(), *_POGGERSProcess))
					_POGGERSProcess = &_ProcessPool.back();
			}

			//Idle check
			if (_idling){
				if ((int)_ProcessPool.size() == 0){
					_clock++;
					continue;
				}
				else {
					//Finally got some process to execute,end the idling status
					_idling = false;
					cout << "idle " << _time_start << "-" << _clock << '\n';
					_time_start = _clock;
				}
			}
			else if (((int)_CPU.size() == 0) && (int)_ProcessPool.size() == 0){
				//Start idling status
				_idling = true;
				_time_start = _clock;
				_clock++;
				continue;
			}

			if ((int)_CPU.size() == 0){

				//Find the process with highest priority
				for (int i = 0; i < (int)_ProcessPool.size(); i++){
					if (_POGGERSProcess == nullptr)_POGGERSProcess = &_ProcessPool[0];
					else if (CompareFuncz[z](_ProcessPool[i], *_POGGERSProcess))_POGGERSProcess = &_ProcessPool[i];
				}

				//Load process into CPU and remove it from process pool
				_CPU.push_back(*_POGGERSProcess);
				_ProcessPool.erase(_ProcessPool.begin() + (_POGGERSProcess - &_ProcessPool[0]));
				_time_start = _clock;
			}
			else if (_POGGERSProcess != nullptr&& (CompareFuncz[z](*_POGGERSProcess,_CPU[0]))){
				//Content switch
				cout << _CPU[0].ProcessName() << " " << _time_start << "-" << _clock << '\n';
				_time_start = _clock;

				_ProcessPool.push_back(_CPU[0]);	//Return the process with lower priority back to process pool
				_CPU.pop_back();
				_CPU.push_back(*_POGGERSProcess);	//Load the process with higher priority into CPU
				_ProcessPool.erase(_ProcessPool.begin() + (_POGGERSProcess - &_ProcessPool[0]));
			}

			//Execute
			_CPU[0].execute();
			_clock++;

			//
			if (_CPU[0].Terminated()){
				cout << _CPU[0].ProcessName() << " " << _time_start << "-" << _clock << "\n";
				_time_start = _clock;
				_CPU.pop_back();
			}
			else if (_clock >= _CPU[0].ProcessDeadline()){
				//Miss deadline
				cout << _CPU[0].ProcessName() << " " << _time_start << "-" << _clock << "\n";
				_time_start = _clock;
				cout << _CPU[0].ProcessName() << " miss deadline\n";

				//Remove the same process in job pool
				for (int i = 0; i < (int)_JobPool_dup.size(); i++){
					if (_JobPool_dup[i].ProcessName() != _CPU[0].ProcessName()) continue;
					_JobPool_dup.erase(_JobPool_dup.begin() + i);
					break;
				}

				_MissDLPool.push_back(_CPU[0]);
				_CPU.pop_back();
			}

			//Check processes in process pool whether they missed deadline
			for (int i = 0; i < (int)_ProcessPool.size(); i++){
				if (_clock < _ProcessPool[i].ProcessDeadline())continue;

				if ((int)_CPU.size() > 0){
					cout << _CPU[0].ProcessName() << " " << _time_start << "-" << _clock << "\n";
					_time_start = _clock;
				}

				cout << _ProcessPool[i].ProcessName() << " miss deadline\n";

				//Remove the same process in job pool
				for (int j = 0; j < (int)_JobPool_dup.size(); j++){
					if (_JobPool_dup[j].ProcessName() != _ProcessPool[i].ProcessName()) continue;
					_JobPool_dup.erase(_JobPool_dup.begin() + j);
					break;
				}

				_MissDLPool.push_back(_ProcessPool[i]);
				_ProcessPool.erase(_ProcessPool.begin() + i);

				i--;
			}
		}

		cout << "End\n" << "Miss deadline:";

		if ((int)_MissDLPool.size() == 0)cout << "None\n";
		else{
			for (int i = 0; i < (int)_MissDLPool.size(); i++)
				cout << (i>0 ? "," : "") << _MissDLPool[i].ProcessName();
		}
		//End
		cout << "\n\n";
	}

	system("PAUSE");
}
