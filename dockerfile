FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./


#--no-cache-dir
RUN pip install --upgrade pip \
  && pip install -r requirements.txt

COPY . .
EXPOSE 8010:8010

RUN mkdir -p proto
RUN python -m grpc_tools.protoc \
	--proto_path=. \
	--python_out=./proto/ \
    --grpc_python_out=./proto/ \
	matrix.proto

RUN python fix_proto_imports.py proto/matrix_pb2_grpc.py proto.

CMD ["python", "./main.py", "--port","8010"]
