gcc -O3 main.c file.c define.c geometry.c index.c core.c -o mapmatch
mapmatch ./
gcc -O3 transform.c file.c define.c geometry.c index.c core.c -o trajectory
trajectory ./ > roadnet.txt
