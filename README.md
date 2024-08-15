# LibTLV
LibTLV is a TCP-based application layer protocol library, which is written in C language and based on the C11 standard. TLV is Type-Length-Value.  
The core code of libtlv is based on libevent, on which a tlv server and client are implemented. Users can use libtlv to easily communicate between the server and the client.

| type(char *) | length(size_t) | value(void *) |  
|:-------------|---------------:|--------------:|
| "type\r\n"   |         length |         value |

# build & install
```shell
cd build
cmake ..
make
make install
```

# Development Plan
- [X] IPV4
- [X] IPV6
- [ ] SSL (In development)

# example
- [simple_server_client](example/simple_server_client)

# License
The MIT license