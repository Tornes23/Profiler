#pragma once
//TO REMOVE
#define PROFILER_ON
#define IMGUI_ON

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
	unsigned long long mMinTime = std::numeric_limits<unsigned long long>::max();
	unsigned long long mMaxTime = 0;
	bool mbLeft = false;

	//overload of << operators for file output
	friend std::ostream& operator<<(std::ostream& os, const Node* node);

};

struct ProfileThis
{
	ProfileThis(const char* id = "DefaultName");
	~ProfileThis();
};

class Profiler
{
	MAKE_SINGLETON(Profiler)

public:

	~Profiler();
	void Enter(const char* id = "DefaultName");
	void Leave();
	void StartFrame();
	void EndFrame();
	void Activate(bool toSet);
	void ActivateGUI(bool toSet);
	void PrintLog(const char* filename = "ProfilerLog.txt");
	void ShowGUI();
	bool IsActive() const;

private:

	void Enter(unsigned long long timestamp, const char* id);
	void Leave(unsigned long long timestamp);
	void Find(Node* node, const char* id, Node** found);
	void Reset(Node* node);
	void Delete(Node* node);
	void PrintNode(Node* node, std::ofstream& file);
	void UpdateCurrent(Node* newNode);
	Node* NewNode(Node* parent, const char* id, unsigned long long time);
	Node* mCurrent = nullptr;
	Node* mPrev = nullptr;
	Node* mTree = nullptr;
	bool mbLeft = false;
	unsigned long long mFrameCount = 1;
	unsigned long long mFramesToCheck = 1;
	bool mbProfile = true;
	bool mbRenderGUI = true;
};


//macros to use the profiler
#define PROFILE(X)		  ProfileThis(X);
#define PROFILER_ENTER(X) Profiler::Instance().Enter(X);
#define PROFILER_LEAVE	  Profiler::Instance().Leave();
#define PRINT_LOG(X)      Profiler::Instance().PrintLog(X); 

#else// PROFILER_OFF

#define PROFILE(X)			  
#define PROFILER_ENTER(X) 
#define PROFILER_LEAVE  
#define PRINT_LOG(X)  

#endif

#if defined(PROFILER_ON) && defined(IMGUI_ON)
//macro to use ImGUI
#define RENDERGUI Profiler::Instance().ShowGUI();
#else// IMGUI_OFF
#define RENDERGUI ;
#endif
