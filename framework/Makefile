NAME=diy-efis

CC=csc.exe
MDP=CanFly.Build.exe
FLAGS=
MDPFLAGS=
SRC=*.cs
LIBS=
RM =rm -rf

all: $(NAME)

$(NAME).pe: $(SRC)
	$(CC) $(FLAGS) $(LIBS) $(SRC) -out:$(NAME)
	$(MDP) -c $(NAME)

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME)

