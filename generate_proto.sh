rm -rf ./proto
mkdir -p ./proto
python3 -m grpc_tools.protoc -I. ./matrix.proto --python_out=./proto --grpc_python_out=./proto
#python3 -m grpc_tools.protoc -I. ./storageServer.proto --python_out=./proto --grpc_python_out=./proto
#python3 -m grpc_tools.protoc -I proto --python_out=. --grpc_python_out=. proto/some/folder/*.proto
touch ./proto/__init__.py

# manually add .proto in the import statement i communication_pb2_grpc.py