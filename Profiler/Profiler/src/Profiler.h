/**
* @file		 Profiler.h      
* @author	 Nestor Uriarte,  nestoe.uriarte@digipen.edu    
* @date		 Fri Mar 26 16:15:36 2021          
* @brief	 Contains the definition of the ProfilerSystem class     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once

#if defined(PROFILER_ON)
#include <limits>
#include <vector>
#include <string>
#include <fstream>

#define MAKE_SINGLETON(classname)\
	public:\
	static classname& Instance()\
	{\
		static classname singleton;\
		return singleton;\
	}\
	private:\
	classname() = default;\
	classname(const classname &) = delete;\
	const classname & operator=(const classname &) = delete;

struct Node
{
	Node(Node* parent, const char* id, unsigned long long time) : mParent(parent), mID(id), mStartTime(time) {}
	void Reset();
	Node* mParent = nullptr;
	std::vector<Node*> mChilds;
	const char* mID;
	unsigned mRecursion = 0;
	unsigned long long mTotal = 0;
	unsigned long long mStartTime = 0;
	unsigned mCallCount = 1;

	//overload of << operators for file output
	friend std::ostream& operator<<(std::ostream& os, const Node* node);

};

class ProfilerSystem
{
	MAKE_SINGLETON(ProfilerSystem)

public:

	~ProfilerSystem();
	void Enter(const char* id = "DefaultName");
	void Leave();
	void StartFrame();
	void EndFrame();
	void Activate(bool toSet);
	void PrintLog(const char* filename = "ProfilerLog.txt");
	bool IsActive() const;

#ifdef IMGUI_ON
	void ShowGUI();
	void NodeGUI(Node* node);
#endif // IMGUI_ON

private:

	void Enter(unsigned long long timestamp, const char* id);
	void Leave(unsigned long long timestamp);
	bool Find(const char* id, Node** found);
	void Reset(Node* node);
	void Delete(Node* node);
	void PrintNode(Node* node, std::ofstream& file);
	void UpdateCurrent(Node* newNode, unsigned long long timestamp);
	Node* NewNode(Node* parent, const char* id, unsigned long long time);
	Node* mCurrent = nullptr;
	Node* mTree = nullptr;
	int mFrameCount = 0;
	int mFramesToCheck = 1;
	bool mbProfile = true;
	bool mbCreateLog = false;
	bool mbLimitFrames = false;
	char mFilename[100] = "Profiler.txt";
};


//macros to use the profiler
#define PROFILE(X)		  ProfilerSystem::Instance().Enter(X);
#define PROFILER_LEAVE	  ProfilerSystem::Instance().Leave();
#define PRINT_LOG(X)      ProfilerSystem::Instance().PrintLog(X); 
#define START_FRAME       ProfilerSystem::Instance().StartFrame(); 
#define END_FRAME		  ProfilerSystem::Instance().EndFrame(); 

#else// PROFILER_OFF

#define PROFILE(X)			 
#define PROFILER_LEAVE  
#define PRINT_LOG(X)  
#define START_FRAME    
#define END_FRAME     

#endif

#if defined(PROFILER_ON) && defined(IMGUI_ON)
#define RENDERGUI ProfilerSystem::Instance().ShowGUI();
#else// IMGUI_OFF
#define GUIPATH   
#define RENDERGUI 
#endif
