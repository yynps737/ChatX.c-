# 多阶段构建

# 第一阶段: 编译后端
FROM ubuntu:22.04 AS backend-builder

# 避免交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# 安装依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 首先复制CMakeLists.txt文件，利用Docker缓存
COPY backend/CMakeLists.txt ./backend/

# 复制依赖相关文件
COPY backend/include ./backend/include
COPY backend/libs ./backend/libs

# 创建构建目录
RUN mkdir -p /app/backend/build

# 复制源代码
COPY backend/src ./backend/src

# 编译后端
WORKDIR /app/backend/build
RUN cmake .. && \
    cmake --build . --config Release

# 第二阶段: 编译前端
FROM node:16 AS frontend-builder

WORKDIR /app

# 首先复制package.json，利用Docker缓存
COPY frontend/package.json frontend/package-lock.json ./frontend/

# 安装依赖
WORKDIR /app/frontend
RUN npm install

# 复制前端源码
COPY frontend ./

# 构建前端
RUN npm run build

# 第三阶段: 运行时
FROM ubuntu:22.04

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libssl3 \
    zlib1g \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# 创建应用目录
WORKDIR /app

# 从构建阶段复制编译好的程序
COPY --from=backend-builder /app/backend/build/ChatX /app/
COPY --from=frontend-builder /app/frontend/public /app/public

# 创建数据目录
RUN mkdir -p /app/data

# 环境变量
ENV PORT=9001
ENV HOST=0.0.0.0
ENV DB_PATH=/app/data/chatx.db
ENV NODE_ENV=production

# 复制环境变量示例文件
COPY backend/.env.example /app/.env.example

# 暴露端口
EXPOSE 9001

# 健康检查
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:9001/health || exit 1

# 运行命令
CMD ["/app/ChatX"]