SOURCES = BCHDecoder.cpp ChaseDecoder.cpp bch_codec/bch_codec.c

all: main.cpp
	g++ main.cpp $(SOURCES) -O3 -o tool

test: testing.cpp
	g++ testing.cpp $(SOURCES) -fsanitize=address,undefined -fno-sanitize-recover=all -o test_tool

clean:
	rm tool || true
	rm test_tool || true
	rm bch_codec/bch_test || true
	rm bch_codec/*.o || true