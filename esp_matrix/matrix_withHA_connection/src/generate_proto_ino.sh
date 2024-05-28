#!/bin/sh

GENERATOR_PATH=/home/tobias/Downloads/nanopb-0.4.7-linux-x86/generator/protoc-gen-nanopb
PROTO_PATH=./proto/
PROTO_OUT_PATH=./src/
PROTO_NAME=humidityStorage

rm -rf $PROTO_OUT_PATH
mkdir -p $PROTO_OUT_PATH

ln ../server/proto/$PROTO_NAME.options ./$PROTO_NAME.options

protoc --plugin=protoc-gen-nanopb=$GENERATOR_PATH \
      --experimental_allow_proto3_optional=false \
      --proto_path=$PROTO_PATH \
      --nanopb_out=-v:$PROTO_OUT_PATH  \
      $PROTO_NAME.proto

rm ./$PROTO_NAME.options


if [ ! -f "$HOME/Arduino/libraries/Nanopb/pb.h" ]; then
  RED='\033[0;31m'
  NC='\033[0m'
  echo "${RED}Library nanoPB is not yet added to the default library folder in the Arduino environment${NC}"
fi
