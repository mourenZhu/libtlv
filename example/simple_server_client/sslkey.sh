#!/bin/bash

server_key='server.key' # 私钥
server_csr='server.csr' # 证书申请文件
server_crt='server.crt' # 证书

openssl genrsa -out $server_key 4096
openssl req -new -key $server_key -out $server_csr

openssl x509 -req -in $server_csr -out $server_crt -signkey $server_key -days 3650
