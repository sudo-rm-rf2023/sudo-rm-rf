### Build/test container ###
# Define builder stage
FROM sudo-rm-rf:base as coverage

# Share work directory
COPY . /usr/src/projects/sudo-rm-rf
WORKDIR /usr/src/projects/sudo-rm-rf/build_coverage

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage


# (Optional) In Dockerfile, this has already been done
# automated testing and ensures that the build process stops and reports any issues if the tests fail
RUN ctest --output-on_failure