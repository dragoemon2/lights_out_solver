# LIGHTS-OUT-SOLVER

This is a solver for the lights out puzzle which start with all lights off and the goal is to turn all lights on. 

## C++ Version

### Dependencies

- opencv
- boost

### Build

```bash
g++ lights_out_solver.cpp -o lights_out_solver `pkg-config --cflags --libs opencv4`
```

### Run

```bash
./lights_out_solver
```

## Python Version

### Dependencies

- numpy
- PIL

### Run

```bash
python lights_out_solver.py
```