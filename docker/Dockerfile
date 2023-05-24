### Build/test container ###
# Define builder stage
FROM sudo-rm-rf:base as builder

# Share work directory
COPY . /usr/src/projects/sudo-rm-rf
WORKDIR /usr/src/projects/sudo-rm-rf/build

# Build and test
RUN cmake ..
RUN make

RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/projects/sudo-rm-rf/build/bin/server .
COPY --from=builder /usr/src/projects/sudo-rm-rf/server_configs/localrun.conf .
COPY --from=builder /usr/src/projects/sudo-rm-rf/server_configs/assign7.conf .
COPY --from=builder /usr/src/projects/sudo-rm-rf/static ./static

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["assign7.conf"]
