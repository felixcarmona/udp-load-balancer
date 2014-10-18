import socket
import unittest
from redis import Redis
from threading import Thread
from time import sleep
import subprocess
import os
import signal
from socket import gethostbyname


class UdpLoadBalancerTestCase(unittest.TestCase):
    LOAD_BALANCER_PORT = 8137
    SERVERS_LIST = [
        ('localhost', 9239),
        ('127.0.0.1', 9240),
        ('127.0.0.1', 9241),
        ('localhost', 9242),
        ('127.0.0.1', 9243),
        ('127.0.0.1', 9244)
    ]

    def setUp(self):
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._redis = Redis()
        self._redis.flushall()
        self._start_servers()
        self._compile_load_balancer()
        self._run_udp_load_balancer()
        self._remove_load_balancer_bin()

    def tearDown(self):
        self._stop_servers()
        self._stop_udp_load_balancer()

    def test_load_balancing(self):
        for i in range(1, 99):
            self._sock.sendto('%s' % i, ('127.0.0.1', self.LOAD_BALANCER_PORT))
            sleep(0.1)

        servers_list = self._get_servers_list()
        for i in range(1, 99):
            expected_server = next(servers_list)
            value = self._redis.lpop('udp_packets')
            if value is not None:
                value = value.decode("utf-8")
            self.assertEqual('127.0.0.1:%s:%s' % (expected_server[1], i), value)

    def _get_servers_list(self):
        while True:
            for server in self.SERVERS_LIST:
                yield server

    def _start_servers(self):
        for server in self.SERVERS_LIST:
            Thread(target=self._start_udp_server, args=(server[1],)).start()
        sleep(1)

    def _compile_load_balancer(self):
        process = subprocess.Popen(
            ["gcc", "src/argparse.c", "src/socket.c", "src/udp_load_balancer.c", "src/hostname_resolver.c", "-o", "build/udp_load_balancer_test"],
            stdout=subprocess.PIPE
        )
        process.wait()

    def _run_udp_load_balancer(self):
        server_list_raw = ""
        for server in self.SERVERS_LIST:
            server_list_raw += "%s:%s, " % (server[0], server[1])

        server_list_raw = server_list_raw.strip(", ")

        self._load_balancer_process = subprocess.Popen(
            ["build/udp_load_balancer_test", "-p", "%s" % self.LOAD_BALANCER_PORT, "-s", server_list_raw],
            stdout=subprocess.PIPE,
            preexec_fn=os.setsid
        )
        sleep(1)

    def _stop_servers(self):
        for server in self.SERVERS_LIST:
            self._sock.sendto(b'die', server)

    def _stop_udp_load_balancer(self):
        os.killpg(self._load_balancer_process.pid, signal.SIGTERM)

    def _remove_load_balancer_bin(self):
        try:
            os.remove('build/udp_load_balancer_test')
        except OSError:
            pass

    @staticmethod
    def _start_udp_server(port):
        """
        @param port: int
        """
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(('127.0.0.1', port))

        redis = Redis()
        while True:
            data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes
            data = data.decode("utf-8")

            if data == 'die':
                return

            value = '127.0.0.1:%s:%s' % (port, data)
            redis.rpush('udp_packets', value)


if __name__ == '__main__':
    unittest.main()
