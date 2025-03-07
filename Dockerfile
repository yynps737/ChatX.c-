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
    && rm -rf /var/lib/apt/lists/*

# 复制后端代码
WORKDIR /app
COPY backend ./backend

# 创建构建目录并编译后端
RUN mkdir -p /app/backend/build && \
    cd /app/backend/build && \
    cmake .. && \
    make -j$(nproc)

# 第二阶段: 编译前端
FROM node:16 AS frontend-builder

WORKDIR /app
COPY frontend ./frontend

# 安装依赖并构建
WORKDIR /app/frontend
RUN npm install && npm run build

# 第三阶段: 运行时
FROM ubuntu:22.04

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
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

# 暴露端口
EXPOSE 9001

# 运行命令
CMD ["/app/ChatX"]
