/**
* @file		 Profiler.cpp      
* @author	 Nestor Uriarte,  nestor.uriarte@digipen.edu    
* @date		 Fri Mar 26 16:15:06 2021          
* @brief	 Contains the implementation of the ProfilerSystem class     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#define PROFILER_ON
#include "Profiler.h"
#include <intrin.h>

/**
* @brief	The Destructor of the class
**/
ProfilerSystem::~ProfilerSystem()
{
	//if we want to profile call to leave
	if (mbProfile)
		Leave(__rdtsc());

	//clweaning the tree
	Delete(mTree);
	mCurrent = nullptr;
	mTree = nullptr;
}

/**
* @brief	The enter function to gather the necessary data
* @param	const char* id
**/
void ProfilerSystem::Enter(const char* id)
{
	//if we want to profile call to enter
	if (mbProfile) Enter(__rdtsc(), id);
}

/**
* @brief	The leave function to compute the spent time in the function
**/
void ProfilerSystem::Leave()
{
	//if we want to profile call to leave
	if (mbProfile) Leave(__rdtsc());
}

/**
* @brief	Starts the frame for the profiler
**/
void ProfilerSystem::StartFrame()
{
	//deactivating the profiler is all the checked frames are done
	if (mbLimitFrames && mFramesToCheck <= mFrameCount)
		mbProfile = false;

	//if we want to keep profiling reset the nodes
	if(mbProfile)
		Reset(mTree);
}

/**
* @brief	Ends the frame for the profiler
**/
void ProfilerSystem::EndFrame()
{
	//increasing the counter
	mFrameCount++;

	//if a log wants to be created print it
	if(mbCreateLog)
		PrintLog(mFilename);
}

/**
* @brief	Sets the Profiler active or not
* @param	bool toSet
**/
void ProfilerSystem::Activate(bool toSet) { mbProfile = toSet; }

/**
* @brief	Prints the log to a TXT
* @param	const char* filename
**/
void ProfilerSystem::PrintLog(const char* filename)
{
	std::ofstream file;
	//opening the file based on if we want a specific set of frames to print or not
	if(!mbLimitFrames)
		file.open(filename, std::ofstream::app);
	else
	{
		file.open(filename);
		mbLimitFrames = false;
	}

	//if the file was opened succesfully
	if (file.is_open())
	{
		//frame header
		file << "******************************************\n";
		file << "Frame Number: " << mFrameCount << "\n";
		
		//recursive print in the log
		PrintNode(mTree, file);

		//printing the end of the frame
		file << "Frame Ended\n";
		file << "******************************************\n";

		//closing the file
		file.close();
	}
}

/**
* @brief	Getter function to know if the profiler is active
* @return	bool
**/
bool ProfilerSystem::IsActive() const { return mbProfile; }

/**
* @brief	Private method that gets adds the node to the tree if necesary
* @param	unsigned long long timestamp
* @param	const char* id
**/
void ProfilerSystem::Enter(unsigned long long timestamp, const char* id)
{
	//if the root does notexist create it
	if (mTree == nullptr) { mTree = NewNode(nullptr, id, timestamp); mCurrent = mTree; return; }
	//if it was a recursive call increment the counter and return
	if (mCurrent->mID == id) { mCurrent->mRecursion++; return; }
	//if the function has already been traversed find it and update the values
	Node* found = nullptr;
	if (Find(id, &found)) { UpdateCurrent(found, timestamp); return; }
	//if not found create a new node and add it to the tree
	Node* node = NewNode(mCurrent, id, timestamp);

	mCurrent->mChilds.push_back(node);
	mCurrent = node;
}

/**
* @brief	Private methos that performs the calculus for the node
* @param	unsigned long long timestamp
**/
void ProfilerSystem::Leave(unsigned long long timestamp)
{
	//if the recurcion call is positive
	if (mCurrent->mRecursion > 0) { mCurrent->mRecursion--; return; }

	//computing the cicles spent in the function
	unsigned long long time = mCurrent->mStartTime - timestamp;
	mCurrent->mTotal += time;//adding it to the total counter
	mCurrent = mCurrent->mParent;//setting the current to the parent node
}

/**
* @brief	Creates a new node
* @param	Node* parent
* @param	const char* id
* @param	unsigned long long time
* @return	Node*
**/
Node* ProfilerSystem::NewNode(Node* parent, const char* id, unsigned long long time)
{
	//creating a new node
	return new Node(parent, id, time);
}

/**
* @brief	Finds a node based on the given id and stores it in the given node**
* @param	const char* id
* @param	Node** found
* @return	bool
**/
bool ProfilerSystem::Find(const char* id, Node** found)
{
	//sanity check
	if (id == nullptr)
		return false;
	//looping through the vector
	for (auto& it : mCurrent->mChilds)
	{
		//if it has the wanted id
		if (it->mID == id)
		{
			//store it and return it was found
			*found = it;
			return true;
		}
	}
	//return it was not found
	return false;
}

/**
* @brief	Resets the values in the node
* @param	Node* node
**/
void ProfilerSystem::Reset(Node* node)
{
	//sanity check
	if (node == nullptr)
		return;
	//reseting the node
	node->Reset();

	//if it has no childs return
	if (node->mChilds.empty())
		return;
	//calling to reset each child node
	for(auto& it: node->mChilds)
		Reset(it);
}

/**
* @brief	Deletes the node
* @param	Node* node
**/
void ProfilerSystem::Delete(Node* node)
{
	//sanity check
	if (node == nullptr)
		return;

	//if it has no childs return
	if (node->mChilds.empty())
		return;
	//call to delete each child node
	for (auto& it : node->mChilds)
		Delete(it);

	//deallocating the memory
	delete node;
	node = nullptr;
}

/**
* @brief	Prints a node to a TXT
* @param	Node* node
* @param	std::ofstream& file
**/
void ProfilerSystem::PrintNode(Node* node, std::ofstream& file)
{
	//sanity check
	if (node == nullptr)
		return;
	//printing the node
	file << node;

	//if it has no childs return
	if (node->mChilds.empty())
		return;
	//print each child to the txt
	for (auto& it : node->mChilds)
		PrintNode(it, file);

}

/**
* @brief	Updates the current node
* @param	Node* newNode
* @param	unsigned long long timestamp
**/
void ProfilerSystem::UpdateCurrent(Node* newNode, unsigned long long timestamp)
{
	//setting the current node
	mCurrent = newNode;
	mCurrent->mCallCount++;//incrementing the call count
	mCurrent->mStartTime = timestamp;//setting the start time
}

/**
* @brief	Resets the Tree of the profiler
**/
void Node::Reset()
{
	//setting the values to 0
	mRecursion = 0;
	mTotal = 0;
	mStartTime = 0;
	mCallCount = 0;
}

/**
* @brief	Overload of the << operator for the nodes
* @param	std::ostream& os
* @param	const Node* node
* @return	std::ostream&
**/
std::ostream& operator<<(std::ostream& os, const Node* node)
{
	//printing the data to the file
	os << "------------------------------------------------\n";
	os << "Function Name: " << node->mID<< "\n";
	os << "Recursion Call Count: " << node->mRecursion<< "\n";
	os << "Total Time In Function: " << node->mTotal<< "\n";
	os << "Total Function Call Count: " << node->mCallCount<< "\n";

	return os;
}
