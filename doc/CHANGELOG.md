# Changelog
Producer-Consumer Problem sandbox

## [Unreleased]

## [0.0.1] - 2021-09-10

### Descr
`pcp-classes` update includes:  
- Prod-Cons problem split into object units in namespace `pcp`. See more on [pcp-classes.png](diagrams/img/pcp-classes.png) 
- [deque](../src/deque.hpp) minor upgrades
- thread-safe [logger](../src/logger.hpp) draft  

### Added
- [src](../src/) dir
    - [deque](../src/deque.hpp), [logger](../src/logger.hpp) modules
    - [buffer](../src/pcp-buffer.hpp), [client](../src/pcp-client.hpp), [server](../src/pcp-server.hpp) modules in `pcp` namespace
- [doc](.) dir
    - this `CHANHELOG.md`
    - [diagrams](diagrams/) dir
        - [Makefile](diagrams/Makefile) for image generation
        - [global.skin](diagrams/global.skin) for PlantUML configs
        - [src](diagrams/src) dir for uml source files
            - [pcp-classes.puml](diagrams/src/pcp-classes.puml)
        - [img](diagrams/img) dir for diagrams output
            - [pcp-classes.png](diagrams/img/pcp-classes.png)
- [CMakeLists.txt](../CMakeLists.txt)
- [build.sh](../build.sh)  

### TODO
- tests
- make the current model data agnostic
- extend the pcp::Server to work with multiple producers/consumers
- upgrade or replace the current logger to reduce blocking
