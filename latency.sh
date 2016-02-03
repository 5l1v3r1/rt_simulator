# /bin/bash
function latency() {
    tmp_file=$(mktemp)
	find / -exec sync \; -exec ls {} \; &> $tmp_file &
	result=$!
}

for i in `seq 1 10`;
do
	latency
    pids+=$result" "
done
sudo ./simulator
kill -9 $pids
