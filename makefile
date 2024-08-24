[default]:
	clear
	mkdir -p build 
	cmake -S . -B build
	cmake --build ./build --parallel 10 --config Release
	./build/pong

run:
	clear
	cmake --build ./build --parallel 10 --config Release
	./build/pong

clear:
	rm -rf ./build/*

clear_cache:
	rm ./build/CMakeCache.txt
