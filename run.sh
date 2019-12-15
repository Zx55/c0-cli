./build.sh

work_dir="/home/c0-cli/toolchain/"
vm_path="/home/c0-cli/toolchain/c0-vm-cpp"
c0_path="/home/c0-cli/build/c0"

input="in.c0"

if [ ! -x "$vm_path" ]; then
    echo "no vm!"
    exit
fi
echo "found vm"

if [ ! -x "$c0_path" ]; then
    echo "no c0"
    exit
fi
echo "found c0"

cp "$c0_path" "$work_dir"
cd "$work_dir"

if [ ! -f "$input" ]; then
    echo "no input file"
    exit
fi

./c0 -c "$input" -o out

echo "output: "
echo ""

./c0-vm-cpp -r out

echo ""
echo "done!"
