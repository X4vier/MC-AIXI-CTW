sed -e 's/cf\/\([a-zA-Z0-9\/.-]*\)\.conf/.\/main cf\/\1.conf logs\/\1 >logs\/\1.txt 2>logs\/\1.err/' | xargs -n1 -P6 -d '\n' bash -c
