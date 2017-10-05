#./can-utils/candump -atd can0 | sed -un '/can0  464/p' | sed -un "/speed/{n;N;s/([^']*'//g;s/\n//g;s/ \!x *\([0-9]*\) x' \"\([0-9]*\).*/\1 \2/;p}" | sed -u '/^ /d' > /tmp/speed

./can-utils/candump -atd  -a can0 | sed -un '/can0  464/p' | sed -u "s/[^']*'.\([^']*\)'/\1/" | sed -un "/spee/{n;N;N;s/\n//g;s/[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\).*/\1 \2/;p}" >/tmp/speed

