while IFS= read -r x
do
	printf '%s\n' "$x"
	echo "$x" | grep "=====" > /dev/null && echo -e "\n\n\n\n\n\n" && sleep 0.1 # && clear
done
