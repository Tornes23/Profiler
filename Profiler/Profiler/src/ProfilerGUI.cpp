/**
* @file		 ProfilerGUI.cpp      
* @author	 Nestor Uriarte,  nestor.uriarte@digipen.edu    
* @date		 Fri Mar 26 16:16:45 2021          
* @brief	 Contains the implementation of the imgui related functions for the profiler     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#define PROFILER_ON
#define IMGUI_ON

#include "Profiler.h"
#include //include the path to your ImGui Library

/**
* @brief	The wrapping function for ImGui
* @param	Node* node
**/
void ProfilerSystem::NodeGUI(Node* node)
{
	if (node == nullptr) return;

	if (ImGui::TreeNode(node->mID))
	{
		std::string text = "Recursion Calls = ";
		text += std::to_string(node->mRecursion);
		ImGui::Text(text.data());

		text = "Total Calls Per Frame = ";
		text += std::to_string(node->mCallCount);
		ImGui::Text(text.data());

		text = "Total Spent Time = ";
		text += std::to_string(node->mTotal);
		ImGui::Text(text.data());

		if (node->mChilds.empty()) { ImGui::TreePop(); return; }

		for (auto& it : node->mChilds)
			NodeGUI(it);

		ImGui::TreePop();
	}
}

/**
* @brief	Renders the ImGui interface 
**/
void ProfilerSystem::ShowGUI()
{
	if (!ImGui::Begin("Profiler"))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::Checkbox("Profiler Active", &mbProfile);
	ImGui::Checkbox("Create Log", &mbCreateLog);
	ImGui::Checkbox("Profile Limited Frames", &mbLimitFrames);
	ImGui::InputText("Output File Name", mFilename, 100);
	ImGui::DragInt("Frames to profile", &mFramesToCheck, 1, 1, 1000);

	NodeGUI(mTree);

	ImGui::End();
}