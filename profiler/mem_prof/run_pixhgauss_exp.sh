./pixhomology_gauss/compile.sh
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step1 ./pixhomology_gauss/step1 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step2 ./pixhomology_gauss/step2 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step3 ./pixhomology_gauss/step3 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step4 ./pixhomology_gauss/step4 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step5 ./pixhomology_gauss/step5 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step6 ./pixhomology_gauss/step6 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step7 ./pixhomology_gauss/step7 ${1} ${1}
valgrind --tool=massif --time-unit=ms --massif-out-file=${2}/massif.out.pixh_gauss_${1}_step8 ./pixhomology_gauss/step8 ${1} ${1}
