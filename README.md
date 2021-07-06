# Timecobot2

Telegram bot provides time-based currency. The accumulated time can be used to block another user. There are no
discriminatory rules, administrators and moderators. Bot also can be used to pin messages.

## Build

We need [`cmake`](https://cmake.org/download) build system and [`conan`](https://conan.io) manager libraries.

1. Prepare the directory for building using the dependencies:
   ```shell
   cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B bin
   ```
1. Putting together the directory and tests:
   ```shell
   cmake --build bin --target all
   ```  
   After that, the main self-executable utility will appear in the `bin` directory under the name `app`. The tests can
   be run by launching the `test` file located nearby.

## Deploy

[![Deploy to DigitalOcean](https://www.deploytodo.com/do-btn-blue-ghost.svg)](https://cloud.digitalocean.com/apps/new?repo=https://github.com/demidko/timecobot2/tree/main)

You can use a Redis cluster to store the time: specify the connection string via the `DATABASE_URL`
environment variable.
