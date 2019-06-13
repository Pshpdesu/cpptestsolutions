#include "Headers.h"
#include "../Extensions/ThreadPool.h"
int main() {
	ThreadPool pool(3);
	for (int i = 0; i < 3; ++i) {
		pool.pushTask([](int k){
			for (int i = 0; i < k; i++) {
				std::cout << "i = " << i << std::endl;
			}
			},10);
	}
	return 0;
}