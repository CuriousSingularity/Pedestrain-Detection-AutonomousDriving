# Multi-stage build for pedestrian detection application
FROM ubuntu:20.04 as builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies and OpenCV
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    make \
    pkg-config \
    libopencv-dev \
    libopencv-contrib-dev \
    libopencv-core4.2 \
    libopencv-imgproc4.2 \
    libopencv-imgcodecs4.2 \
    libopencv-highgui4.2 \
    libopencv-calib3d4.2 \
    libopencv-objdetect4.2 \
    libopencv-flann4.2 \
    libopencv-videoio4.2 \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-tools \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Set execute permissions for scripts
RUN chmod +x generate_design.sh

# Build the application for PC platform
RUN make clean || true

# Build with OpenCV include paths for Ubuntu 20.04
RUN make PLATFORM=PC EXTRA_CFLAGS="-I/usr/include/opencv4"

# Runtime stage
FROM ubuntu:20.04 as runtime

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies only
RUN apt-get update && apt-get install -y \
    libopencv-core4.2 \
    libopencv-imgproc4.2 \
    libopencv-imgcodecs4.2 \
    libopencv-highgui4.2 \
    libopencv-calib3d4.2 \
    libopencv-objdetect4.2 \
    libopencv-flann4.2 \
    libopencv-videoio4.2 \
    libgstreamer1.0-0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    v4l-utils \
    && rm -rf /var/lib/apt/lists/*

# Create app user for security
RUN useradd -m -s /bin/bash appuser

# Set working directory
WORKDIR /app

# Copy built executable and necessary files from builder stage
COPY --from=builder /app/exe_PC /app/
COPY --from=builder /app/Design/ /app/Design/

# Create necessary directories and set permissions
RUN mkdir -p /app/build && \
    chown -R appuser:appuser /app

# Switch to non-root user
USER appuser

# Expose any ports if needed (none specified in the original application)
# EXPOSE 8080

# Health check to verify the application is responsive
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD pgrep -f exe_PC || exit 1

# Set environment variables
ENV PLATFORM=PC
ENV TARGET_PLATFORM=3

# Default command
CMD ["./exe_PC"]
