cc = g++
target = main
obj = main.o GitCommit.o FileGraph.o

$(target): $(obj)
	$(cc) $(obj) -o $(target)

%.o: %.c
	$(cc) -c $< -o $@
