# Server Setup Guide

This project has two server options. Use the fake server for frontend/extension development, and the real server when you need actual database and AI functionality.

## Fake Server (for development)

A lightweight Node.js server that returns hardcoded responses. No database, no Gemini API key, no C++ toolchain needed.

### Prerequisites

- Node.js (v18+)

### Start

```bash
cd fake-server
npm install
npm start
```

Runs on `http://localhost:18080`. Accepts any email/password for login. The `/chat` endpoint returns fake AI responses after a short delay.

### What it does

- All endpoints match the real backend: `/signup`, `/login`, `/logout`, `/deactivate`, `/survey`, `/upload`, `/profile`, `/chat`
- `/profile` returns sample documents and survey data
- `/chat` returns a hardcoded legal-sounding response
- CORS is pre-configured for the Vue dev server (`localhost:5173`)

### When to use

- Frontend UI development
- Extension development
- Testing flows without waiting for C++ builds
- When you don't have the database connection string or Gemini API key

---

## Real Server (C++ / Crow)

The production backend with PostgreSQL, session management, and Gemini AI integration.

### Prerequisites

- Docker, **or**:
- CMake 3.20+, Ninja, a C++20 compiler (GCC/Clang)
- vcpkg with packages: `asio`, `crow`, `curl`, `libenvpp`, `libpqxx`
- System libraries: `libcurl`, `libssl`, `libpq`, `libsodium`

### Option A: Docker (recommended)

```bash
cd backend
docker build -t lexassist-backend .
docker run -p 18080:18080 \
  -e LEGALCHATBOT_CONSTRING="dbname=legalchatbot user=avnadmin password=<password> host=<host> port=<port>" \
  lexassist-backend
```

### Option B: Build locally

```bash
cd backend

# Configure (point to your vcpkg installation)
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -G Ninja

# Build
cmake --build build

# Set the database connection string
export LEGALCHATBOT_CONSTRING="dbname=legalchatbot user=avnadmin password=<password> host=<host> port=<port>"

# Run
./build/legal_chatbot
```

Runs on `http://localhost:18080`.

### Environment variables

| Variable | Required | Description |
|----------|----------|-------------|
| `LEGALCHATBOT_CONSTRING` | Yes | PostgreSQL connection string |

### Gemini API key

The API key is currently hardcoded in `backend.cpp` (line 18). Replace the placeholder before using the `/chat` endpoint:

```cpp
const std::string GEMINI_API_KEY = "replace-with-gemini-api-key";
```

---

## Running with the frontend

Both servers use port 18080. The Vue frontend already points to `http://localhost:18080`, so just start whichever server you need, then:

```bash
cd frontend/chatbot
npm install
npm run dev
```

The frontend dev server runs on `http://localhost:5173`.
