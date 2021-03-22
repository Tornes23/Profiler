#pragma once
#include <limits>
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
	Node* mChild = nullptr;
	Node* mSibling = nullptr;
	const char* mID;
	unsigned mRecursion = 0;
	unsigned long long mTotal = 0;
	unsigned long long mStartTime = 0;
	unsigned mCallCount = 1;
	unsigned long long mMinTime = std::numeric_limits<unsigned long long>::max();
	unsigned long long mMaxTime = 0;
	bool mbLeft = false;

	//overload of << operators
	friend std::ostream& operator<<(std::ostream& os, const Node* node);

};

class Profiler
{
	MAKE_SINGLETON(Profiler)

public:

	~Profiler();
	void Enter(const char* id);
	void Leave();
	void StartFrame();
	void EndFrame();
	void Switch();
	void Print();

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
};

#define Prof Profiler::Instance()