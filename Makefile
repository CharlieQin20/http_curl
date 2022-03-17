VERSION  =1.00
CC   =gcc
DEBUG   =-DUSE_DEBUG
CFLAGS  =-Wall
SOURCES   =$(wildcard ./src/*.c)
INCLUDES   =-I./include
LIB_NAMES  =-ljansson -lcurl
LIB_PATH  =-L./lib
OBJ   =$(patsubst %.c, %.o, $(SOURCES))
TARGET  =http

#links
$(TARGET):$(OBJ)
	@mkdir -p bin
	$(CC) $(OBJ) $(LIB_PATH) $(LIB_NAMES) -o ./$(TARGET)$(VERSION)
	@cp ./$(TARGET)$(VERSION) bin/
	@rm -rf $(OBJ)
 
#compile
%.o: %.c
	$(CC) $(INCLUDES) $(DEBUG) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	@echo "Remove linked and compiled files......"
	rm -rf $(OBJ) $(TARGET) bin 
