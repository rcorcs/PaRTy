OPT=-O3

echo 'GCC/OMP'
g++ /home/rcor/bin/exp/execution_timer.cpp $1.omp.cpp $OPT -fopenmp -pthread -lm -o $1.omp.st.gcc -std=c++11 -DUSE_OMP_STATIC
g++ /home/rcor/bin/exp/execution_timer.cpp $1.omp.cpp $OPT -fopenmp -pthread -lm -o $1.omp.dy.gcc -std=c++11

g++ /home/rcor/bin/exp/execution_timer.cpp $1.omp.cpp $OPT -pthread -lm -o $1.seq.gcc -std=c++11

echo 'LLVM/OMP'
clang++ /home/rcor/bin/exp/execution_timer.cpp $1.omp.cpp $OPT -fopenmp -pthread -lm -L/home/rcor/dev/tapir/TEdIR/tapir/build/lib/ -o $1.omp.st.llvm -std=c++11 -DUSE_OMP_STATIC
clang++ /home/rcor/bin/exp/execution_timer.cpp $1.omp.cpp $OPT -fopenmp -pthread -lm -L/home/rcor/dev/tapir/TEdIR/tapir/build/lib/ -o $1.omp.dy.llvm -std=c++11

clang++ $1.omp.cpp $OPT -fopenmp -pthread -lm -L/home/rcor/dev/tapir/TEdIR/tapir/build/lib/ -o $1.omp.llvm.ll -std=c++11 -emit-llvm -S

clang++ /home/rcor/bin/exp/execution_timer.cpp $1.omp.cpp $OPT -pthread -lm -o $1.seq.llvm -std=c++11


echo 'GCC/MyPaRTy'
g++ /home/rcor/bin/exp/execution_timer.cpp $1.ws.cpp $OPT -pthread -lm -o $1.ws.gcc -std=c++11
g++ /home/rcor/bin/exp/execution_timer.cpp $1.st.cpp $OPT -pthread -lm -o $1.st.gcc -std=c++11
g++ /home/rcor/bin/exp/execution_timer.cpp $1.ck.cpp $OPT -pthread -lm -o $1.ck.gcc -std=c++11
g++ /home/rcor/bin/exp/execution_timer.cpp $1.dy.cpp $OPT -pthread -lm -o $1.dy.gcc -std=c++11

echo 'LLVM/MyPaRTy'
clang++ /home/rcor/bin/exp/execution_timer.cpp $1.st.cpp $OPT -pthread -lm -o $1.st.llvm -std=c++11
clang++ $1.st.cpp $OPT -pthread -lm -o $1.st.llvm.ll -std=c++11 -emit-llvm -S
clang++ /home/rcor/bin/exp/execution_timer.cpp $1.ck.cpp $OPT -pthread -lm -o $1.ck.llvm -std=c++11
clang++ /home/rcor/bin/exp/execution_timer.cpp $1.dy.cpp $OPT -pthread -lm -o $1.dy.llvm -std=c++11
clang++ /home/rcor/bin/exp/execution_timer.cpp $1.ws.cpp $OPT -pthread -lm -o $1.ws.llvm -std=c++11

