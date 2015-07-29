.PHONY: build install clean test


build:
	make clean
	gcc src/hostname_resolver.c src/argparse.c src/socket.c src/udp_load_balancer.c -o build/udp_load_balancer

install:
	make build
	cp build/udp_load_balancer /usr/local/bin/udp_load_balancer
	make clean

clean:
	find build -type f -not -name '.gitkeep' -exec rm {} +

test:
	make clean
	pip install -r tests/requirements.txt
	python tests/udp_load_balancer_test.py
