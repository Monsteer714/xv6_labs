#!/bin/bash
# newuser.sh - 在 xv6 user 目录下创建新的用户程序，并更新 Makefile 和 compile_commands.json
# Usage: ./newuser.sh <name>
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <name>"
    echo "  例: $0 mycmd  会在 user/ 下创建 mycmd.c，并注册到构建系统"
    exit 1
fi

NAME="$1"
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
CC_JSON="$PROJECT_DIR/compile_commands.json"

python3 - "$NAME" "$PROJECT_DIR" "$CC_JSON" << 'PYEOF'
import sys, os

name = sys.argv[1]
project_dir = sys.argv[2]
cc_json_path = sys.argv[3]
makefile_path = os.path.join(project_dir, "Makefile")
user_dir = os.path.join(project_dir, "user")
c_file = os.path.join(user_dir, f"{name}.c")

# ========== 1. 创建 user/<name>.c ==========
if os.path.exists(c_file):
    print(f"[跳过] {c_file} 已存在")
else:
    template = f'''#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{{
    // TODO: 在此编写你的用户程序
    printf("hello from {name}\\n");
    exit(0);
}}
'''
    with open(c_file, 'w') as f:
        f.write(template)
    print(f"[创建] {c_file}")

# ========== 2. 更新 Makefile UPROGS ==========
with open(makefile_path, 'r') as f:
    lines = f.readlines()

tag = f"$U/_{name}"
if any(tag in line for line in lines):
    print(f"[跳过] Makefile 中已存在 {tag}")
else:
    # 找到 UPROGS 列表结束位置（紧跟 UPROGS 后面的第一个空行）
    uprogs_start = None
    for i, line in enumerate(lines):
        if line.startswith("UPROGS=\\"):
            uprogs_start = i
            break

    if uprogs_start is None:
        print("[错误] 找不到 UPROGS 定义")
        sys.exit(1)

    # 找到 UPROGS 之后的第一个空行，在它前面插入
    end_line = None
    for i in range(uprogs_start + 1, len(lines)):
        if lines[i].strip() == '':
            end_line = i
            break

    if end_line is None:
        end_line = len(lines)

    # 在 end_line 位置插入新条目
    new_entry = f"\t$U/_{name}\\\n"
    lines.insert(end_line, new_entry)

    with open(makefile_path, 'w') as f:
        f.writelines(lines)
    print(f"[更新] Makefile: 添加 {tag}")

# ========== 3. 更新 compile_commands.json ==========
import json

with open(cc_json_path, 'r') as f:
    data = json.load(f)

target_file = os.path.join(project_dir, "user", f"{name}.c")
for entry in data:
    if entry.get("file") == target_file:
        print(f"[跳过] compile_commands.json 中已存在 user/{name}.c")
        break
else:
    new_entry = {
        "directory": project_dir,
        "command": (
            "riscv64-unknown-elf-gcc -Wall -Werror -O -fno-omit-frame-pointer "
            "-ggdb -gdwarf-2 -DSOL_UTIL -DLAB_UTIL -MD -mcmodel=medany "
            "-ffreestanding -fno-common -nostdlib "
            "-fno-builtin-strncpy -fno-builtin-strncmp -fno-builtin-strlen "
            "-fno-builtin-memset -fno-builtin-memmove -fno-builtin-memcmp "
            "-fno-builtin-log -fno-builtin-bzero -fno-builtin-strchr "
            "-fno-builtin-exit -fno-builtin-malloc -fno-builtin-putc "
            "-fno-builtin-free -fno-builtin-memcpy -Wno-main "
            "-fno-builtin-printf -fno-builtin-fprintf -fno-builtin-vprintf "
            "-I. -fno-stack-protector -fno-pie -no-pie "
            f"-c -o user/{name}.o {project_dir}/user/{name}.c"
        ),
        "file": target_file
    }

    # 找到最后一个 user/ 条目，插入到它后面
    insert_index = len(data)
    for i in range(len(data) - 1, -1, -1):
        if data[i]["file"].startswith(os.path.join(project_dir, "user/")):
            insert_index = i + 1
            break

    data.insert(insert_index, new_entry)

    with open(cc_json_path, 'w') as f:
        json.dump(data, f, indent=2)
        f.write('\n')
    print(f"[更新] compile_commands.json: 添加 user/{name}.c")

print()
print("===== 完成 =====")
print(f"已创建:   user/{name}.c")
print(f"Makefile: $U/_{name}")
print(f"compile_commands.json: user/{name}.c")
print()
print("接下来运行 make qemu 即可编译并测试你的程序。")
PYEOF
