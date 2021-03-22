#include <intrin.h>
#include "Profiler.h"

Profiler::Profiler(const char* id)
{
	Enter(__rdtsc(), id);
}

Profiler::~Profiler()
{
	Leave(__rdtsc());
}

void Profiler::Enter(const char* id)
{
	Enter(__rdtsc(), id);
}

void Profiler::Leave()
{
	Leave(__rdtsc());
}

void Profiler::Enter(unsigned long long timestamp, const char* id)
{
	if (mTree == nullptr) { mTree = NewNode(nullptr, id, timestamp); mCurrent = mTree; return; }

	if (mCurrent->mID == id) { mCurrent->mRecursion++; return; }

	Node* found = nullptr;
	Find(mCurrent->mChild, id, &found);
	if (found != nullptr) { UpdateCurrent(found); return; }
	Find(mCurrent->mSibling, id, &found);
	if (found != nullptr) { UpdateCurrent(found); return; }

	Node* node = NewNode(mCurrent, id, timestamp);

	if (mPrev != nullptr && mPrev->mbLeft)
		mCurrent->mSibling = node;
	else
		mCurrent->mChild = node;

	mPrev = mCurrent;
	mCurrent = node;

}

void Profiler::Leave(unsigned long long timestamp)
{
	if (mCurrent->mRecursion > 0) { mCurrent->mRecursion--; return; }

	unsigned long long time = mCurrent->mStartTime - timestamp;
	if (time < mCurrent->mMinTime) mCurrent->mMinTime = time;
	if (time > mCurrent->mMaxTime) mCurrent->mMaxTime = time;
	mCurrent->mTotal = time;
	mCurrent->mbLeft = true;
	mPrev = mCurrent;
	mCurrent = mCurrent->mParent;
}

Profiler::Node* Profiler::NewNode(Node* parent, const char* id, unsigned long long time)
{
	return new Node(parent, id, time);
}

void Profiler::Find(Node* node, const char* id, Node** found)
{
	//recursive function to find in a tree
	if (node == nullptr)
		return;

	if (node->mID == id) { *found = node; return; }

	Find(node->mChild, id, found);
	Find(node->mSibling, id, found);

}

void Profiler::UpdateCurrent(Node* newNode)
{
	mCurrent = newNode;
	mCurrent->mCallCount++;
	mCurrent->mStartTime = __rdtsc();
}
