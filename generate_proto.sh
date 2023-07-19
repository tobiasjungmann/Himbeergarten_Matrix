rm -rf ./proto
mkdir -p ./proto

# correctly creates protofile in own folder
#python3 -m grpc_tools.protoc --proto_path=. --python_out=. --grpc_python_out=. matrix.proto 

python3 -m grpc_tools.protoc -I proto --proto_path=. --python_out=./proto/ --grpc_python_out=./proto/ matrix.proto 
python3 fix_proto_imports.py proto/matrix_pb2_grpc.py proto.

touch ./proto/__init__.py

# manually add .proto in the import statement i communication_pb2_grpc.py