#include "sleep_request_handler.h"
#define TIME_SLEEPING 1

SleepRequestHandler::SleepRequestHandler()
{
}

int SleepRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{
    sleep(TIME_SLEEPING);
    res.version(req.version());
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Sleeping";
    res.prepare_payload();

    return 0;
}
