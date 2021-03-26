#include "Profiler.h"
#ifdef PROFILER_ON
#include <intrin.h>

ProfilerSystem::~ProfilerSystem()
{
	if (mbProfile)
		Leave(__rdtsc());

	Delete(mTree);
	mCurrent = nullptr;
	mTree = nullptr;
}

void ProfilerSystem::Enter(const char* id)
{
	if (mbProfile) Enter(__rdtsc(), id);
}

void ProfilerSystem::Leave()
{
	if (mbProfile) Leave(__rdtsc());
}

void ProfilerSystem::StartFrame()
{
	if (mbLimitFrames && mFramesToCheck <= mFrameCount)
		mbProfile = false;

	if(mbProfile)
		Reset(mTree);
}

void ProfilerSystem::EndFrame()
{
	mFrameCount++;

	if(mbCreateLog)
		PrintLog(mFilename);
}

void ProfilerSystem::Activate(bool toSet) { mbProfile = toSet; }

void ProfilerSystem::PrintLog(const char* filename)
{
	std::ofstream file;

	if(!mbLimitFrames)
		file.open(filename, std::ofstream::app);
	else
		file.open(filename);


	if (file.is_open())
	{
		//frame header
		file << "******************************************\n";
		file << "Frame Number: " << mFrameCount << "\n";
		//recursive print in the log
		PrintNode(mTree, file);

		file << "Frame Ended\n";
		file << "******************************************\n";

		file.close();
	}
}

bool ProfilerSystem::IsActive() const { return mbProfile; }

void ProfilerSystem::Enter(unsigned long long timestamp, const char* id)
{
	if (mTree == nullptr) { mTree = NewNode(nullptr, id, timestamp); mCurrent = mTree; return; }

	if (mCurrent->mID == id) { mCurrent->mRecursion++; return; }

	Node* found = nullptr;
	if (Find(id, &found)) { UpdateCurrent(found, timestamp); return; }

	Node* node = NewNode(mCurrent, id, timestamp);

	mCurrent->mChilds.push_back(node);
	mCurrent = node;
}

void ProfilerSystem::Leave(unsigned long long timestamp)
{
	if (mCurrent->mRecursion > 0) { mCurrent->mRecursion--; return; }

	unsigned long long time = mCurrent->mStartTime - timestamp;
	mCurrent->mTotal += time;
	mCurrent = mCurrent->mParent;
}

Node* ProfilerSystem::NewNode(Node* parent, const char* id, unsigned long long time)
{
	return new Node(parent, id, time);
}

bool ProfilerSystem::Find(const char* id, Node** found)
{
	if (id == nullptr)
		return false;

	for (auto& it : mCurrent->mChilds)
	{
		if (it->mID == id)
		{
			*found = it;
			return true;
		}
	}

	return false;
}

void ProfilerSystem::Reset(Node* node)
{
	if (node == nullptr)
		return;

	node->Reset();

	if (node->mChilds.empty())
		return;

	for(auto& it: node->mChilds)
		Reset(it);
}

void ProfilerSystem::Delete(Node* node)
{
	if (node == nullptr)
		return;

	if (node->mChilds.empty())
		return;

	for (auto& it : node->mChilds)
		Delete(it);

	delete node;
	node = nullptr;
}

void ProfilerSystem::PrintNode(Node* node, std::ofstream& file)
{
	if (node == nullptr)
		return;

	file << node;

	if (node->mChilds.empty())
		return;

	for (auto& it : node->mChilds)
		PrintNode(it, file);

}

void ProfilerSystem::UpdateCurrent(Node* newNode, unsigned long long timestamp)
{
	mCurrent = newNode;
	mCurrent->mCallCount++;
	mCurrent->mStartTime = timestamp;
}

void Node::Reset()
{
	mRecursion = 0;
	mTotal = 0;
	mStartTime = 0;
	mCallCount = 0;
}

std::ostream& operator<<(std::ostream& os, const Node* node)
{
	os << "------------------------------------------------\n";
	os << "Function Name: " << node->mID<< "\n";
	os << "Recursion Call Count: " << node->mRecursion<< "\n";
	os << "Total Time In Function: " << node->mTotal<< "\n";
	os << "Total Function Call Count: " << node->mCallCount<< "\n";

	return os;
}
#endif
