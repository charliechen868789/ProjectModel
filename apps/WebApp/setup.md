# Environmental Monitor Setup Guide

## Architecture Overview

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Browser   │───▶│  lighttpd   │───▶│   WebApp    │
│             │    │   :8080     │    │   :8081     │
└─────────────┘    └─────────────┘    └─────────────┘
                          │
                          ▼
                   ┌─────────────┐
                   │Static Files │
                   │(HTML/CSS/JS)│
                   └─────────────┘
```

## Directory Structure

```
/var/www/environmental-monitor/
├── index.html                 # Main dashboard
├── css/
│   ├── main.css              # Base styles
│   └── dashboard.css         # Dashboard-specific styles
├── js/
│   ├── api.js                # API communication
│   ├── dashboard.js          # Dashboard logic
│   └── app.js                # Application initialization
└── assets/                   # Optional: images, fonts, etc.
    ├── favicon.ico
    └── logo.png
```

## Installation Steps

### 1. Install lighttpd

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install lighttpd

# CentOS/RHEL
sudo yum install lighttpd
# or
sudo dnf install lighttpd
```

### 2. Setup Web Directory

```bash
# Create web directory
sudo mkdir -p /var/www/environmental-monitor
sudo mkdir -p /var/www/environmental-monitor/css
sudo mkdir -p /var/www/environmental-monitor/js

# Set ownership
sudo chown -R www-data:www-data /var/www/environmental-monitor
sudo chmod -R 755 /var/www/environmental-monitor
```

### 3. Deploy Web Files

Copy the files to the web directory:

```bash
# Copy HTML file
sudo cp index.html /var/www/environmental-monitor/

# Copy CSS files
sudo cp css/main.css /var/www/environmental-monitor/css/
sudo cp css/dashboard.css /var/www/environmental-monitor/css/

# Copy JS files
sudo cp js/api.js /var/www/environmental-monitor/js/
sudo cp js/dashboard.js /var/www/environmental-monitor/js/
sudo cp js/app.js /var/www/environmental-monitor/js/
```

### 4. Configure lighttpd

```bash
# Backup original config
sudo cp /etc/lighttpd/lighttpd.conf /etc/lighttpd/lighttpd.conf.backup

# Copy new config
sudo cp lighttpd.conf /etc/lighttpd/lighttpd.conf

# Enable required modules
sudo lighttpd-enable-mod proxy
sudo lighttpd-enable-mod compress

# Test configuration
sudo lighttpd -t -f /etc/lighttpd/lighttpd.conf
```

### 5. Start Services

```bash
# Start lighttpd
sudo systemctl start lighttpd
sudo systemctl enable lighttpd

# Compile and start your webapp
cd /path/to/your/webapp
make
./webapp
```

### 6. Verify Setup

1. **Web Interface**: Open http://localhost:8080
2. **API Direct**: Test http://localhost:8081/api/data
3. **Check logs**: 
   ```bash
   sudo tail -f /var/log/lighttpd/error.log
   sudo tail -f /var/log/lighttpd/access.log
   ```

## API Endpoints

The webapp provides these endpoints:

- `GET /api/data` - Returns sensor data and algorithm results
- `POST /api/config` - Updates configuration
- `POST /api/command` - Executes commands (reset, etc.)

## Features

### Web Dashboard
- **Real-time data display** - Temperature, humidity, pressure
- **Environmental analysis** - Comfort index and recommendations
- **Interactive controls** - Refresh, reset, configuration
- **Auto-refresh** - Configurable update intervals
- **Responsive design** - Works on desktop and mobile

### API Features
- **RESTful design** - Clean HTTP methods and responses
- **JSON format** - All data in JSON format
- **CORS enabled** - Cross-origin requests supported
- **Error handling** - Proper HTTP status codes

## Configuration

### Webapp Configuration
Edit the webapp source to change:
- Port (default: 8081)
- Refresh intervals
- Data processing logic

### Web UI Configuration
Edit `js/api.js` to change:
- API base URL
- Request timeout
- Error handling behavior

Edit `js/dashboard.js` to change:
- Default refresh rate
- Display formats
- Animation settings

## Troubleshooting

### Common Issues

1. **"Connection failed"**
   - Check if webapp is running on port 8081
   - Verify lighttpd proxy configuration
   - Check firewall settings

2. **"404 Not Found" for API calls**
   - Verify lighttpd proxy module is enabled
   - Check lighttpd.conf proxy configuration
   - Ensure webapp is handling /api/* routes

3. **Static files not loading**
   - Check file permissions (755 for directories, 644 for files)
   - Verify lighttpd document-root setting
   - Check file paths in HTML

4. **CORS errors**
   - Verify Access-Control headers in lighttpd.conf
   - Check webapp CORS settings
   - Test with browser developer tools

### Log Files
- lighttpd access: `/var/log/lighttpd/access.log`
- lighttpd errors: `/var/log/lighttp