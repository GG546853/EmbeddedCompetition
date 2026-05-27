#!/bin/bash
  set -eu

  # 第 1 步：用 stedgeai 编译模型
  stedgeai generate \
    --model blazeface_front_128_quant_pc_ff_od_wider_face.tflite \
    --target stm32n6 \
    --st-neural-art default@user_neuralart.json \
    --input-data-type uint8 \
    --output-data-type int8 \
    --no-inputs-allocation \
    --no-outputs-allocation

  # 第 2 步：复制生成的 C 文件到 Model/ 目录（供编译时用）
  cp st_ai_output/network.c .
  cp st_ai_output/network_ecblobs.h .
  # stai_network.c/h 也需要，如果 stedgeai 生成了的话
  cp st_ai_output/stai_network.c . 2>/dev/null || true
  cp st_ai_output/stai_network.h . 2>/dev/null || true
  # 网络权重头文件也要复制到工程中供 include
  cp st_ai_output/network.h . 2>/dev/null || true

  # 第 3 步：权重二进制转换成 HEX（注意地址是正点原子的 0x70200000）
  cp st_ai_output/network_atonbuf.xSPI2.raw network_data.xSPI2.bin
  arm-none-eabi-objcopy -I binary network_data.xSPI2.bin \
    --change-addresses 0x70200000 -O ihex ../Binary/network-data.hex

  echo "Done! network-data.hex generated for 0x70200000"