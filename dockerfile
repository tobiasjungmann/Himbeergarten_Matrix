FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./

RUN pip install --no-cache-dir --upgrade pip \
  && pip install --no-cache-dir -r requirements.txt

COPY . .
RUN dir -s  
#RUN ./generate_proto.sh
RUN mkdir -p proto
#RUN cd proto
#RUN python -m grpc_tools.protoc -I proto --python_out=. --grpc_python_out=. proto/*.proto
#RUN cd ..
RUN python -m grpc_tools.protoc \
	 --proto_path=. --python_out=./proto/ --grpc_python_out=./proto/ matrix.proto 

CMD ["python", "./main.py"]
