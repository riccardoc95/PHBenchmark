gcc -o ./pixhomology/profiler ./pixhomology/pixhom.c -pg -std=c99
gcc -o ./pixhomology_gauss/profiler ./pixhomology_gauss/pixhom.c -pg -std=c99 -lm

# pixhomology
./pixhomology/profiler 32 32
gprof pixhomology/profiler > pixhomology/out_32.prof

./pixhomology/profiler 128 128
gprof pixhomology/profiler > pixhomology/out_128.prof

./pixhomology/profiler 256 256
gprof pixhomology/profiler > pixhomology/out_256.prof

./pixhomology/profiler 512 512
gprof pixhomology/profiler > pixhomology/out_512.prof

./pixhomology/profiler 1024 1024
gprof pixhomology/profiler > pixhomology/out_1024.prof

./pixhomology/profiler 2048 2048
gprof pixhomology/profiler > pixhomology/out_2048.prof

./pixhomology/profiler 4096 4096
gprof pixhomology/profiler > pixhomology/out_4096.prof

./pixhomology/profiler 8192 8192
gprof pixhomology/profiler > pixhomology/out_8192.prof



# pixhomology_gauss
./pixhomology_gauss/profiler 32 32
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_32.prof

./pixhomology_gauss/profiler 128 128
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_128.prof

./pixhomology_gauss/profiler 256 256
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_256.prof

./pixhomology_gauss/profiler 512 512
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_512.prof

./pixhomology_gauss/profiler 1024 1024
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_1024.prof

./pixhomology_gauss/profiler 2048 2048
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_2048.prof

./pixhomology_gauss/profiler 4096 4096
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_4096.prof

./pixhomology_gauss/profiler 8192 8192
gprof pixhomology_gauss/profiler > pixhomology_gauss/out_8192.prof
