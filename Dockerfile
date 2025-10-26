FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    && rm -rf /var/lib/apt/lists/*

# Copy requirements and install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application files
COPY dashboard.py .
COPY index.html .

# Create a non-root user for security
RUN useradd -m -u 1000 dashboarduser && chown -R dashboarduser:dashboarduser /app
USER dashboarduser

# Expose the port
EXPOSE 8050

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD python -c "import requests; requests.get('http://localhost:8050')" || exit 1

# Run the application
CMD ["python", "dashboard.py"]
