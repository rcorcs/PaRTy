MAXEXEC=15

echo ./$1.st.gcc
python ~/bin/exp/statrun.py --cmd="./$1.st.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.ws.gcc
python ~/bin/exp/statrun.py --cmd="./$1.ws.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.ck.gcc
python ~/bin/exp/statrun.py --cmd="./$1.ck.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.dy.gcc
python ~/bin/exp/statrun.py --cmd="./$1.dy.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.omp.st.gcc
python ~/bin/exp/statrun.py --cmd="./$1.omp.st.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.omp.dy.gcc
python ~/bin/exp/statrun.py --cmd="./$1.omp.dy.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.seq.gcc
python ~/bin/exp/statrun.py --cmd="./$1.seq.gcc" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer


echo ./$1.st.llvm
python ~/bin/exp/statrun.py --cmd="./$1.st.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.ws.llvm
python ~/bin/exp/statrun.py --cmd="./$1.ws.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.ck.llvm
python ~/bin/exp/statrun.py --cmd="./$1.ck.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.dy.llvm
python ~/bin/exp/statrun.py --cmd="./$1.dy.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

export LD_LIBRARY_PATH="/home/rcor/dev/tapir/TEdIR/tapir/build/lib"
echo ./$1.omp.st.llvm
python ~/bin/exp/statrun.py --cmd="./$1.omp.st.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer
echo ./$1.omp.dy.llvm
python ~/bin/exp/statrun.py --cmd="./$1.omp.dy.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

echo ./$1.seq.llvm
python ~/bin/exp/statrun.py --cmd="./$1.seq.llvm" -c 0.96 --max-error 0.01 --min-exec 3 --max-exec $MAXEXEC --sys-timer

