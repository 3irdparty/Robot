all:
	g++ ./src/*.c -o ./bin/robot -lpthread -I./include -I./src -llua -lz -g
	cp ./script/* ./bin/ -r

clean:
	cd ./bin/ && rm * -rf && cd ../


