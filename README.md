# Timecobot 2

Telegram bot provides time-based currency. The accumulated time can be used to block another user. There are no
discriminatory rules, administrators and moderators. Bot also can be used to pin messages.

## Usage

To start using the system, just add [`@timecobot`](https://t.me/timecobot) to the group with admin rights. Bot
understands spoken language (english and russian). You can see [examples there](https://habr.com/ru/post/556474/).
Experiment!

## Features

1. faq
1. time account
1. ban [to, duration]
1. unban [to, duration]
1. transfer [to, duration]
1. pin [message, duration]
1. clearing self messages [duration]

## Build

We need [`CMake`](https://cmake.org/download) build system and [`Conan`](https://conan.io) manager libraries.

Prepare the project for building in release mode:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B bin
```

Building together the application and tests:

```shell
cmake --build bin --target all
```  

After that, the main self-executable utility will appear in the `bin` directory under the name `app`. The tests can be
run by launching the `test` file located nearby.

## Containerization

We need [`Docker`](https://www.docker.com/) installed:

```shell
docker build . -t utility
```

After that, we can run the utility in the container (if needed):

```shell
docker run -it utility
```

## Deploy

The sample configuration already exists in the `.do` directory. To deploy the application, just click the button.

[![Deploy to DigitalOcean](https://www.deploytodo.com/do-btn-blue-ghost.svg)](https://cloud.digitalocean.com/apps/new?repo=https://github.com/demidko/timecobot2/tree/main)

## Conventions

* The entry point must be located in the `Main.cpp` file for correct build script work.
* To initialize resources, we using [modern parameter passing by value](https://habr.com/ru/post/460955/), rather than a
  constant link.
* Only the result of the compilation of `* .cpp` files in the` src` folder is included in the release assembly.
* The `src` folder contains the` *.cpp` and `*.h` project files together.
* The `test` folder contains the` *.cpp` and `*.h` project test files together.
* Each `*.h` file must define only one entity in the global namespace, whose name must match the file name.
* The contents of `*.cpp` files not declared in` *.h` file must be protected from `external linkage` from others
  compilation units by adding them to the anonymous namespace or adding the keyword `static`.
