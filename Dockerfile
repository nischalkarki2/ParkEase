
FROM gcc:14-bookworm

WORKDIR /app

COPY backend/include ./backend/include
COPY backend/src ./backend/src
COPY data/.gitkeep ./data/.gitkeep

RUN gcc -Wall -Wextra \
    backend/src/main.c \
    backend/src/vehicle.c \
    backend/src/parking.c \
    backend/src/queue.c \
    backend/src/linkedlist.c \
    backend/src/stack.c \
    backend/src/storage.c \
    backend/src/server.c \
    -o parkease

EXPOSE 10000

CMD ["./parkease", "--server"]
