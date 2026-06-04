#!/bin/bash

cd ~/Projects/CloudStorage || exit 1

# Stop old processes
#pkill cloud 2>/dev/null
#pkill cloudflared 2>/dev/null

echo "Starting backend..."

./build/cloud &
BACKEND_PID=$!

sleep 3

echo "Starting Cloudflare Tunnel..."

rm -f tunnel.log

cloudflared tunnel --url http://localhost:18080 > tunnel.log 2>&1 &

while true
do
    URL=$(grep -oE 'https://[a-zA-Z0-9.-]+\.trycloudflare\.com' tunnel.log | tail -1)

    if [ -n "$URL" ]
    then
        break
    fi

    sleep 1
done

echo ""
echo "Tunnel URL:"
echo "$URL"

cat > web/backend-url.json << EOF
{
    "apiBase": "$URL"
}
EOF

echo ""
echo "backend-url.json updated"

echo ""
echo "Updating GitHub..."

git add web/backend-url.json

git commit -m "Update tunnel URL" && git push origin main || true

echo ""
echo "GitHub updated"

echo ""
echo "Waiting for Netlify deployment..."
echo "Website: https://cloudstoragemanage.netlify.app"
echo ""
echo "Press Ctrl+C to stop"

wait
