cc = g++
src = $(wildcard ./*.cpp)
obj = $(patsubst %.c, %.o, $(src))
target = main


$(target): $(obj)
	$(cc) $(obj) -o $(target)

%.o: %.c
	$(cc) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(obj) $(target)
