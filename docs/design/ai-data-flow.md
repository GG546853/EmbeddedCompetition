# AI Data Flow — Embedded Warehouse Face Recognition System

Final configuration as of 2026-07-25. CROP_MODE 1 (box direct resize) is the chosen approach.

---

## 1. Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                        IMX335 Camera Sensor                         │
│                        2592 × 1944 (4:3)                            │
└───────────────┬─────────────────────────┬───────────────────────────┘
                │                         │
                ▼                         ▼
┌───────────────────────────┐ ┌───────────────────────────────────────┐
│       DCMIPP PIPE1        │ │            DCMIPP PIPE2               │
│  (Display + Face Crop)    │ │        (Model 1 NN Input)             │
│                           │ │                                       │
│  2592×1944 → 800×480      │ │  Center crop 1944×1944                │
│  RGB888 continuous DMA    │ │  → 1/4 decimation → 486×486           │
│  → g_ltdc_framebuf        │ │  → downsize → 128×128 RGB888          │
│  Layer 0 (background)     │ │  → nn_input_u8[]                     │
└───────────┬───────────────┘ └───────────┬───────────────────────────┘
            │                             │
            │                             ▼
            │               ┌─────────────────────────────────────────┐
            │               │           NPU Inference                  │
            │               │      Model 1: BlazeFace 128×128          │
            │               │      network_f (NPU accelerated)         │
            │               │                                         │
            │               │  Input: 128×128 RGB float32              │
            │               │  Output: 896 anchors                    │
            │               │    Grid 0: 16×16×2 = 512                │
            │               │    Grid 1:  8×8×6  = 384                │
            │               └───────────┬─────────────────────────────┘
            │                           │
            │                           ▼
            │               ┌─────────────────────────────────────────┐
            │               │         Post-Processing                  │
            │               │   Box decode + X bias + NMS + EMA       │
            │               │                                         │
            │               │  → ai_result_t (detections + keypoints)  │
            │               └───────────┬─────────────────────────────┘
            │                           │
            │                           ▼
            │               ┌─────────────────────────────────────────┐
            │               │     Draw Boxes on Display                │
            │               │   NN coords → display coords             │
            │               │   PIPE1 paused during draw               │
            │               │   → g_ltdc_framebuf (Layer 0 overlay)    │
            │               └─────────────────────────────────────────┘
            │
            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    Face Crop from Display FB                         │
│   ai_crop_resize_face_112()  (CPU software resize)                   │
│                                                                      │
│   CROP_MODE = 1: Detection box + 10% padding → 112×112 RGB888        │
│   Source: g_ltdc_framebuf (800×480), DMA2D-accelerated resize        │
│   Output: face_crop_buf[112×112×3]                                   │
└───────────────┬─────────────────────────────────────────────────────┘
                │
                ▼
┌─────────────────────────────────────────────────────────────────────┐
│           Quantize uint8→int8 + NPU Inference                        │
│      Model 2: Face Re-ID (network_fc, NPU accelerated)               │
│                                                                      │
│   Input:  112×112×3 int8                                             │
│   Output: 128-dim float32 embedding (L2-normalized)                  │
└───────────────┬─────────────────────────────────────────────────────┘
                │
                ▼
┌─────────────────────────────────────────────────────────────────────┐
│                     Face Matching                                    │
│                                                                      │
│   1:N Identify: L2 distance to all gallery entries                  │
│     - best_L2 < FACE_MATCH_THRESHOLD (0.40)                         │
│     - best_L2 / second_L2 < FACE_RATIO_THRESHOLD (0.93)             │
│     - Skip ratio check if best_L2 < FACE_CONFIDENT_DIST (0.25)      │
│                                                                      │
│   1:1 Verify: L2 distance to selected entry                         │
│     - dist < FACE_VERIFY_THRESHOLD (0.17)                           │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Thread Architecture

| Thread | Priority | Stack | Role |
|--------|----------|-------|------|
| Sensor_Task | Normal+2 | — | DCMIPP start/stop, camera config |
| AI_Task | Normal+1 | 2048×4 | Model 1+2 inference, face matching, serial cmds |
| LV_Task | Normal | 128KB | LVGL UI rendering, auto-unlock polling |

Data flow synchronization:
- `cam_frame_sem` (binary semaphore): Sensor_Task releases per frame → AI_Task acquires
- `dma2d_mutex`: protects DMA2D access between AI_Task (draw boxes) and LV_Task (UI rendering)
- `g_auto_unlock`: AI_Task sets → LV_Task `ui_poll_auto_unlock()` consumes (every 5ms)

---

## 3. Coordinate System

```
Sensor:     2592 × 1944  (columns 0–2591)
PIPE2 crop: 1944 × 1944  (columns 324–2267, centered)
NN input:    128 × 128   (Model 1 coordinate space [0,1]×[0,1])
Display:     800 × 480   (PIPE1 output to LCD)

NN → Display mapping (X axis accounts for PIPE2 center crop):
  x_display = x_nn × NN2DISP_SCALE_X + NN2DISP_OFFSET_X
            = x_nn × 600 + 100

  width_display = width_nn × 600

  y_display = y_nn × 480
```

---

## 4. Model 1 Parameters (BlazeFace Detection)

Defined in `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h`:

| Parameter | Value | Description |
|-----------|-------|-------------|
| `AI_FD_CONF_THRESHOLD` | 0.75 | Minimum confidence for valid detection |
| `AI_FD_IOU_THRESHOLD` | 0.50 | NMS IoU threshold |
| `AI_FD_MAX_DETECTIONS` | 1 | Max detections per frame |
| `AI_FD_NUM_KEYPOINTS` | 6 | Keypoints: LEye, REye, Nose, Mouth, LEar, REar |
| `AI_FD_IMG_SIZE` | 128 | Model 1 input resolution |
| `AI_FD_GRID_0_NB_BOXES` | 512 | Grid 0 anchors (16×16×2) |
| `AI_FD_GRID_1_NB_BOXES` | 384 | Grid 1 anchors (8×8×6) |
| `AI_FD_BOX_SCALE_W` | 1.00 | Box width scale (no artificial scaling) |
| `AI_FD_BOX_SCALE_H` | 1.00 | Box height scale (no artificial scaling) |
| `AI_FD_EMA_ALPHA` | 0.20 | EMA temporal smoothing factor |
| `AI_FD_TRACK_IOU_THRESH` | 0.50 | IoU threshold for frame-to-frame tracking |
| `AI_FD_X_BIAS` | 0.015 | Systematic X offset correction (normalized [0,1]) |

### Box Decoding Formula

```
x_center   = raw[0] × (1/128) + ax + AI_FD_X_BIAS
y_center   = raw[1] × (1/128) + ay
width      = raw[2] × (1/128) × AI_FD_BOX_SCALE_W
height     = raw[3] × (1/128) × AI_FD_BOX_SCALE_H

keypoints[k][0] = raw[4 + k×2]     × (1/128) + ax + AI_FD_X_BIAS
keypoints[k][1] = raw[4 + k×2 + 1] × (1/128) + ay
```

Where `(ax, ay)` is the grid anchor center and `raw[]` are the decoded model outputs.

---

## 5. Face Crop — CROP_MODE 1 (Final)

Defined in `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c`:

| Parameter | Value | Description |
|-----------|-------|-------------|
| `CROP_MODE` | **1** | Detection-box direct resize (selected final) |
| `BOX_PAD_RATIO` | 0.10 | 10% padding on each side of detection box |
| `CROP_OUT_SZ` | 112 | Output size (112×112 RGB888) |

### Mode 1 Algorithm

```
bx = x_center × 600 + 100    (NN → display X)
by = y_center × 480          (NN → display Y)
bw = width × 600             (NN → display width)
bh = height × 480            (NN → display height)

pad_w = bw × 0.1
pad_h = bh × 0.1

crop_x0 = bx - bw/2 - pad_w
crop_y0 = by - bh/2 - pad_h
crop_w  = bw + pad_w × 2
crop_h  = bh + pad_h × 2
```

The non-square crop region is then DMA2D-resized to 112×112 RGB888.

**Comparison**: Mode 1 (box direct resize) achieved better recognition accuracy than Mode 0 (eye-distance square crop). Mode 1 preserves the full face region including ears and chin without forcing a square crop, then lets the network handle the non-square resize.

---

## 6. Model 2 Parameters (Face Re-ID)

Defined in `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h`:

| Parameter | Value | Description |
|-----------|-------|-------------|
| `FACE_EMBEDDING_DIM` | 128 | Embedding vector dimension |
| `FACE_GALLERY_MAX` | 10 | Max enrolled faces |
| `FACE_ENROLL_SAMPLES` | 10 | Multi-shot enrollment captures per person |
| `FACE_MATCH_THRESHOLD` | 0.40 | 1:N match max L2 distance |
| `FACE_RATIO_THRESHOLD` | 0.93 | best_L2 / second_L2 ratio check |
| `FACE_CONFIDENT_DIST` | 0.25 | Skip ratio check below this distance |
| `FACE_VERIFY_THRESHOLD` | 0.17 | 1:1 verify max L2 distance |
| `ENROLL_QUALITY_COSIM` | 0.88 | Reject enrollment sample if cosine similarity to running average < this |

### Matching Logic

**1:N Identify** (`ai_face_identify`):
1. Compute L2 distance to every gallery entry
2. Find best and second-best match
3. Accept if: `best_L2 < 0.40` AND (`best_L2 < 0.25` OR `best_L2/second_L2 < 0.93`)

**1:1 Verify** (`ai_face_verify`):
1. Compute L2 distance to the selected gallery entry
2. Accept if: `dist < 0.17`

---

## 7. Face Distance Distribution Data

### Self (same person) — 262 samples

| Distance Range | Count | Cumulative |
|---------------|-------|------------|
| 0.00–0.05 | 43 | 16.4% |
| 0.05–0.10 | 148 | 72.9% |
| 0.10–0.15 | 60 | 95.8% |
| 0.15–0.20 | 11 | 100.0% |
| 0.20–0.25 | 0 | 100.0% |

**Statistics**: Mean ≈ 0.088, Max = 0.197

### Non-self (different person) — 140 samples

| Distance Range | Count | Cumulative |
|---------------|-------|------------|
| 0.00–0.05 | 0 | 0.0% |
| 0.05–0.10 | 0 | 0.0% |
| 0.10–0.15 | 0 | 0.0% |
| 0.15–0.20 | 2 | 1.4% |
| 0.20–0.25 | 2 | 2.9% |
| 0.25–0.30 | 12 | 11.4% |
| 0.30–0.35 | 22 | 27.1% |
| 0.35–0.40 | 22 | 42.9% |
| 0.40–0.45 | 24 | 60.0% |
| 0.45–0.50 | 21 | 75.0% |
| 0.50–0.55 | 23 | 91.4% |
| 0.55–0.60 | 10 | 98.6% |
| 0.60–0.65 | 2 | 100.0% |

**Statistics**: Mean ≈ 0.404, Min = 0.155

### Threshold Analysis

| Threshold | Self Accept Rate | Non-self Reject Rate |
|-----------|-----------------|---------------------|
| 0.15 | 95.8% | 100.0% |
| **0.17** | **98.5%** | **98.6%** |
| 0.20 | 100.0% | 98.6% |
| 0.25 | 100.0% | 97.1% |
| 0.30 | 100.0% | 88.6% |

**Conclusion**: `FACE_VERIFY_THRESHOLD = 0.17` achieves an excellent balance — 98.5% self-acceptance with 98.6% non-self rejection. The 0.17 threshold sits in the gap between the self distribution tail (max 0.197) and the non-self distribution head (min 0.155), with only 2 non-self outliers below 0.17 (both at 0.155–0.158).

---

## 8. Serial Commands Reference

| Command | Description |
|---------|-------------|
| `register <name>` | Start enrollment for named person |
| `s` | Capture one sample during enrollment |
| `select <name>` | Select identity for 1:1 verification |
| `unselect` | Clear selection, back to 1:N identify |
| `identify` | Print identification result (continuous) |
| `gallery` | List all registered faces |
| `saveface` | Save face gallery to SD NAND |
| `loadface` | Load face gallery from SD NAND |
| `clearface` | Clear face gallery in RAM only |
| `cam status` | Print camera Gain/Exposure/WB/AEC/AWB |
| `cam gain <val>` | Set manual gain 0~72000 mdB |
| `cam exp <val>` | Set manual exposure 0~33266 us |
| `cam wb <temp>` | WB: 0=auto, 2856=A, 4000=TL84, 5000=D50, 6500=D65 |
| `cam auto <0\|1>` | 0=manual, 1=auto (AEC+AWB) |
| `crccc` | CRC32 check of network_fc weights |
| `dumpface` | One-shot hex dump of current face crop |
| `dumpface on/off` | Auto-dump face crop on every identify frame |
| `enrolldump on/off` | Auto-dump face crop on every enrollment capture |

---

## 9. File Index

| File | Purpose |
|------|---------|
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.h` | Detection/recognition parameters, data structures |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/app_x-cube-ai.c` | Model 1 post-process, Model 2 crop, identify/verify, gallery, enrollment |
| `ElectronicWarehouse/Appli/APP/AI_task.c` | AI task, serial commands, draw boxes, BGR→RGB conversion, ReID pipeline |
| `ElectronicWarehouse/Appli/APP/LV_task.c` | LVGL task, UI init, auto-unlock polling |
| `ElectronicWarehouse/Appli/APP/ui/actions.c` | UI action callbacks (camera start/stop, recognize, enroll, IO) |
| `ElectronicWarehouse/Appli/APP/ui/ui_bridge.cpp` | Bridge between AI variables and LVGL UI, `ui_poll_auto_unlock()` |
| `ElectronicWarehouse/Appli/APP/fd_blazeface_anchors.h` | 896 anchor definitions for Model 1 |
| `ElectronicWarehouse/Appli/Drivers/BSP/IMX335/imx335.c` | DCMIPP PIPE1/PIPE2 configuration, camera registers |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_f.h` | Model 1 NPU network descriptor |
| `ElectronicWarehouse/Appli/X-CUBE-AI/App/network_fc.h` | Model 2 NPU network descriptor |
