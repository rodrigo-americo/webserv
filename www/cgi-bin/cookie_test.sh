#!/bin/bash
echo "Content-Type: text/plain"

visitas=0
if [ -n "$HTTP_COOKIE" ]; then
    visitas=$(echo "$HTTP_COOKIE" | sed -n 's/.*visitas=\([0-9]*\).*/\1/p')
    if [ -z "$visitas" ]; then
        visitas=0
    fi
fi

visitas=$((visitas + 1))

echo "Set-Cookie: visitas=$visitas"
echo ""
echo "Você visitou este site $visitas vez(es)."