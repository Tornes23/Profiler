#pragma once
#include <limits>

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

class Profiler
{
	MAKE_SINGLETON(Profiler)

		struct Node
	{
		Node(Node* parent, const char* id, unsigned long long time) : mParent(parent), mID(id), mStartTime(time) {}
		Node* mParent = nullptr;
		Node* mChild = nullptr;
		Node* mSibling = nullptr;
		const char* mID;
		unsigned mRecursion = 0;
		unsigned long long mTotal = 0;
		unsigned long long mStartTime = 0;
		unsigned mCallCount = 0;
		unsigned long long mMinTime = std::numeric_limits<unsigned long long>::max();
		unsigned long long mMaxTime = 0;
		bool mbLeft = false;

	};

public:

	Profiler(const char* id);
	~Profiler();
	void Enter(const char* id);
	void Leave();

private:

	void Enter(unsigned long long timestamp, const char* id);
	void Leave(unsigned long long timestamp);
	void Find(Node* node, const char* id, Node** found);
	void UpdateCurrent(Node* newNode);
	Node* NewNode(Node* parent, const char* id, unsigned long long time);
	Node* mCurrent = nullptr;
	Node* mPrev = nullptr;
	Node* mTree = nullptr;
	bool mbLeft = false;
};

#define Prof Profiler::Instance()