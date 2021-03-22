#include <intrin.h>
#include "Profiler.h"

Profiler::~Profiler()
{
	if (mbProfile)
		Leave(__rdtsc());

	Delete(mTree);
	mPrev = nullptr;
	mCurrent = nullptr;
	mTree = nullptr;
}

void Profiler::Enter(const char* id)
{
	if(mbProfile)
		Enter(__rdtsc(), id);
}

void Profiler::Leave()
{
	if (mbProfile)
		Leave(__rdtsc());
}

void Profiler::StartFrame()
{
	if (mFramesToCheck < mFrameCount)
		mbProfile = false;

	Reset(mTree);
}

void Profiler::EndFrame()
{
	mFrameCount++;

	//render imgui
	//dump stats to log
	Print();
}

void Profiler::Switch() { mbProfile = !mbProfile; }

void Profiler::Print()
{
	std::ofstream file;

	file.open("ProfilerStats.txt");

	if (file.is_open())
	{
		//frame header
		file << "Frame Number: " << mFrameCount << "\n";
		//recursive print in the log
		PrintNode(mTree, file);

		file.close();
	}
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

Node* Profiler::NewNode(Node* parent, const char* id, unsigned long long time)
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

void Profiler::Reset(Node* node)
{
	if (node == nullptr)
		return;

	node->Reset();
	Reset(node->mChild);
	Reset(node->mSibling);
}

void Profiler::Delete(Node* node)
{
	if (node == nullptr)
		return;

	Delete(node->mChild);
	Delete(node->mSibling);

	delete node;
	node = nullptr;
}

void Profiler::PrintNode(Node* node, std::ofstream& file)
{
	if (node == nullptr)
		return;

	file << node;

	PrintNode(node->mChild, file);
	PrintNode(node->mSibling, file);

}

void Profiler::UpdateCurrent(Node* newNode)
{
	mCurrent = newNode;
	mCurrent->mCallCount++;
	mCurrent->mStartTime = __rdtsc();
}

void Node::Reset()
{
	mRecursion = 0;
	mTotal = 0;
	mStartTime = 0;
	mCallCount = 0;
	mMinTime = std::numeric_limits<unsigned long long>::max();
	mMaxTime = 0;
	mbLeft = false;
}

std::ostream& operator<<(std::ostream& os, const Node* node)
{
	os << "------------------------------------------------\n";
	os << "Function Name: " << node->mID<< "\n";
	os << "Recursion Call Count: " << node->mRecursion<< "\n";
	os << "Total Time In Function: " << node->mTotal<< "\n";
	os << "Total Function Call Count: " << node->mCallCount<< "\n";
	os << "Minimum Time In Function: " << node->mMinTime<< "\n";
	os << "Maximum Time In Function: " << node->mMaxTime<< "\n";

	return os;
}
