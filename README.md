# HTTP Server

Multi-threaded HTTP server for static content hosting.

## Build

```bash
g++ -std=c++11 -pthread server.cpp -o server
```

## Run

```bash
./server
```

Server runs on `http://localhost:8080`

## Pages

- `http://localhost:8080` - index.html
- `http://localhost:8080/page2.html` - page2.html

## Load Testing

### Setup

1. Install Locust in virtual environment:
```bash
python3 -m venv .venv
.venv/bin/pip install locust
```

### Run Load Tests

1. Start the HTTP server (terminal 1):
```bash
./server
```

2. Start Locust (terminal 2):
```bash
.venv/bin/locust --host=http://localhost:8080
```

3. Open browser and navigate to:
```
http://localhost:8089
```

4. Configure test parameters:
   - **Number of users:** Start with 100, then increase (200, 500...)
   - **Spawn rate:** 10 users/second
   - **Duration:** 30 seconds per test

5. Click "Start swarming" and monitor metrics
