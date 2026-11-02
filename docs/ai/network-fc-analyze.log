
Analyzing model 
C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/Utilities/windows/stedgeai.exe analyze --target stm32n6 --name network_fc -m C:/Users/96022/Desktop/FACE_INT8/mobilefacenet_int8_faces.onnx --st-neural-art net_fc_flash@C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/scripts/N6_scripts/user_neuralart.json --workspace C:/Users/96022/AppData/Local/Temp/mxAI_workspace1223249555747003264874211895863089 --output C:/Users/96022/.stm32cubemx/network_fc_output 
ST Edge AI Core v2.2.0-20266 2adc00962 
WARNING: Unsupported keys in the current profile net_fc_flash are ignored: memory_desc 
 > memory_desc is not a valid key anymore, use machine_desc instead 
WARNING: _conv1_prelu_PRelu_output_0 is not quantized 
WARNING: _conv2_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_23_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_23_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_0_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_0_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_1_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_1_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_2_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_2_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_3_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_3_model_model_3_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_34_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_34_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_0_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_0_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_1_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_1_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_2_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_2_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_3_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_3_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_4_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_4_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_5_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_4_model_model_5_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_45_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_45_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_5_model_model_0_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_5_model_model_0_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_5_model_model_1_conv_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_5_model_model_1_conv_dw_prelu_PRelu_output_0 is not quantized 
WARNING: _conv_6_sep_prelu_PRelu_output_0 is not quantized 
WARNING: output is not quantized 
 >>>> EXECUTING NEURAL ART COMPILER 
   C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/Utilities/windows/atonn.exe -i "C:/Users/96022/.stm32cubemx/network_fc_output/mobilefacenet_int8_faces_OE_3_3_0.onnx" --json-quant-file "C:/Users/96022/.stm32cubemx/network_fc_output/mobilefacenet_int8_faces_OE_3_3_0_Q.json" -g "network_fc.c" --load-mdesc "C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/Utilities/configs/stm32n6.mdesc" --load-mpool "C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/scripts/N6_scripts/my_mpools/stm32n6_net_fc.mpool" --save-mpool-file "C:/Users/96022/AppData/Local/Temp/mxAI_workspace1223249555747003264874211895863089/neural_art__network_fc/stm32n6_net_fc.mpool" --out-dir-prefix "C:/Users/96022/AppData/Local/Temp/mxAI_workspace1223249555747003264874211895863089/neural_art__network_fc/" --optimization 3 --all-buffers-info --mvei --cache-maintenance --Oauto-sched --native-float --enable-virtual-mem-pools --Omax-ca-pipe 4 --Ocache-opt --Os --output-info-file "c_info.json" --network-name "network_fc" 
 <<<< DONE EXECUTING NEURAL ART COMPILER 
  
 Exec/report summary (analyze) 
 -------------------------------------------------------------------------------------------------------------------- 
 model file         :   C:\Users\96022\Desktop\FACE_INT8\mobilefacenet_int8_faces.onnx                                
 type               :   onnx                                                                                          
 c_name             :   network_fc                                                                                    
 options            :   allocate-inputs, allocate-outputs                                                             
 optimization       :   balanced                                                                                      
 target/series      :   stm32n6npu                                                                                    
 workspace dir      :   C:\Users\96022\AppData\Local\Temp\mxAI_workspace1223249555747003264874211895863089            
 output dir         :   C:\Users\96022\.stm32cubemx\network_fc_output                                                 
 model_fmt          :   ss/sa per tensor                                                                              
 model_name         :   mobilefacenet_int8_faces                                                                      
 model_hash         :   0x37a5b97189de3272bf7e6fa09819f172                                                            
 params #           :   993,472 items (1021.38 KiB)                                                                   
 -------------------------------------------------------------------------------------------------------------------- 
 input 1/1          :   'Input_0_out_0', int8(1x3x112x112), 36.75 KBytes, QLinear(0.007843138,0,int8), activations    
 output 1/1         :   'BatchNormalization_290_out_0', f32(1x128), 512 Bytes, activations                            
 macc               :   0                                                                                             
 weights (ro)       :   1,156,289 B (1129.19 KiB) (1 segment) / -2,817,599(-70.9%) vs float model                     
 activations (rw)   :   4,116,480 B (3.93 MiB) (6 segments) *                                                         
 ram (total)        :   4,116,480 B (3.93 MiB) = 4,116,480 + 0 + 0                                                    
 -------------------------------------------------------------------------------------------------------------------- 
 (*) 'input'/'output' buffers are allocated in the activations buffer 
Computing AI RT data/code size (target=stm32n6npu).. 
Compilation details 
   --------------------------------------------------------------------------------- 
Compiler version: 1.1.1-14 
Compiler arguments:  -i C:\Users\96022\.stm32cubemx\network_fc_output\mobilefacenet_int8_faces_OE_3_3_0.onnx --json-quant-file C:\Users\96022\.stm32cubemx\network_fc_output\mobilefacenet_int8_faces_OE_3_3_0_Q.json -g network_fc.c --load-mdesc C:\Users\96022\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AI\10.2.0\Utilities\configs\stm32n6.mdesc --load-mpool C:\Users\96022\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AI\10.2.0\scripts\N6_scripts\my_mpools\stm32n6_net_fc.mpool --save-mpool-file C:\Users\96022\AppData\Local\Temp\mxAI_workspace1223249555747003264874211895863089\neural_art__network_fc\stm32n6_net_fc.mpool --out-dir-prefix C:\Users\96022\AppData\Local\Temp\mxAI_workspace1223249555747003264874211895863089\neural_art__network_fc/ --optimization 3 --all-buffers-info --mvei --cache-maintenance --Oauto-sched --native-float --enable-virtual-mem-pools --Omax-ca-pipe 4 --Ocache-opt --Os --output-info-file c_info.json --network-name network_fc 
==================================================================================== 
Memory usage information  (input/output buffers are included in activations) 
   --------------------------------------------------------------------------------- 
 cpuRAM1    [0x34080000 - 0x34080000]:          0  B /          0  B  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
 cpuRAM2    [0x34100000 - 0x34200000]:      1.000 MB /      1.000 MB  (100.00 % used) -- weights:          0  B (  0.00 % used)  activations:      1.000 MB (100.00 % used) 
 npuRAM3    [0x34200000 - 0x34270000]:    448.000 kB /    448.000 kB  (100.00 % used) -- weights:          0  B (  0.00 % used)  activations:    448.000 kB (100.00 % used) 
 npuRAM4    [0x34270000 - 0x342E0000]:    448.000 kB /    448.000 kB  (100.00 % used) -- weights:          0  B (  0.00 % used)  activations:    448.000 kB (100.00 % used) 
 npuRAM5    [0x342E0000 - 0x34350000]:    448.000 kB /    448.000 kB  (100.00 % used) -- weights:          0  B (  0.00 % used)  activations:    448.000 kB (100.00 % used) 
 npuRAM6    [0x34350000 - 0x343C0000]:     84.000 kB /    448.000 kB  ( 18.75 % used) -- weights:          0  B (  0.00 % used)  activations:     84.000 kB ( 18.75 % used) 
 octoFlash  [0x71200000 - 0x71E00000]:      1.103 MB /     12.000 MB  (  9.19 % used) -- weights:      1.103 MB (  9.19 % used)  activations:          0  B (  0.00 % used) 
 hyperRAM   [0x90000000 - 0x92000000]:      1.531 MB /     32.000 MB  (  4.79 % used) -- weights:          0  B (  0.00 % used)  activations:      1.531 MB (  4.79 % used) 
Total:                                             5.029 MB                                  -- weights:      1.103 MB                  activations:      3.926 MB 
==================================================================================== 
Used memory ranges 
   --------------------------------------------------------------------------------- 
 cpuRAM2    [0x34100000 - 0x34200000]: 0x34100000-0x34200000 
 npuRAM3    [0x34200000 - 0x34270000]: 0x34200000-0x34270000 
 npuRAM4    [0x34270000 - 0x342E0000]: 0x34270000-0x342E0000 
 npuRAM5    [0x342E0000 - 0x34350000]: 0x342E0000-0x34350000 
 npuRAM6    [0x34350000 - 0x343C0000]: 0x34350000-0x34365000 
 octoFlash  [0x71200000 - 0x71E00000]: 0x71200000-0x7131A4D0 
 hyperRAM   [0x90000000 - 0x92000000]: 0x90000000-0x90188000 
==================================================================================== 
Epochs details 
   --------------------------------------------------------------------------------- 
Total number of epochs: 183 of which 110 implemented in software 
epoch ID   HW/SW/EC Operation (SW only) 
epoch 1       HW 
epoch 2       HW 
epoch 3      -SW-   (  DequantizeLinear  ) 
epoch 4      -SW-   (       PRelu        ) 
epoch 5      -SW-   (   QuantizeLinear   ) 
epoch 6       HW 
epoch 7       HW 
epoch 8      -SW-   (  DequantizeLinear  ) 
epoch 9      -SW-   (       PRelu        ) 
epoch 10     -SW-   (   QuantizeLinear   ) 
epoch 11      HW 
epoch 12     -SW-   (  DequantizeLinear  ) 
epoch 13     -SW-   (       PRelu        ) 
epoch 14     -SW-   (   QuantizeLinear   ) 
epoch 15      HW 
epoch 16      HW 
epoch 17     -SW-   (  DequantizeLinear  ) 
epoch 18     -SW-   (       PRelu        ) 
epoch 19     -SW-   (   QuantizeLinear   ) 
epoch 20      HW 
epoch 21      HW 
epoch 22     -SW-   (  DequantizeLinear  ) 
epoch 23     -SW-   (       PRelu        ) 
epoch 24     -SW-   (   QuantizeLinear   ) 
epoch 25      HW 
epoch 26      HW 
epoch 27     -SW-   (  DequantizeLinear  ) 
epoch 28     -SW-   (       PRelu        ) 
epoch 29     -SW-   (   QuantizeLinear   ) 
epoch 30      HW 
epoch 31      HW 
epoch 32     -SW-   (  DequantizeLinear  ) 
epoch 33     -SW-   (       PRelu        ) 
epoch 34     -SW-   (   QuantizeLinear   ) 
epoch 35      HW 
epoch 36      HW 
epoch 37     -SW-   (  DequantizeLinear  ) 
epoch 38     -SW-   (       PRelu        ) 
epoch 39     -SW-   (   QuantizeLinear   ) 
epoch 40      HW 
epoch 41      HW 
epoch 42     -SW-   (  DequantizeLinear  ) 
epoch 43     -SW-   (       PRelu        ) 
epoch 44     -SW-   (   QuantizeLinear   ) 
epoch 45      HW 
epoch 46      HW 
epoch 47     -SW-   (  DequantizeLinear  ) 
epoch 48     -SW-   (       PRelu        ) 
epoch 49     -SW-   (   QuantizeLinear   ) 
epoch 50      HW 
epoch 51      HW 
epoch 52     -SW-   (  DequantizeLinear  ) 
epoch 53     -SW-   (       PRelu        ) 
epoch 54     -SW-   (   QuantizeLinear   ) 
epoch 55      HW 
epoch 56      HW 
epoch 57     -SW-   (  DequantizeLinear  ) 
epoch 58     -SW-   (       PRelu        ) 
epoch 59     -SW-   (   QuantizeLinear   ) 
epoch 60      HW 
epoch 61      HW 
epoch 62     -SW-   (  DequantizeLinear  ) 
epoch 63     -SW-   (       PRelu        ) 
epoch 64     -SW-   (   QuantizeLinear   ) 
epoch 65      HW 
epoch 66      HW 
epoch 67     -SW-   (  DequantizeLinear  ) 
epoch 68     -SW-   (       PRelu        ) 
epoch 69     -SW-   (   QuantizeLinear   ) 
epoch 70      HW 
epoch 71      HW 
epoch 72     -SW-   (  DequantizeLinear  ) 
epoch 73     -SW-   (       PRelu        ) 
epoch 74     -SW-   (   QuantizeLinear   ) 
epoch 75      HW 
epoch 76      HW 
epoch 77     -SW-   (  DequantizeLinear  ) 
epoch 78     -SW-   (       PRelu        ) 
epoch 79     -SW-   (   QuantizeLinear   ) 
epoch 80      HW 
epoch 81      HW 
epoch 82     -SW-   (  DequantizeLinear  ) 
epoch 83     -SW-   (       PRelu        ) 
epoch 84     -SW-   (   QuantizeLinear   ) 
epoch 85      HW 
epoch 86      HW 
epoch 87     -SW-   (  DequantizeLinear  ) 
epoch 88     -SW-   (       PRelu        ) 
epoch 89     -SW-   (   QuantizeLinear   ) 
epoch 90      HW 
epoch 91      HW 
epoch 92     -SW-   (  DequantizeLinear  ) 
epoch 93     -SW-   (       PRelu        ) 
epoch 94     -SW-   (   QuantizeLinear   ) 
epoch 95      HW 
epoch 96      HW 
epoch 97     -SW-   (  DequantizeLinear  ) 
epoch 98     -SW-   (       PRelu        ) 
epoch 99     -SW-   (   QuantizeLinear   ) 
epoch 100     HW 
epoch 101     HW 
epoch 102    -SW-   (  DequantizeLinear  ) 
epoch 103    -SW-   (       PRelu        ) 
epoch 104    -SW-   (   QuantizeLinear   ) 
epoch 105     HW 
epoch 106     HW 
epoch 107    -SW-   (  DequantizeLinear  ) 
epoch 108    -SW-   (       PRelu        ) 
epoch 109    -SW-   (   QuantizeLinear   ) 
epoch 110     HW 
epoch 111     HW 
epoch 112    -SW-   (  DequantizeLinear  ) 
epoch 113    -SW-   (       PRelu        ) 
epoch 114    -SW-   (   QuantizeLinear   ) 
epoch 115     HW 
epoch 116     HW 
epoch 117    -SW-   (  DequantizeLinear  ) 
epoch 118    -SW-   (       PRelu        ) 
epoch 119    -SW-   (   QuantizeLinear   ) 
epoch 120     HW 
epoch 121     HW 
epoch 122    -SW-   (  DequantizeLinear  ) 
epoch 123    -SW-   (       PRelu        ) 
epoch 124    -SW-   (   QuantizeLinear   ) 
epoch 125     HW 
epoch 126     HW 
epoch 127    -SW-   (  DequantizeLinear  ) 
epoch 128    -SW-   (       PRelu        ) 
epoch 129    -SW-   (   QuantizeLinear   ) 
epoch 130     HW 
epoch 131     HW 
epoch 132    -SW-   (  DequantizeLinear  ) 
epoch 133    -SW-   (       PRelu        ) 
epoch 134    -SW-   (   QuantizeLinear   ) 
epoch 135     HW 
epoch 136     HW 
epoch 137    -SW-   (  DequantizeLinear  ) 
epoch 138    -SW-   (       PRelu        ) 
epoch 139    -SW-   (   QuantizeLinear   ) 
epoch 140     HW 
epoch 141     HW 
epoch 142     HW 
epoch 143    -SW-   (  DequantizeLinear  ) 
epoch 144    -SW-   (       PRelu        ) 
epoch 145    -SW-   (   QuantizeLinear   ) 
epoch 146     HW 
epoch 147     HW 
epoch 148    -SW-   (  DequantizeLinear  ) 
epoch 149    -SW-   (       PRelu        ) 
epoch 150    -SW-   (   QuantizeLinear   ) 
epoch 151     HW 
epoch 152     HW 
epoch 153     HW 
epoch 154    -SW-   (  DequantizeLinear  ) 
epoch 155    -SW-   (       PRelu        ) 
epoch 156    -SW-   (   QuantizeLinear   ) 
epoch 157     HW 
epoch 158     HW 
epoch 159    -SW-   (  DequantizeLinear  ) 
epoch 160    -SW-   (       PRelu        ) 
epoch 161    -SW-   (   QuantizeLinear   ) 
epoch 162     HW 
epoch 163     HW 
epoch 164     HW 
epoch 165    -SW-   (  DequantizeLinear  ) 
epoch 166    -SW-   (       PRelu        ) 
epoch 167    -SW-   (   QuantizeLinear   ) 
epoch 168     HW 
epoch 169    -SW-   (  DequantizeLinear  ) 
epoch 170    -SW-   (  DequantizeLinear  ) 
epoch 171    -SW-   (  DequantizeLinear  ) 
epoch 172    -SW-   (        Conv        ) 
epoch 173    -SW-   (        Conv        ) 
epoch 174    -SW-   (        Conv        ) 
epoch 175    -SW-   (        Cast        ) 
epoch 176    -SW-   (        Cast        ) 
epoch 177    -SW-   (        Cast        ) 
epoch 178     HW 
epoch 179     HW 
epoch 180     HW 
epoch 181     HW 
epoch 182    -SW-   (  DequantizeLinear  ) 
epoch 183    -SW-   ( BatchNormalization ) 
==================================================================================== 
 Requested memory size by section - "stm32n6npu" target 
 ------------------------------- -------- ----------- ------ ----------- 
 module                              text      rodata   data         bss 
 ------------------------------- -------- ----------- ------ ----------- 
 network_fc.o                      28,492     118,920      0           0 
 NetworkRuntime1020_CM55_GCC.a     11,132           0      0           0 
 lib (toolchain)*                  11,308       2,254      0           0 
 ll atonn runtime                  12,176      22,583     72       1,637 
 ------------------------------- -------- ----------- ------ ----------- 
 RT total**                        63,108     143,757     72       1,637 
 ------------------------------- -------- ----------- ------ ----------- 
 weights                                0   1,156,289      0           0 
 activations                            0           0      0   4,116,480 
 io                                     0           0      0           0 
 ------------------------------- -------- ----------- ------ ----------- 
 TOTAL                             63,108   1,300,046     72   4,118,117 
 ------------------------------- -------- ----------- ------ ----------- 
 *  toolchain objects (libm/libgcc*) 
 ** RT AI runtime objects (kernels+infrastructure) 
  Summary - "stm32n6npu" target 
  --------------------------------------------------- 
               FLASH (ro)      %*    RAM (rw)      % 
  --------------------------------------------------- 
  RT total        206,937   15.2%       1,709   0.0% 
  --------------------------------------------------- 
  TOTAL         1,363,226           4,118,189 
  --------------------------------------------------- 
  *  rt/total 
Creating txt report file C:\Users\96022\.stm32cubemx\network_fc_output\network_fc_analyze_report.txt 
elapsed time (analyze): 32.845s 
Model file:      mobilefacenet_int8_faces.onnx 
Total Flash:     1363226 B (1.30 MiB) 
    Weights:     1156289 B (1.10 MiB) 
    Library:     206937 B (202.09 KiB) 
Total Ram:       4118189 B (3.93 MiB) 
    Activations: 4116480 B (3.93 MiB) 
    Library:     1709 B (1.67 KiB) 
    Input:       37632 B (36.75 KiB included in Activations) 
    Output:      512 B (included in Activations) 
Done 
Analyze complete on AI model