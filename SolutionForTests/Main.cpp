#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
using namespace std;

//You are gay (╬▔皿▔)╯

const int nGroups = 4;

int PrepareData(std::vector<int>& gays) {

	//groups of 1 and 3 gays
	int take = min(gays[2], gays[0]);
	gays[2] -= take;
	gays[0] -= take;
	gays[3] += take;

	if (gays[1] % 2 == 1) {
		int take = gays[0] > 0 ? 1 : 0;
		gays[1] -= take;
		gays[0] -= take;
		gays[2] += take;
	}

	//groups what contains 2 gays
	if (gays[1] > gays[0]) {
		int take = gays[1] / 2;
		gays[1] -= take*2;
		gays[3] += take;
	}

	if (gays[0] > 1) {
		int pairs = gays[0] / 2;
		gays[0] -= pairs*2;
		gays[1] += pairs;
	}
	if (gays[1] > 1) {
		int pairs = gays[1] / 2;
		gays[1] -= pairs*2;
		gays[3] += pairs;
	}

	if (gays[0] == 1 && gays[1] > 0) {
		gays[0]--;
		gays[1]--;
		gays[2]++;
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
	std::cout << PrepareData(groups);
	return 0;
}