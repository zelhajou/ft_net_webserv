#include "TrieTree.hpp"

int	cmp (std::string a, std::string b)
{
	if (b.find(a) != std::string::npos)
		return 0;
	return 1;
}

void setlvl(std::vector<LocationNode *>& root) {
	std::vector<LocationNode*> queue;
	for (int i = 0; i < root.size(); i++) {
		root[i]->lvl = 0;
		std::cout << root[i]->name << " lvl: " << root[i]->lvl << std::endl; // "lvl: 0
		queue.push_back(root[i]);
	}
	while (!queue.empty()) {
		LocationNode* temp = queue.front();
		queue.erase(queue.begin());
		for (int i = 0; i < temp->children.size(); i++) {
			temp->children[i]->lvl = temp->lvl + 1;
			std::cout << temp->children[i]->name << " lvl: " << temp->children[i]->lvl << std::endl; // "lvl: 1
			queue.push_back(temp->children[i]);
		}
	}
}

void print(std::vector<LocationNode *>& root) {
	int lvl = 0;
	std::vector<LocationNode*> queue;
	for (int i = 0; i < root.size(); i++) {
		queue.push_back(root[i]);
	}
	while (!queue.empty()) {
		LocationNode* temp = queue.front();
		queue.erase(queue.begin());
		if (temp->lvl != lvl) {
			std::cout << std::endl;
			lvl = temp->lvl;
		}
		std::cout << std::setw(10) << temp->name << "	";
		queue.insert(queue.end(), temp->children.begin(), temp->children.end());
	}
}

bool	insert(std::vector<LocationNode *>& root, LocationNode* node, int (*cmp)(std::string, std::string)) {
	if (root.size() == 0 || cmp(node->name, root[0]->name) == 0) {
		node->children.insert(node->children.end(), root.begin(), root.end());
		root.clear();
		root.push_back(node);
		return true;
	}
	for (int i = 0; i < root.size(); i++) {
		if (cmp(root[i]->name, node->name) == 0) {
			if (insert(root[i]->children, node, cmp)) {
				return true;
			}
		}
	}
	root.push_back(node);
	return true;
}
