# Source Code

**C++ backend server**

## logger.h, logger.c
Define and implement the ```Logger``` class, which provides convenient ways of outputting information based on severity level (Debug, Info, Warning, Error, Fatal)

## config_parser.h, config_parser.cc
Define and implement the ```NginxConfig``` and ```NginxConfigParser``` class, which takes Nginx config file as input, determine the validity of the config file, and extract informations.

## server.h, server.cc
Define and implement the ```server``` class which creates all necessary tools (request handlers and config parsers) for parsing requests. It also creates a ```session``` object to listen to requests.

## session.h, session.cc
Define and implement the ```session``` class which listens to and parse the HTTP requests. The requests are dispatched to specific request handler for further processing.

## echo_request_handler_factory.h, echo_request_handler_factory.cc
Define and implement the ```EchoRequestHandlerFactory``` class, a factory class for creating ```echo_request_handler```.

## static_request_handler_factory.h, static_request_handler_factory.cc
Define and implement the ```StaticRequestHandlerFactory``` class, a factory class for creating ```static_request_handler```.

## request_handler_factory.h, request_handler_factory.cc
Define and implement the virtual class ```RequestHandlerFactory``` , a factory class for creating request_handler. All specific factory class of request handler should be inherited from this class.

## request_handler.h
Defined and implement the virtual class ```RequestHandler``` for extracting information from requests and generate appropriate response message. All specific class of request handler should be inherited from this class.

## echo_request_handler.h, echo_request_handler.cc
Define and implement the ```EchoRequestHandler``` class, for echoing back request message with appropriate header appended in the front.

## static_request_handler.h, static_request_handler.cc
Define and implement the ```StaticRequestHandler``` class, for reading the files specified in the request and return the file content.

## not_found_handler.h, not_found_handler.cc
Define and implement the ```NotFoundHandler``` class, for dealing with invalid requests or unfound resources. 

## sleep_request_handler_factory.h, sleep_request_handler_factory.cc
Define and implement the ```SleepRequestHandlerFactory``` class, for creating `SleepRequestHandler` object.

## sleep_request_handler.h, sleep_request_handler.cc
Define and implement the ```SleepRequestHandler``` class, for making the server sleep for 1 second.

## health_request_handler_factory.h, health_request_handler_factory.cc
Define and implement the ```HealthRequestHandlerFactory``` class, for creating `HealthRequestHandler` object.

## health_request_handler.h, health_request_handler.cc
Define and implement the ```HealthRequestHandler``` class, for reporting the server status.

## markdown_request_handler_factory.h, markdown_request_handler_factory.cc
Define and implement the ```Markdown_RequestHandlerFactory``` class, for creating `MarkdownRequestHandler` object.

## markdown_request_handler.h, markdown_request_handler.cc
Define and implement the ```MarkdownRequestHandler``` class, for translating markdown text into HTML text.

**Angular Frontend**

## angular-blog/src/app/blog.service.ts
Implement functions for the frontend application to interact with backend server CRUD API.

## angular-blog/src/app/app.component.ts
Implement the application logic for displaying markdown posts to user.

## angular-blog/src/app/preview
Implement the markdown rendering component in the web applciation.

## angular-blog/src/app/list
Implement the list component that displays all the posts in a directory.

## angular-blog/src/app/edit
Implement the edit component that displays the markdown post content and allow the user to edit it.

# Build the code
We would like to perform an out-of-source build, so the first step is to create a build directory.

```bash
mkdir build
cd build
```

Then, calling cmake and make sequentially to build the target program.
```bash
cmake ..
make
```

For docker build, we can use the following commands.
```bash
docker build -f docker/base.Dockerfile -t snore:base .  && docker build -f docker/Dockerfile -t my_image .  && docker run --rm -p 80:80 --name my_run my_image
```

To build the angular app
```bash
ng build --base-href=/ --deploy-url=/ --prod=true
```

# Test the code
For testing of a single module, we could use gtest library. For example,

```C++
#include "gtest/gtest.h"
#include "echo_request_handler.h"
#include <vector>

class EchoTestFixture : public ::testing::Test
{
protected:
    std::vector<std::string> locations = {"/static", "EchoHandler", "./static"};
};
TEST_F(EchoTestFixture, test_empty_request)
{
    //something here
}
```

After including the necessary libraries, we first define a class as a subclass of ```testing::Test```, the variables within the class is shared for all test cases. 

Each ```TEST_F``` block is a test case, which takes in two argument, the first one is the class we defined, and the second one is the name of the specific test.

After writing the test file, we need to include it in the **CMakeLists.txt** for compilation.

```bash
gtest_discover_tests(echo_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
```

We can also test the whole program using integration_test. Details can be seen in **integration_test.sh**.

# Run the code
The compiled executable is bin/server, so we call the following to run the code (in the build directory).

```bash
bin/server ../config_files/deploy_config
```
This command will open the server at port 80.

Or we can run the following to run the server at port 8080.
```bash
bin/server ../config_files/ex_config
```

To run the angular frontend app
```bash
cd angular-blog
npm install
npm start
```

# Add a request handler
Firstly, we need to add a request handler factory class as a subclass of RequestHandlerFactory.

```C++
class RequestHandlerFactory
{
public:
    RequestHandlerFactory(std::map<std::string, std::string> args);
    virtual RequestHandler* create() = 0;
protected:
    std::map<std::string, std::string> m_args;
};
```

The created class should have a ```create``` method to return a dynamically allocated RequestHandler.

Next, we need to define the specific handler class, which should be the subclass of ```RequestHandler```.

```C++
namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
class RequestHandler
{
public:
    virtual int serve(const http::request<http::string_body> req, http::response<http::string_body> &res) = 0;
};
```

The added request handler should implement the serve method, which takes in a ```http::request<http::string_body> req```, and a ```http::response<http::string_body> &res```. The first argument is the received HTTP request, and the second argument is the referenced HTTP response. The returned int is the status code in which 0 represents success and all other codes represent failure.

Take echo handler as an example, the factory method has the following interface and implementations.

```C++
//echo_request_handler_factory.h
class EchoRequestHandlerFactory : public RequestHandlerFactory
{
public:
    EchoRequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler* create();
};
```

```C++
//echo_request_handler_factory.cc
EchoRequestHandlerFactory::EchoRequestHandlerFactory(std::map<std::string, std::string> args)
: RequestHandlerFactory(args)
{}
RequestHandler* EchoRequestHandlerFactory::create()
{
    return new EchoRequestHandler();
}
```

The EchoRequestHandler only overwrites the create method with a ```new EchoRequestHandler()```.
```C++
class EchoRequestHandler : public RequestHandler
{
public:
    EchoRequestHandler();
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);
};
```

```C++
EchoRequestHandler::EchoRequestHandler()
{
}

int EchoRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{

    res = {http::status::ok, req.version()};
    res.set(http::field::content_type, "text/plain");
    std::stringstream header_buf;
    header_buf << req.base();   // convert header to string
    res.body() = header_buf.str() + req.body();
    res.prepare_payload();
    
    return 0;
}
```

The echo handler overwrites the serve method by setting appropriate headers of response and then appending the request body to the end. After setting the headers and body of the request, ```res.prepare_payload()``` is called to convert it into a real response.