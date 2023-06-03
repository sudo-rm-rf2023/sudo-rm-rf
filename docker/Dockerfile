### Build/test container ###
# Define builder stage
FROM sudo-rm-rf:base as builder
ARG SECRET_KEY
ENV SECRET_KEY=$SECRET_KEY

# Share work directory
COPY . /usr/src/projects/sudo-rm-rf
WORKDIR /usr/src/projects/sudo-rm-rf/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Obfuscation container ###
FROM node:14 as obfuscator

WORKDIR /app

# Install javascript-obfuscator
RUN npm install --global javascript-obfuscator

# Copy all necessary files from the builder stage
COPY --from=builder /usr/src/projects/sudo-rm-rf/dino ./dino

# Obfuscate your JavaScript files
RUN find ./dino -name "*.js" -exec javascript-obfuscator {} --output {} \;


### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

# Define the SECRET_KEY variable here too
ARG SECRET_KEY
ENV SECRET_KEY=${SECRET_KEY}

# Copy server output binary to "."
COPY --from=builder /usr/src/projects/sudo-rm-rf/build/bin/server .
COPY --from=builder /usr/src/projects/sudo-rm-rf/server_configs/localrun.conf .
COPY --from=builder /usr/src/projects/sudo-rm-rf/server_configs/assign9.conf .

# Copy all necessary files from the obfuscator stage
COPY --from=obfuscator /app/dino ./dino

# Copy static files
COPY --from=builder /usr/src/projects/sudo-rm-rf/static ./static

# This line is added to fix a bug: libjsoncpp-dev.so.25: cannot open shared object file: No such file or directory
# Other possible solutions: use static jsoncpp library instead of dynamic library
RUN apt-get update && apt-get install -y libjsoncpp-dev
RUN echo ${SECRET_KEY} > ./dino/secret_key.txt

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["assign9.conf"]
