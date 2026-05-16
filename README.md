# Manufacturing Station Simulator

A C++ console application that simulates a simple manufacturing control terminal.

The program allows users to:

- Configure a processing flow such as `A->B->C`
- Input raw material values (integer) and process them through the flow
- View all produced product values
- View processing counts for each station
- Save and restore application state using a local JSON file

---

## Features

- Object-oriented station design
- Extensible architecture for adding new stations
- Configurable processing flows
- Product history tracking
- Station processing count tracking
- JSON-based save and restore
- Input validation and graceful error handling

---
# System Deployment

## Prerequisites

Before building and running this project, make sure the following tools are installed:

- Git
- CMake 3.16 or later
- A C++ compiler

On Windows, Visual Studio 2022 is recommended. During installation, select the **Desktop development with C++** workload.

## Verify The Installation

Open a terminal and run the following commands to confirm that Git and CMake are installed correctly:

```sh
git --version
cmake --version
```

## Download The Project

Run the following commands in a terminal:

```sh
git clone https://github.com/JeremyChouJI/ADEQuiz.git
cd ADEQuiz
```

## Generate The Build Files

Run the following command from the project directory:

```sh
cmake -S . -B build
```

## Build The Project

Run the following command:

```sh
cmake --build build --config Release
```

## Run The Program

Run the following command:

```sh
.\build\Release\ADEQuiz.exe
```

If the command does not work, you can also locate `ADEQuiz.exe` directly in the project folder and run it manually.
---
## Station Rules

### Station A
Adds `1` to the input value.

### Station B
Subtracts `1` from the input value.

### Station C
Keeps the value unchanged.

- If the input is odd, continue to the next station normally.
- If the input is even, skip the next station.

---

## Example

Flow:

```text
A->B->C->B->A
```

Input:

```text
2
```

Processing:

```text
A: 2 -> 3
B: 3 -> 2
C: 2 is even, skip next B
A: 2 -> 3
```

Final product:

```text
3
```

---

## Valid Flow Examples

```text
A->B
C->B->A
A->B->C->B->A
```

Invalid examples:

```text
->A
A->
A->->B
A--B
```

---

## Save File

By default, the program stores `state.json` in the same folder as the running `ADEQuiz.exe`. Keep `ADEQuiz.exe` and `state.json` together when moving the app between folders so saved history is restored consistently.

The program stores state in a JSON file, including:

- Current processing flow
- Product history
- Station processing counts

Example:

```json
{
  "flow": ["A", "B", "C", "B", "A"],
  "products": [3, 3, 8],
  "counts": {
    "A": 4,
    "B": 1,
    "C": 1
  }
}
```
