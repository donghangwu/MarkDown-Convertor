#!/bin/bash

# arg1: server binary
# arg2: server config file
# arg3: request payloads directory
# arg4: echo or static tests

send_request() {
    # arg1: server request payload
    # arg2: server port
    local response
    response=$(echo "$1" | timeout 0.03 nc localhost "$2")
    echo "$response"
}

send_sleep_request() {
    # arg1: server request payload
    # arg2: server port
    local response
    response=$(echo "$1" | timeout 2.03 nc localhost "$2")
    echo "$response"
}

compare() {
    # arg1: request
    # arg2: response
    # arg3: request payload filename
    local response_body
    # filter out response header
    response_body=$(sed '1,/^\r\{0,1\}$/d' <<< "$2")
    local difference
    # compare request body and response body
    difference=$(diff <(printf "%s" "$1") <(printf "%s" "$response_body"))
    if [[ -z $difference ]]
    then
        echo "Integration test payload: $3 succeed!" >&2
        echo 0
    else
        echo "Integration test payload: $3 failed!" >&2
        echo "$difference" >&2
        echo 1
    fi
}

clean_up() {
    # arg1: server pid
    kill "$1"
}


# sanitize arguments
if [[ -e $1 ]]
then
    if [[ ! -x $1 ]]
    then
        echo "Server binary: $1 is not executable!" >&2
        exit 1
    fi
else
    echo "Server binary: $1 does not exist!" >&2
    exit 1
fi

if [[ -e $2 ]]
then
    if [[ ! ( -f $2 && -r $2 ) ]]
    then
        echo "Server config: $2 is not a readable file!" >&2
        exit 1
    fi
else
    echo "Server config: $2 does not exist!" >&2
    exit 1
fi

if [[ -e $3 ]]
then
    if [[ ! -d $3 ]]
    then
        echo "Request payload directory: $3 is not directory!" >&2
        exit 1
    fi
else
    echo "Request payload directory: $3 does not exists!" >&2
    exit 1
fi

# start server
$1 "$2" &
server_pid=$!
# setup trap function so that we always kill the server if something went wrong
trap 'clean_up "$server_pid"' ERR INT TERM EXIT
server_port=$(lsof -Pan -p $server_pid -i 2>/dev/null | tr -s ' ' | cut -d ' ' -f 9 | grep -E '^\*:[[:digit:]]+$' | cut -c 3-)

# send request
final_result=0
for file in "$3"/*; do
    if [[ -f $file && -r $file ]]
    then
        request=$(cat "$file")
        if [[ "$4" = "echo" ]]
        then
            response=$(send_request "$request" "$server_port")
            result=$(compare "$request" "$response" "$file")
        elif [[ "$4" = "static" ]]
        then
            # In testing static, test payload file must be named exactly the same as the requested static file
            static_file=$(cat "./static/${file}")
            response=$(send_request "${request}" "$server_port")
            result=$(compare "${static_file}" "$response" "$file")
        elif [[ "$4" = "sleep" ]]
        then
            response=$(send_sleep_request "${request}" "$server_port")
            if [[ "*Sleeping*" == "$response" ]]
            then
                result=1
            else
                result=0
            fi
        elif [[ "$4" = "health" ]]
        then
            response=$(send_sleep_request "${request}" "$server_port")
            if [[ "*OK*" == "$response" ]]
            then
                result=1
            else
                result=0
            fi
        fi

        if [[ "$result" -eq 1 ]]
        then
            final_result=1
        fi
    fi
done

# shutdown server
# kill $server_pid
# exit 0 on success, 1 on fail
exit "$final_result"