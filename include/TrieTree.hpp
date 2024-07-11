#ifndef __TRIETREE_HPP__
# define __TRIETREE_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

struct LocationNode
{
	std::string					name;
	int							lvl;
	std::vector<LocationNode*>	children;
};

bool	insert(std::vector<LocationNode *>& root, LocationNode* node, int (*cmp)(std::string, std::string));
void	print(std::vector<LocationNode *>& root);
void	setlvl(std::vector<LocationNode *>& root);
int		cmp (std::string a, std::string b);



#endif