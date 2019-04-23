#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
using namespace std;

//You are gay (╬▔皿▔)╯

const int nGroups = 4;

int FindMax(std::vector<int>&& gays) {
	int gayscount = gays.size();
	size_t maxgays = gays.size();
	
	for (int i = 0; i < gayscount - 1; ++i) {
		int j = gayscount-2;
		while (gays[i] > 0 && j>=i) {
			if (i + j + 1 < gayscount) {
				int neededcount = min(gays[i], gays[j]);
				if (i == j) {
					neededcount /= 2;
					gays[i] -= neededcount*2;
					gays[i + j + 1] += neededcount;
				}
				else {
					gays[i] -= neededcount;
					gays[j] -= neededcount;
					gays[i + j + 1] += neededcount;	
				}
				if(neededcount > 0) j += i+2;
			}
			j--;
		}
	}
	return std::accumulate(gays.begin(), gays.end(), 0);
}

int main() {
	int count;
	std::cin >> count;
	std::vector<int> groups(nGroups);
	for (int i = 0; i < count; i++) {
		int val;
		std::cin >> val;
		groups[val - 1]++;
	}
	std::cout << FindMax(std::move(groups));
	return 0;
}