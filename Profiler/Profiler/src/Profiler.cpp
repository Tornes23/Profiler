#include <intrin.h>
#include "Profiler.h"
#include <IMGUI/imgui.h>

Profiler::~Profiler()
{
	if (mbProfile)
		Leave(__rdtsc());

	Delete(mTree);
	mCurrent = nullptr;
	mTree = nullptr;
}

void Profiler::Enter(const char* id)
{
	if (mbProfile) Enter(__rdtsc(), id);
}

void Profiler::Leave()
{
	if (mbProfile) Leave(__rdtsc());
}

void Profiler::StartFrame()
{
	if (mFramesToCheck > 0 && mFramesToCheck <= mFrameCount)
		mbProfile = false;

	Reset(mTree);
}

void Profiler::EndFrame()
{
	mFrameCount++;

	if(mbCreateLog)
		PrintLog(mFilename);
}

void Profiler::Activate(bool toSet) { mbProfile = toSet; }

void Profiler::PrintLog(const char* filename)
{
	std::ofstream file;

	file.open(filename);

	if (file.is_open())
	{
		//frame header
		file << "Frame Number: " << mFrameCount << "\n";
		//recursive print in the log
		PrintNode(mTree, file);

		file.close();
	}
}

void Profiler::ShowGUI()
{
	if (!ImGui::Begin("Profiler"))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	
	std::string name(mFilename);

	ImGui::Checkbox("Profiler Active", &mbProfile);
	ImGui::Checkbox("Create Log", &mbCreateLog);
	//ImGui::InputText("Output File Name", const_cast<char*>(name.data()), 200); something wrong
	//mFilename = name.data();
	ImGui::SliderInt("Frames to profile", &mFramesToCheck, 0, 100000);

	NodeGUI(mTree);

	ImGui::End();
}

bool Profiler::IsActive() const { return mbProfile; }

void Profiler::Enter(unsigned long long timestamp, const char* id)
{
	if (mTree == nullptr) { mTree = NewNode(nullptr, id, timestamp); mCurrent = mTree; return; }

	if (mCurrent->mID == id) { mCurrent->mRecursion++; return; }

	Node* found = nullptr;
	if (Find(id, found)) { UpdateCurrent(found); return; }

	Node* node = NewNode(mCurrent, id, timestamp);

	mCurrent = node;
}

void Profiler::Leave(unsigned long long timestamp)
{
	if (mCurrent->mRecursion > 0) { mCurrent->mRecursion--; return; }

	unsigned long long time = mCurrent->mStartTime - timestamp;
	if (time < mCurrent->mMinTime) mCurrent->mMinTime = time;
	if (time > mCurrent->mMaxTime) mCurrent->mMaxTime = time;
	mCurrent->mTotal += time;
	mCurrent = mCurrent->mParent;
}

Node* Profiler::NewNode(Node* parent, const char* id, unsigned long long time)
{
	return new Node(parent, id, time);
}

void Profiler::NodeGUI(Node* node)
{
	if (node == nullptr) return;
	
	if (ImGui::TreeNode(node->mID))
	{
		ImGui::Text("Recursion Calls = %d",       node->mRecursion);
		ImGui::Text("Total Calls Per Frame = %d", node->mCallCount);
		ImGui::Text("Total Spent Time = %d",      node->mTotal);
		ImGui::Text("Minimum Spent Time = %d",    node->mMinTime);
		ImGui::Text("Maximum Spent Time= %d",     node->mMaxTime);

		if (node->mChilds.empty()) return;
		for (auto& it : node->mChilds)
			NodeGUI(it);

		ImGui::TreePop();
	}



}

bool Profiler::Find(const char* id, Node* found)
{
	if (id == nullptr)
		return false;

	for (auto& it : mCurrent->mChilds)
	{
		if (it->mID == id)
		{
			found = it;
			return true;
		}
	}

	return false;
}

void Profiler::Reset(Node* node)
{
	if (node == nullptr)
		return;

	node->Reset();

	if (node->mChilds.empty())
		return;

	for(auto& it: node->mChilds)
		Reset(it);
}

void Profiler::Delete(Node* node)
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

void Profiler::PrintNode(Node* node, std::ofstream& file)
{
	if (node == nullptr)
		return;

	file << node;

	if (node->mChilds.empty())
		return;

	for (auto& it : node->mChilds)
		PrintNode(it, file);

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
