#def var,compile macro
CC = g++
LIBNAME = -lxtpquoteapi -lxtptraderapi
LIBPATH = -L/home/zhangkai/xtp/lib
# gcc will use this var
#CFLAGS = -Wall

OBJDIR = obj
VPATH = src
SOURCE = $(wildcard $(VPATH)/*.cpp)
OBJS = $(addprefix $(OBJDIR)/, $(patsubst %.cpp,%.o,$(notdir $(SOURCE))))
OUTNAME = linux_main
#OBJS = ./obj/quote_spi.o ./obj/FileUtils.o ./obj/trade_spi.o ./obj/xtp_api_demo.o 
#INCLUDES = /usr/local/lib

#def command
main: ${OBJS}
	${CC} -o ${OUTNAME} ${OBJS} ${LIBNAME} ${LIBPATH}

#def command
clean:
	rm -f ${OBJS} ${OUTNAME}

${OBJDIR}/%.o: %.cpp
	gcc -c $< -o $@
