rm -rf ./proto
mkdir -p ./proto

python3 -m grpc_tools.protoc -I proto --proto_path=. --python_out=./proto/ --grpc_python_out=./proto/ matrix.proto 
python3 fix_proto_imports.py proto/matrix_pb2_grpc.py proto.

touch ./proto/__init__.py