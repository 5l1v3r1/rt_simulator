# /bin/bash
tmp_file=$(mktemp)
find / -exec sync \; -exec ls {} \; &> $tmp_file