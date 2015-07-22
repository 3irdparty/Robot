all:
	gcc ./src/*.c -o ./bin/robot -std=c99 -lpthread -I./include -I./src -llua -lz
	cp ./script/* ./bin/ -r

clean:
	cd ./bin/ && rm * -rf && cd ../


