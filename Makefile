C := g++ -O3 -g
#all: main
run: main .PHONY
	./main cf/coinflip.conf
main: src/agent.cpp src/environment.cpp src/main.cpp src/predict.cpp src/search.cpp src/util.cpp
	$C -o $@ $^
predict_test: src/predict_test.cpp src/predict.cpp src/util.cpp
	$C -std=c++11 -o $@ $^
predict_test1: src/predict_test1.cpp src/predict.cpp src/util.cpp
	$C -std=c++11 -o $@ $^
test_envs: main .PHONY
	ls cf/r-* | xargs -n1 ./$< | tee test_envs.log
	#Please look at test_envs.log
runall-short: .PHONY
	ls cf/short/*.conf | sed -e 's/cf\/short\/\([a-zA-Z0-9-]*\)\.conf/time .\/main cf\/short\/\1.conf >logs\/short\/\1.log 2>logs\/short\/\1.err/' | xargs -n1 -P6 -d '\n' bash -c
runall: .PHONY
	ls cf/*.conf | sed -e 's/cf\/\([a-zA-Z0-9-]*\)\.conf/.\/main cf\/\1.conf >logs\/\1.log 2>logs\/\1.err/' | xargs -n1 -P6 -d '\n' bash -c
.PHONY:
