gcc -o ./profiler ./pixhom.c -pg -std=c99

./profiler 32 32
gprof profiler > out_32.prof

./profiler 128 128
gprof profiler > out_128.prof

./profiler 256 256
gprof profiler > out_256.prof

./profiler 512 512
gprof profiler > out_512.prof

./profiler 1024 1024
gprof profiler > out_1024.prof

./profiler 2048 2048
gprof profiler > out_2048.prof

./profiler 4096 4096
gprof profiler > out_4096.prof

./profiler 8192 8192
gprof profiler > out_8192.prof

./profiler 16384 16384
gprof profiler > out_16384.prof