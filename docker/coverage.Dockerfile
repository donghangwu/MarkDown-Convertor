# Define builder stage
FROM snore:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build server binary first, as integration test need it
RUN cmake ..
RUN make

# Make coverage
WORKDIR /usr/src/project/build_coverage
RUN pwd
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage