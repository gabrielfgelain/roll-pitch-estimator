# Roll and Pitch Estimator

C++17 command-line tool that estimates **roll and pitch** from a log of raw 3-axis
accelerometer readings, using the **aerospace rotation sequence** described in
NXP application note AN3461.

```
input log (time, x, y, z in mg)  →  parser  →  estimator  →  output CSV (time, roll, pitch in degrees)
```

---

## How to compile and run

### Requirements
- A C++17 compiler (tested with MSVC / Visual Studio 2022 Build Tools)
- CMake 3.10 or newer
- No external dependencies

### Build
From the repository root:

```bash
cmake -S . -B build
cmake --build build --config Release
```

The executable is created at:
- `build/Release/attitude_estimator.exe` with Visual Studio (multi-config generators)
- `build/attitude_estimator` with Makefiles / Ninja (single-config generators)

### Run

```bash
attitude_estimator <input.log> [output.csv]
```

| Argument      | Required | Description                                          |
|---------------|----------|------------------------------------------------------|
| `input.log`   | yes      | Accelerometer log (`time_stamp_ms; x; y; z`)         |
| `output.csv`  | no       | Output file (default: `attitude_output.csv`)         |

Example:

```bash
./build/Release/attitude_estimator.exe attitude_exam.log
```

```
Input:   attitude_exam.log
Samples: 5478
Output:  attitude_output.csv
Columns: time_stamp_ms (ms); roll_deg, pitch_deg (degrees, aerospace sequence, roll [-180, 180], pitch [-90, 90])
```

### Output format

`attitude_output.csv` uses `;` as separator (same as the input) and has a header line:

```
time_stamp_ms;roll_deg;pitch_deg
54741;1.28;1.57
54751;1.27;1.33
...
```

| Column          | Unit    | Range          |
|-----------------|---------|----------------|
| `time_stamp_ms` | ms      | copied from input |
| `roll_deg`      | degrees | [-180, 180]    |
| `pitch_deg`     | degrees | [-90, 90]      |

The result for the provided log is committed as [`attitude_output.csv`](attitude_output.csv).

### Messages and exit codes

| Situation                          | Message (stderr)                                       | Exit code |
|------------------------------------|--------------------------------------------------------|-----------|
| No input argument                  | `Usage: <program> <input.log> [output.csv]`            | 1         |
| Input file cannot be opened        | `Error: Failed to open file: '<path>'`                 | 1         |
| No valid sample in the input       | `Error: no valid samples found in '<path>'`            | 1         |
| Output file cannot be created/written | `Error: Failed to open file: '<path>'` / `Failed to write to file: '<path>'` | 1 |
| Invalid input line                 | `Warning: line N skipped (invalid format)` (first 10 shown, then a count) | 0 (processing continues) |
| Success                            | Summary on stdout                                      | 0         |

---

## How the task was performed

### 1. Understanding the data
The log contains 5478 samples at **100 Hz** (10 ms period, about 55 s), from an MMA8451
accelerometer at ±8g full scale, already converted to milli-g.

The first thing noticed was that **z is negative on every sample**: about −1000 mg when the
device is level, and between about −770 and −1080 mg across the whole log.
This turned out to be the key detail of the exercise (see the next section).

### 2. Method: aerospace rotation sequence (AN3461)
With no linear acceleration, the accelerometer only measures gravity, rotated into the sensor
frame. For the aerospace sequence R<sub>xyz</sub> (AN3461, eq. 25 and 26):

```
roll  = atan2(Gpy, Gpz)                     range [-180, 180]
pitch = atan2(-Gpx, sqrt(Gpy² + Gpz²))      range [-90, 90]
```

- **Units cancel:** both formulas are ratios, so mg can be used directly, and |G| does not
  need to be exactly 1g.
- **`atan2` instead of `atan`:** it picks the correct quadrant from the signs of both
  arguments, giving the full ±180° range for roll (AN3461, p. 12). It is also used for pitch,
  to avoid a division by zero.
- **Yaw is not observable** with an accelerometer alone, because a rotation about gravity
  does not change the reading.

### 3. Axis and sign convention
AN3461 defines its frame with **x forward, z pointing down**, and y completing a right-handed
system (p. 4, Fig. 3).

A real accelerometer reads **−1g** on an axis aligned with gravity (p. 3), which is exactly
what the log shows (z ≈ −1000 mg when level). AN3461 then adopts the convention that the output is
**negated**, `Gp = −raw`, so that the downward axis reads +1g (p. 5). Equations 25 and 26 are
written for this negated vector.

Substituting `Gp = −raw` gives the equations implemented:

```
roll  = atan2(−y, −z)
pitch = atan2( x, sqrt(y² + z²))
```

**Verification:** applying eq. 25/26 directly to the raw data, without the negation, gives
roll ≈ −178.7° for a level, stationary device, and the value jumps between +180° and −180°
whenever noise changes the sign of y. With the convention applied, the same sample gives
roll = +1.28° and pitch = +1.57°.

### 4. Implementation

| Module                   | Responsibility                                                  |
|--------------------------|-----------------------------------------------------------------|
| `LogParser`              | Reads the file line by line and validates each line             |
| `AttitudeEstimator`      | Pure math: one sample in, roll and pitch out                    |
| `ResultWriter`           | Writes the CSV output                                           |
| `main`                   | Command-line arguments, wiring, user messages, exit codes       |

Some design decisions:
- **Invalid lines are skipped with a warning** instead of aborting, so one corrupted line does
  not discard the whole log. Timestamps are kept in the output, so gaps stay visible. The
  program only fails if no valid sample is found.
- **Line validation:** 4 numeric fields separated by `;`, nothing extra at the end, and a
  timestamp that is non-negative and fits in `uint32_t`. Trailing `\r` (CRLF files) is
  tolerated, so the parser also works on Linux.
- **Two decimal places in the output.** The sensor resolution (~1 mg/LSB at ±8g) corresponds
  to ~0.06°, so more decimals would be false precision.
- **No `-0.00` in the output.** When y = 0 exactly, `−y` is negative zero and
  `atan2(-0.0, x)` returns `-0.0`. Values that round to zero are written as `0.00`.

### 5. Verification
- Results checked against hand calculations:

  | Log line | x, y, z (mg)   | Roll      | Pitch      |
  |----------|----------------|-----------|------------|
  | 1        | 27, -22, -982  | +1.28°    | +1.57°     |
  | 642      | 434, 12, -898  | -0.77°    | +25.79°    |

- [`tests/data/attitude_exam_corrupted.log`](tests/data/attitude_exam_corrupted.log): a copy of
  the input with 5 lines broken on purpose. All 5 are reported with the correct line numbers,
  and the remaining 5473 samples are processed.
- Error paths tested: no arguments, non-existent input file.

### 6. Results
The output looks like a bench test: the device is moved between several fixed orientations and
held still for a few seconds at each one. Pitch ranges from about −23° to +40° and roll from
about −12° to +20°. The steady segments are stable (no ±180° jumps), while the transitions
are noisy because the motion itself adds acceleration to the measurement.

---

## Limitations and possible improvements
- **Linear acceleration is not compensated** (AN3461, p. 6). Braking, turns and vibration add
  error of roughly `atan(a / g)`, e.g. 0.1g gives ≈ 5.7°. Possible improvements: a low-pass
  filter, or fusing a gyroscope through a complementary or Kalman filter.
- **Roll instability near pitch = ±90°** (AN3461, p. 12). It is not reached in this dataset.
  AN3461 eq. 38 (roll with a small factor μ) could be used if it were.
- **No calibration:** sensor offsets (bias) and gain mismatch between axes are not corrected.
- **Corrupted data policy:** the program could abort or flag the output when corruption exceeds a
  threshold (e.g. a long run of consecutive bad lines).

---

## Project structure

```
include/        public headers (Sample.h, AttitudeEstimator.h, LogParser.h, ResultWriter.h)
src/            implementations and main.cpp
tests/data/     corrupted input used for testing
attitude_exam.log      provided input
attitude_output.csv    result for the provided input
```

## Development process
- Developed with feature branches and pull requests (`feature/attitude-estimator`,
  `feature/log-parser`, `feature/output-and-cli`, `docs/readme`).
- I used an AI assistant (Claude) as a pair-programming partner: to review my code, explain C++
  concepts I was rusty on, and discuss the theory in AN3461. Design decisions were made and
  verified by me, for example by hand-checking results, reading the application note, and
  testing with corrupted input.

## References
- NXP (Freescale) AN3461, *Tilt Sensing Using a Three-Axis Accelerometer*, Rev. 6, 2013.
- NXP MMA8451Q datasheet.