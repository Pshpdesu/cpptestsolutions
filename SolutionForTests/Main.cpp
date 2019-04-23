#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
using namespace std;

//You are gay (╬▔皿▔)╯

const int nGroups = 4;

void PrepareData(std::vector<int>& gays) {
	if (gays[1] % 2 == 1) {
		int take = gays[0] > 0 ? 1 : 0;
		gays[1] -= take;
		gays[0] -= take;
		gays[2] += take;
	}
	if (gays[1] > gays[0]) {
		int take = gays[1] / 2;
		gays[1] -= take*2;
		gays[3] += take;
	}
	int take = min(gays[2], gays[0]);
	gays[2] -= take;
	gays[0] -= take;
	gays[3] += take;
}

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
	PrepareData(groups);
	std::cout << FindMax(std::move(groups));
	return 0;
}