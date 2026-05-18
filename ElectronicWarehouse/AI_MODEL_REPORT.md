network(yunetn_320_qdq_int8.onnx)分析报告：


Analyzing model 
C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/Utilities/windows/stedgeai.exe analyze --target stm32n6 --name network -m C:/Users/96022/Desktop/yunetn_320_qdq_int8.onnx --st-neural-art n6-allmems-O3@C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/scripts/N6_scripts/user_neuralart.json --workspace C:/Users/96022/AppData/Local/Temp/mxAI_workspace939063120630014106916820783839375 --output C:/Users/96022/.stm32cubemx/network_output 
ST Edge AI Core v2.2.0-20266 2adc00962 
 >>>> EXECUTING NEURAL ART COMPILER 
   C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/Utilities/windows/atonn.exe -i "C:/Users/96022/.stm32cubemx/network_output/yunetn_320_qdq_int8_OE_3_3_0.onnx" --json-quant-file "C:/Users/96022/.stm32cubemx/network_output/yunetn_320_qdq_int8_OE_3_3_0_Q.json" -g "network.c" --load-mdesc "C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/scripts/N6_scripts/my_mdescs/stm32n6.mdesc" --load-mpool "C:/Users/96022/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-AI/10.2.0/scripts/N6_scripts/my_mpools/stm32n6.mpool" --save-mpool-file "C:/Users/96022/AppData/Local/Temp/mxAI_workspace939063120630014106916820783839375/neural_art__network/stm32n6.mpool" --out-dir-prefix "C:/Users/96022/AppData/Local/Temp/mxAI_workspace939063120630014106916820783839375/neural_art__network/" --optimization 3 --all-buffers-info --mvei --cache-maintenance --Oauto-sched --native-float --enable-virtual-mem-pools --Omax-ca-pipe 4 --Ocache-opt --Os --output-info-file "c_info.json" 
 <<<< DONE EXECUTING NEURAL ART COMPILER 
  
 Exec/report summary (analyze) 
 ------------------------------------------------------------------------------------------------------------------------- 
 model file         :   C:\Users\96022\Desktop\yunetn_320_qdq_int8.onnx                                                    
 type               :   onnx                                                                                               
 c_name             :   network                                                                                            
 options            :   allocate-inputs, allocate-outputs                                                                  
 optimization       :   balanced                                                                                           
 target/series      :   stm32n6npu                                                                                         
 workspace dir      :   C:\Users\96022\AppData\Local\Temp\mxAI_workspace939063120630014106916820783839375                  
 output dir         :   C:\Users\96022\.stm32cubemx\network_output                                                         
 model_fmt          :   ss/sa per tensor                                                                                   
 model_name         :   yunetn_320_qdq_int8                                                                                
 model_hash         :   0x92cbe767964f6d662de5c5744c7e0517                                                                 
 params #           :   73,840 items (288.44 KiB)                                                                          
 ------------------------------------------------------------------------------------------------------------------------- 
 input 1/1          :   'Input_0_out_0', int8(1x3x320x320), 300.00 KBytes, QLinear(1.000000000,-128,int8), activations     
 output 1/12        :   'Quantize_133_out_0', int8(1x1600x1), 1.56 KBytes, QLinear(0.003292752,-128,int8), activations     
 output 2/12        :   'Quantize_180_out_0', int8(1x400x1), 400 Bytes, QLinear(0.003212299,-128,int8), activations        
 output 3/12        :   'Quantize_227_out_0', int8(1x100x1), 100 Bytes, QLinear(0.002707064,-128,int8), activations        
 output 4/12        :   'Quantize_143_out_0', int8(1x1600x1), 1.56 KBytes, QLinear(0.003796757,-128,int8), activations     
 output 5/12        :   'Quantize_190_out_0', int8(1x400x1), 400 Bytes, QLinear(0.003871595,-128,int8), activations        
 output 6/12        :   'Quantize_237_out_0', int8(1x100x1), 100 Bytes, QLinear(0.000669989,-128,int8), activations        
 output 7/12        :   'Quantize_153_out_0', int8(1x1600x4), 6.25 KBytes, QLinear(0.011400674,-61,int8), activations      
 output 8/12        :   'Quantize_200_out_0', int8(1x400x4), 1.56 KBytes, QLinear(0.011022569,-46,int8), activations       
 output 9/12        :   'Quantize_247_out_0', int8(1x100x4), 400 Bytes, QLinear(0.012532321,-16,int8), activations         
 output 10/12       :   'Quantize_163_out_0', int8(1x1600x10), 15.62 KBytes, QLinear(0.019418273,-26,int8), activations    
 output 11/12       :   'Quantize_210_out_0', int8(1x400x10), 3.91 KBytes, QLinear(0.015351823,-33,int8), activations      
 output 12/12       :   'Quantize_257_out_0', int8(1x100x10), 1000 Bytes, QLinear(0.016416648,-45,int8), activations       
 outputs (total)    :   32.81 KBytes                                                                                       
 macc               :   0                                                                                                  
 weights (ro)       :   112,881 B (110.24 KiB) (1 segment) / -182,479(-61.8%) vs float model                               
 activations (rw)   :   844,820 B (825.02 KiB) (2 segments) *                                                              
 ram (total)        :   844,820 B (825.02 KiB) = 844,820 + 0 + 0                                                           
 ------------------------------------------------------------------------------------------------------------------------- 
 (*) 'input'/'output' buffers are allocated in the activations buffer 
Computing AI RT data/code size (target=stm32n6npu).. 
Compilation details 
   --------------------------------------------------------------------------------- 
Compiler version: 1.1.1-14 
Compiler arguments:  -i C:\Users\96022\.stm32cubemx\network_output\yunetn_320_qdq_int8_OE_3_3_0.onnx --json-quant-file C:\Users\96022\.stm32cubemx\network_output\yunetn_320_qdq_int8_OE_3_3_0_Q.json -g network.c --load-mdesc C:\Users\96022\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AI\10.2.0\scripts\N6_scripts\my_mdescs\stm32n6.mdesc --load-mpool C:\Users\96022\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AI\10.2.0\scripts\N6_scripts\my_mpools\stm32n6.mpool --save-mpool-file C:\Users\96022\AppData\Local\Temp\mxAI_workspace939063120630014106916820783839375\neural_art__network\stm32n6.mpool --out-dir-prefix C:\Users\96022\AppData\Local\Temp\mxAI_workspace939063120630014106916820783839375\neural_art__network/ --optimization 3 --all-buffers-info --mvei --cache-maintenance --Oauto-sched --native-float --enable-virtual-mem-pools --Omax-ca-pipe 4 --Ocache-opt --Os --output-info-file c_info.json 
==================================================================================== 
Memory usage information  (input/output buffers are included in activations) 
   --------------------------------------------------------------------------------- 
 flexMEM    [0x34000000 - 0x34000000]:          0  B /          0  B  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
 cpuRAM1    [0x34064000 - 0x34064000]:          0  B /          0  B  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
 cpuRAM2    [0x34100000 - 0x34200000]:          0  B /      1.000 MB  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
 npuRAM3    [0x34200000 - 0x34270000]:          0  B /    448.000 kB  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
 npuRAM4    [0x34270000 - 0x342E0000]:    400.000 kB /    448.000 kB  ( 89.29 % used) -- weights:          0  B (  0.00 % used)  activations:    400.000 kB ( 89.29 % used) 
 npuRAM5    [0x342E0000 - 0x34350000]:    425.020 kB /    448.000 kB  ( 94.87 % used) -- weights:          0  B (  0.00 % used)  activations:    425.020 kB ( 94.87 % used) 
 npuRAM6    [0x34350000 - 0x343C0000]:          0  B /    448.000 kB  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
 octoFlash  [0x71000000 - 0x78000000]:    110.235 kB /    112.000 MB  (  0.10 % used) -- weights:    110.235 kB (  0.10 % used)  activations:          0  B (  0.00 % used) 
 hyperRAM   [0x90000000 - 0x92000000]:          0  B /     32.000 MB  (  0.00 % used) -- weights:          0  B (  0.00 % used)  activations:          0  B (  0.00 % used) 
Total:                                           935.255 kB                                  -- weights:    110.235 kB                  activations:    825.020 kB 
==================================================================================== 
Used memory ranges 
   --------------------------------------------------------------------------------- 
 npuRAM4    [0x34270000 - 0x342E0000]: 0x34270000-0x342D4000 
 npuRAM5    [0x342E0000 - 0x34350000]: 0x342E0000-0x3434A420 
 octoFlash  [0x71000000 - 0x78000000]: 0x71000000-0x7101B900 
==================================================================================== 
Epochs details 
   --------------------------------------------------------------------------------- 
Total number of epochs: 72 of which 2 implemented in software 
epoch ID   HW/SW/EC Operation (SW only) 
epoch 1       HW 
epoch 2       HW 
epoch 3       HW 
epoch 4       HW 
epoch 5       HW 
epoch 6       HW 
epoch 7       HW 
epoch 8       HW 
epoch 9       HW 
epoch 10      HW 
epoch 11      HW 
epoch 12      HW 
epoch 13      HW 
epoch 14      HW 
epoch 15      HW 
epoch 16      HW 
epoch 17      HW 
epoch 18      HW 
epoch 19      HW 
epoch 20      HW 
epoch 21      HW 
epoch 22      HW 
epoch 23      HW 
epoch 24      HW 
epoch 25      HW 
epoch 26      HW 
epoch 27      HW 
epoch 28      HW 
epoch 29      HW 
epoch 30      HW 
epoch 31      HW 
epoch 32      HW 
epoch 33      HW 
epoch 34      HW 
epoch 35      HW 
epoch 36      HW 
epoch 37      HW 
epoch 38     -SW-   (    DepthToSpace    ) 
epoch 39      HW 
epoch 40      HW 
epoch 41      HW 
epoch 42      HW 
epoch 43      HW 
epoch 44      HW 
epoch 45      HW 
epoch 46      HW 
epoch 47      HW 
epoch 48      HW 
epoch 49      HW 
epoch 50      HW 
epoch 51     -SW-   (    DepthToSpace    ) 
epoch 52      HW 
epoch 53      HW 
epoch 54      HW 
epoch 55      HW 
epoch 56      HW 
epoch 57      HW 
epoch 58      HW 
epoch 59      HW 
epoch 60      HW 
epoch 61      HW 
epoch 62      HW 
epoch 63      HW 
epoch 64      HW 
epoch 65      HW 
epoch 66      HW 
epoch 67      HW 
epoch 68      HW 
epoch 69      HW 
epoch 70      HW 
epoch 71      HW 
epoch 72      HW 
==================================================================================== 
 Requested memory size by section - "stm32n6npu" target 
 ------------------------------- -------- --------- ------ --------- 
 module                              text    rodata   data       bss 
 ------------------------------- -------- --------- ------ --------- 
 network.o                         13,012   112,355      0         0 
 ll_aton_reloc_network.o                0         0      0         0 
 NetworkRuntime1020_CM55_GCC.a          0         0      0         0 
 lib (toolchain)*                       0         0      0         0 
 ll atonn runtime                   8,174    17,030     72     1,637 
 ------------------------------- -------- --------- ------ --------- 
 RT total**                        21,186   129,385     72     1,637 
 ------------------------------- -------- --------- ------ --------- 
 weights                                0   112,881      0         0 
 activations                            0         0      0   844,820 
 io                                     0         0      0         0 
 ------------------------------- -------- --------- ------ --------- 
 TOTAL                             21,186   242,266     72   846,457 
 ------------------------------- -------- --------- ------ --------- 
 *  toolchain objects (libm/libgcc*) 
 ** RT AI runtime objects (kernels+infrastructure) 
  Summary - "stm32n6npu" target 
  -------------------------------------------------- 
               FLASH (ro)      %*   RAM (rw)      % 
  -------------------------------------------------- 
  RT total        150,643   57.2%      1,709   0.2% 
  -------------------------------------------------- 
  TOTAL           263,524            846,529 
  -------------------------------------------------- 
  *  rt/total 
Creating txt report file C:\Users\96022\.stm32cubemx\network_output\network_analyze_report.txt 
elapsed time (analyze): 31.532s 
Model file:      yunetn_320_qdq_int8.onnx 
Total Flash:     263524 B (257.35 KiB) 
    Weights:     112881 B (110.24 KiB) 
    Library:     150643 B (147.11 KiB) 
Total Ram:       846529 B (826.69 KiB) 
    Activations: 844820 B (825.02 KiB) 
    Library:     1709 B (1.67 KiB) 
    Input:       307200 B (300.00 KiB included in Activations) 
    Output:      33600 B (32.81 KiB included in Activations) 
Done 
Analyze complete on AI model