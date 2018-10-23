all:
	mkdir -p build/
	gcc src/*.c -o build/battleships

clean:
	rm -rf build/