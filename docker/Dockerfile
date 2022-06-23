### Build/test container ###
# Define builder stage
FROM snore:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest -V --output-on_failure



FROM node:12.20-alpine as frontbuild
RUN mkdir -p /app
RUN npm set progress=false && npm config set depth 0 && npm cache clean --force

# The /app directory should act as the main application directory
WORKDIR /app

# Copy the app package and package-lock.json file
COPY angular-blog/package*.json ./

# Install node packages
RUN npm install

# Copy or project directory (locally) in the current directory of our docker image (/app)
COPY angular-blog/ .

# Set host to localhost / the docker image
ENV NUXT_HOST=0.0.0.0

RUN $(npm bin)/ng build --prod



### Deploy container ###
# Define deploy stage
FROM ubuntu:focal as deploy

# TODO(!): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/config_files/deploy_config .

RUN mkdir ./static
RUN mkdir ./crud
COPY --from=builder /usr/src/project/crud ./crud
COPY --from=builder /usr/src/project/static ./static
COPY --from=frontbuild /app/dist/ ./static/
COPY --from=builder /usr/src/project/authPage/ ./static/angular-blog/

# COPY --from=builder /usr/src/project/crud ./crud

# TODO(!): Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(!): Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(!): Update with real server args
CMD ["deploy_config"]
