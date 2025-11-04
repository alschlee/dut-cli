# DUT - Directory Usage Tree

디렉터리 디스크 사용량을 트리 구조로 분석하고 시각화하는 C 기반 Linux CLI 도구

## 개요

DUT(Directory Usage Tree)는 디렉터리의 디스크 사용량을 직관적인 트리 구조로 시각화한다. 파일 시스템을 재귀적으로 탐색하여 각 디렉터리와 파일의 크기를 계산하고, 이를 계층적 구조로 표현한다.

## 설치

```bash
# 설치 방법 추가 예정
```

## 사용법

```bash
dut [PATH] [OPTIONS]
```

### 인자

- `PATH` - 분석할 디렉터리 경로 (기본값: 현재 디렉터리)

### 옵션

#### 필터링 및 정렬

- `--max-depth=N` - 트리 깊이를 N 레벨로 제한
- `--min-size=SIZE` - SIZE보다 큰 항목만 표시 (예: 100M, 1G)
- `--top=N` - 각 디렉터리에서 상위 N개 항목만 표시
- `--exclude=PATTERN` - PATTERN과 일치하는 항목 제외 (반복 가능)
- `-r, --reverse` - 정렬 순서 반전 (오름차순)

#### 출력 옵션

- `--percent` - 각 항목의 전체 크기 대비 백분율 표시
- `--summary` - 각 디렉터리의 항목 수 (디렉터리 및 파일) 표시
- `--help` - 도움말 메시지 표시

### 크기 단위

모든 크기 단위는 대소문자를 구분하지 않는다다:

- `B` - 바이트
- `K`, `KB` - 킬로바이트
- `M`, `MB` - 메가바이트
- `G`, `GB` - 기가바이트
- `T`, `TB` - 테라바이트

## 사용 예시

### 기본 사용

```bash
# 현재 디렉터리 분석
dut

# 특정 디렉터리 분석
dut /home/user/projects

# 깊이 제한과 함께 분석
dut /home/user/projects --max-depth=2
```

### 필터링

```bash
# 1GB보다 큰 항목만 표시
dut . --min-size=1G

# 디렉터리당 상위 5개 항목만 표시
dut . --top=5

# 일반적인 디렉터리 제외
dut . --exclude=node_modules --exclude=.git --exclude=*.log
```

### 정렬

```bash
# 가장 작은 항목부터 표시
dut . -r

# 디렉터리당 가장 작은 5개 항목 표시
dut . --top=5 --reverse
```

### 출력 형식

```bash
# 백분율 표시
dut . --percent

# 항목 수 표시
dut . --summary

# 여러 옵션 조합
dut . --max-depth=3 --min-size=500M --percent --summary
```

## 출력 형식

### 기본 출력

```
[10.4G] /home/user/projects
├─ [5.1G] src
│   ├─ [2.2G] assets
│   │   ├─ [1.5G] images
│   │   └─ [700M] videos
│   ├─ [1.6G] components
│   └─ [1.3G] scripts
├─ [3.8G] build
│   ├─ [2.7G] output
│   └─ [1.1G] logs
└─ [1.5G] docs
    ├─ [900M] api
    └─ [600M] guides

Total: 10.4G in /home/user/projects
```

### 백분율 표시

```
[10.4G] 100.0% /home/user/projects
├─ [5.1G]  49.0% src
├─ [3.8G]  36.5% build
└─ [1.5G]  14.4% docs

Total: 10.4G in /home/user/projects
```

### 요약 정보 표시

```
[10.4G] /home/user/projects (15 items: 8 dirs, 7 files)
├─ [5.1G] src (250 items: 45 dirs, 205 files)
├─ [3.8G] build (8 items: 3 dirs, 5 files)
└─ [1.5G] docs (32 items: 8 dirs, 24 files)

Total: 10.4G, 305 items (64 directories, 241 files)
```
