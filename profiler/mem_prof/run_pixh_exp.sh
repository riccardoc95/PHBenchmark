./pixhomology/compile.sh
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step1 ./pixhomology/step1 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step2 ./pixhomology/step2 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step3 ./pixhomology/step3 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step4 ./pixhomology/step4 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step5 ./pixhomology/step5 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step6 ./pixhomology/step6 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step7 ./pixhomology/step7 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_${1}_step8 ./pixhomology/step8 ${1} ${1}
